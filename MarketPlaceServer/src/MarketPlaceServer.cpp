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
#include <Poco/Exception.h>
#include <Poco/Net/NetException.h>
#include <Poco/FileChannel.h>
#include <Poco/Logger.h>
#include <Poco/AutoPtr.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>


#include "MarketPlaceServer.h"
#include "MarketPlaceSys.h"
#include "ConnectionHandler.h"
#include "WaitingSocketReactor.h"
#include "FoundationException.h"
#include "MarketPlaceException.h"


using Poco::Logger;
using Poco::FileChannel;
using Poco::AutoPtr;
using Poco::PatternFormatter;
using Poco::FormattingChannel;

namespace ChoiceNet
{

namespace Eco
{

    MarketPlaceServer::MarketPlaceServer():
    _helpRequested(false),
    _marketSubsystemPtr(NULL)
    {
    }

    MarketPlaceServer::~MarketPlaceServer()
    {
		std::cout << "terminating MarketPlaceServer" << std::endl;
		if (_marketSubsystemPtr != NULL)
			delete _marketSubsystemPtr;
    }

    void MarketPlaceServer::initialize(Poco::Util::Application& self)
    {

        Poco::Util::ServerApplication::initialize(self);

    }

    void MarketPlaceServer::uninitialize()
    {
        Poco::Util::ServerApplication::uninitialize();
    }

    void MarketPlaceServer::defineOptions(Poco::Util::OptionSet& options)
    {
        Poco::Util::ServerApplication::defineOptions(options);

        options.addOption(
        Poco::Util::Option("help", "h", "display argument help information")
            .required(false)
            .repeatable(false)
            .callback(Poco::Util::OptionCallback<MarketPlaceServer>(
                this, &MarketPlaceServer::handleHelp)));
    }

    void MarketPlaceServer::handleHelp(const std::string& name,
                    const std::string& value)
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader(
            "A server that intermediate between agents.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    MarketPlaceSys * MarketPlaceServer::getMarketPlaceSubsystem()
    {
		return _marketSubsystemPtr;
		throw Poco::NotFoundException("The subsystem has not been registered", typeid(MarketPlaceSys).name());
	}

    int MarketPlaceServer::main(const std::vector<std::string>& args)
    {

        // Initialize the system.
        try
        {

        	loadConfiguration();
        	_marketSubsystemPtr  = new MarketPlaceSys();
        	(*_marketSubsystemPtr).initialize();


        } catch	(FoundationException &e){
        	std::cout << e.message() << std::endl;
        	terminate();
        } catch (Poco::NotFoundException &e){
        	std::cout << e.message() << std::endl;
        	terminate();
        } catch (MarketPlaceException &e){
        	std::cout << e.message() << std::endl;
        	terminate();
        }


		AutoPtr<FileChannel> fileChannel(new FileChannel("MarketPlaceServer.log"));
		AutoPtr<PatternFormatter> formatter(new PatternFormatter("%d-%m-%Y %H:%M:%S %s: %t"));
		AutoPtr<FormattingChannel> formattingChannel(new FormattingChannel(formatter, fileChannel));

		fileChannel->setProperty("rotateOnOpen", "true");
		Poco::Logger& logger = Poco::Util::ServerApplication::logger();
		logger.setChannel(formattingChannel);
		logger.setLevel(Poco::Message::PRIO_INFORMATION);

		// Reads from the configuration the listening port
		unsigned short port = (unsigned short)
						config().getInt("listening_port", 5555);

		// Server Socket
		Poco::Net::ServerSocket svs(port);
		// Reactor-Notifier
		WaitingSocketReactor reactor;
		// Server-Acceptor
		Poco::Net::SocketAcceptor<ConnectionHandler> acceptor(svs, reactor);
		// Threaded Reactor
		Poco::Thread thread;
		thread.start(reactor);

		// Sends the port for start listening for clock periods
		std::string type = config().getString("type", "market_place");

		MarketPlaceSys *sys = getMarketPlaceSubsystem();
		(*sys).addAsClockListener((Poco::UInt16) port, type);
		// Wait for CTRL+C
		waitForTerminationRequest();

		// Stop reactor
		reactor.stop();
		thread.join();
		return Poco::Util::ServerApplication::Application::EXIT_OK;
		return Poco::Util::ServerApplication::EXIT_OK;
    }

} /// End Eco namespace

}  /// End ChoiceNet namespace
