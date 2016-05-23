#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Thread.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/OptionCallback.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/Subsystem.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/AutoPtr.h>
#include <Poco/Timer.h>
#include <Poco/Stopwatch.h>
#include <Poco/FileChannel.h>
#include <Poco/Logger.h>
#include <Poco/AutoPtr.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>


#include "ClockServer.h"
#include "ClockSys.h"
#include "WaitingSocketReactor.h"
#include "ConnectionHandler.h"
#include "TimerNotification.h"
#include "Service.h"
#include "DecisionVariable.h"
#include "FoundationException.h"


using Poco::Logger;
using Poco::FileChannel;
using Poco::AutoPtr;
using Poco::PatternFormatter;
using Poco::FormattingChannel;

namespace ChoiceNet
{

namespace Eco
{

    ClockServer::ClockServer(): 
	_helpRequested(false),
	_clockSysPtr(NULL)
    {
    }

    ClockServer::~ClockServer()
    {
		if (_clockSysPtr != NULL)
			delete _clockSysPtr;
    }

    void ClockServer::initialize(Poco::Util::Application& self)
    {
        try{
        	loadConfiguration();
        	_clockSysPtr = new ClockSys();
        	(*_clockSysPtr).initialize(self);
        } catch(FoundationException &e) {
        	std::cout << e.message() << std::endl;
        	terminate();
        }
        Poco::Util::ServerApplication::initialize(self);               
    }

    void ClockServer::uninitialize()
    {
        Poco::Util::ServerApplication::uninitialize();
    }

    void ClockServer::defineOptions(Poco::Util::OptionSet& options)
    {
        Poco::Util::ServerApplication::defineOptions(options);

        options.addOption(
        Poco::Util::Option("help", "h", "display argument help information")
            .required(false)
            .repeatable(false)
            .callback(Poco::Util::OptionCallback<ClockServer>(
                this, &ClockServer::handleHelp)));
    }

    void ClockServer::handleHelp(const std::string& name, 
                    const std::string& value)
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader(
            "A server that establish quality parameters and routes.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    ClockSys* ClockServer::getClockSubsystem()
    {
		if (_clockSysPtr != NULL)
		{
			return _clockSysPtr;
		}
		throw Poco::NotFoundException("The subsystem has not been registered", typeid(ClockSys).name());
	}

    int ClockServer::main(const std::vector<std::string>& args)
    {
        if (!_helpRequested)
        {
			

			AutoPtr<FileChannel> fileChannel(new FileChannel("ClockServer.log"));
			AutoPtr<PatternFormatter> formatter(new PatternFormatter("%d-%m-%Y %H:%M:%S %s: [%p] %t"));
			AutoPtr<FormattingChannel> formattingChannel(new FormattingChannel(formatter, fileChannel));
			fileChannel->setProperty("rotateOnOpen", "true");
			
			Poco::Util::ServerApplication::logger().setChannel(formattingChannel);
			Poco::Util::ServerApplication::logger().setLevel("debug");

			Poco::Logger& logger = Poco::Util::ServerApplication::logger();
			
			poco_warning(logger, "This is a warning");
			

        	// Reads from the configuration the listening port
		   unsigned short port = (unsigned short)
					config().getInt("listening_port", 3333);
					
		   // Server Socket
		   Poco::Net::ServerSocket svs(port);
		   // Reactor-Notifier
		   WaitingSocketReactor reactor;
		   // Server-Acceptor
		   Poco::Net::SocketAcceptor<ConnectionHandler> acceptor(svs, reactor);
		   // Threaded Reactor
		   Poco::Thread thread;
		   thread.start(reactor);
		   
		   // Starts timer events 
		   // Get the time for each interval
		   unsigned short interval = (unsigned short)
					config().getInt("time_intervals", 3000);

		   // Get the number of intervals for making a round of bids
		   unsigned short bid_intervals = (unsigned short)
					config().getInt("intervals_for_bid", 3);
					
		   // Get the number of intervals_for_service_completion
		   unsigned short complete_intervals = (unsigned short)
					config().getInt("intervals_for_service_completion", 1);
		   
					
		   Poco::Timer timer(interval, interval);
		   TimerNotification notification(bid_intervals, complete_intervals);
		   Poco::TimerCallback<TimerNotification> callback(notification, &TimerNotification::onEndPeriod);
		   timer.start(callback);
		   
		   // Wait for CTRL+C
		   waitForTerminationRequest();
		   getClockSubsystem()->broadcastTerminate();
		   // Stop timer
		   timer.stop();
		   // Stop reactor
		   reactor.stop();
		   int sleptime = (interval * bid_intervals) / 1000;
		   std::cout << "sleeping for: " << sleptime <<  std::endl;
		   sleep(sleptime);
			
		   thread.join();		   
		  
		   
		   return Poco::Util::ServerApplication::Application::EXIT_OK; 
        }
        return Poco::Util::ServerApplication::EXIT_OK;
    }

} /// End Eco namespace

}  /// End ChoiceNet namespace
