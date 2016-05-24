#include <vector>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/NetException.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Types.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/StringTokenizer.h>
#include <sstream>
#include <string>
#include <limits.h>
#include <unistd.h>
#include <Poco/LogStream.h>



#include "FoundationSys.h"
#include "ClockSys.h"
#include "ClockServerException.h"
#include "FoundationException.h"
#include "PointSetDemandForecaster.h"
#include "DemandForecaster.h"
#include "TrafficConverter.h"
#include "SimplestTrafficConverter.h"
#include "Message.h"


using Poco::LogStream;

namespace ChoiceNet
{
namespace Eco
{

ClockSys::ClockSys(void): FoundationSys(CLOCK_SERVER),
_period(1),
_interval(0),
p_cName('C'),
_executionNumber(0)
{
}

ClockSys::~ClockSys(void)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Disconnecting the Event Discrete System");

    // Release the memory assigned to listeners ( Pointers referenced by Listener Id)
    
    std::map<std::string, Listener *>::iterator it_listeners_byId;
    it_listeners_byId = _listeners_by_id.begin();
    while ( it_listeners_byId != _listeners_by_id.end())
    {
		_listeners_by_id.erase(it_listeners_byId);
		++it_listeners_byId;
	}

    // Release the memory assigned to listeners ( Pointers referenced by socket address)
    Listeners::iterator it_listeners;
    it_listeners = _listeners.begin();
    while ( it_listeners != _listeners.end())
    {
		(*(it_listeners->second)).Disconnect();
		delete it_listeners->second;
		_listeners.erase(it_listeners);
		++it_listeners;
	}
		
	app.logger().information("Event Discrete System Finished");
}

void ClockSys::initialize(Poco::Util::Application &app)
{
    app.logger().information("Initialization the Event Discrete System");
    unsigned bid_periods = (unsigned)
                app.config().getInt("bid_periods", 11);


	// Get the interval to send information
	unsigned short send_interval = (unsigned short)
					app.config().getInt("interval_for_customer_activation", 0);

	_interval_for_customer_activation = send_interval;
	

	// Get the number of intervals for making a round of bids
	unsigned short intervals_per_cycle = (unsigned short)
					app.config().getInt("intervals_per_cycle", 2);
	
	// initialize variable
	_intervals_per_cycle = intervals_per_cycle;

	FoundationSys::initialize(app, bid_periods, 0);
	app.logger().debug("Event Discrete System initialized");
          
}

const char* ClockSys::name() const
{
    return "SUBsystem Clock ";
}

bool ClockSys::isAlreadyListener(std::string idListener)
{
	bool val_return;
	if (_listeners_by_id.count(idListener) > 0)
	    val_return = true;
	else
		val_return = false;
	return val_return;
}

bool ClockSys::isAlreadyListener(Poco::Net::SocketAddress socketAddress)
{
	bool found = false;
	Listeners::iterator it;
	it = _listeners.find(socketAddress);
	if ( it != _listeners.end())
	{
		found = true;
	}
	return found;
	
}

void ClockSys::getMessage(Poco::FIFOBuffer & fifoIn, 
						  int len, Message &message )
{
	// If the socket address corresponds to a listener, then the message size
	// could be unlimited, when it is not a listener then we have as its 
	// limit 1024.
	std::string receiveString;
	std::size_t i = 0;
	for (; i < fifoIn.used() ; ++i){
		receiveString += (*(fifoIn.begin() + i));
	}
	fifoIn.drain(len);
	message.setData(receiveString);
	std::cout << "Data accumulated is:"  << receiveString << std::endl;
}


void ClockSys::addStagedData(Poco::Net::SocketAddress socketAddress,
				   Poco::FIFOBuffer & fifoIn, 
				   int len)
{
	// If the socket address corresponds to a listener, then the message size
	// could be unlimited, when it is not a listener then we have as its 
	// limit 1024.
	
	bool found = false;
	Listeners::iterator it;
	it = _listeners.find(socketAddress);
	if ( it != _listeners.end())
	{
		(*(it->second)).addStreamStagedForProcessing(fifoIn,len);
	}
	else
	{
		throw ClockServerException("The agent is not a listener", 303);
	}
}


void ClockSys::insertListener(std::string idListener, 
							 Poco::Net::SocketAddress socketAddress,
							 Message & messageResponse )
{
	if (isAlreadyListener(idListener))
	{
		throw ClockServerException("The agent is already a listener", 302);
	}
	else
	{
		std::cout << "Insert Listener";
		Listener *listener = new Listener(idListener, socketAddress);
		
		_listeners.insert( std::pair<Poco::Net::SocketAddress, Listener *>(socketAddress,listener));
		_listeners_by_id.insert( std::pair<std::string, Listener *>(idListener,listener));
		
		std::cout << "Size:" << _listeners.size() << std::endl;
		messageResponse.setResponseOk();
	}
}

void ClockSys::startListening(Poco::Net::SocketAddress socketAddress, 
							 Poco::UInt16 port, std::string type,
							 Message & messageResponse)
{
	Listeners::iterator it;
	it = _listeners.find(socketAddress);
	if (it != _listeners.end())
	{
		Poco::Net::SocketAddress sa = (*(it->second)).getSocketAddress();
		try
		{
			std::cout << "Found the provider" << std::endl;
			Poco::Net::SocketAddress sockadd(sa.host(), port);
			(*(it->second)).Connect(sockadd);
			(*(it->second)).setType(type);
			insertListenerBytype(type, (*(it->second)).getId());
			messageResponse.setResponseOk();
		} catch(const Poco::InvalidArgumentException &ex) {
			throw ClockServerException("Invalid host", 307);
		} catch(const Poco::Net::ServiceNotFoundException &ex){
			throw ClockServerException("Invalid port", 301);
		}		
	}
    else
    {
		throw ClockServerException("The agent is not connected", 302);
	}
	std::cout << "Listening" << socketAddress.toString()<< "Connected" << std::endl;
}

void ClockSys::insertListenerBytype(std::string type, std::string listenerId)
{
	std::map<std::string, std::vector<std::string> >::iterator it;
	it = _listeners_by_type.find(type);
	if (it == _listeners_by_type.end())
	{
		std::vector<std::string> list;
		_listeners_by_type.insert(std::pair<std::string, std::vector<std::string> > (type, list));
		
	}
	it = _listeners_by_type.find(type);
	(it->second).push_back(listenerId);	
}

void ClockSys::sendCurrentPeriod(Poco::Net::SocketAddress socketAddress,
								 Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());

	lstr << "Entering send current period" << socketAddress.toString() << std::endl;

	int val_return = -1;
	Listeners::iterator it;
	it = _listeners.begin();
	bool found = false;
    std::string sockAddrPar = socketAddress.toString();
    
	while( (it!=_listeners.end()) && (found ==false) )
	{
		Poco::Net::SocketAddress sa = (*(it->second)).getSocketAddress();
		std::string sockAddrStr = sa.toString();
		
		if ( sockAddrPar.compare(sockAddrStr) == 0 ){
            // Builds the message with the current period
            messageResponse.setResponseOk();
            messageResponse.setParameter("Period", Poco::NumberFormatter::format(_period));
            found = true;
		}
		++it;
	}
    if (found == false){
		throw ClockServerException("The agent is not inscribed as listener", 303);
	}

    lstr << "Ending Current period sent to" << socketAddress.toString() << std::endl;

}

bool ClockSys::getMessage(Poco::Net::SocketAddress socketAddress, 
				Message & message)
{
	bool val_return = false;
	Listeners::iterator it;
	it = _listeners.find(socketAddress);
	if ( it != _listeners.end())
	{
		val_return = (*(it->second)).getMessage(message);
	}
	else
	{
		throw ClockServerException("The agent is not a listener", 303);
	}
	return val_return;
}

void ClockSys::broadcastPeriodEnd(void)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());
	lstr << "In broadcastPeriodEnd" << _period << std::endl;
    
    // Builds the message with the current period
    Message endPeriod;
    Method method = end_period;
    endPeriod.setMethod(method);
    endPeriod.setParameter("Period", Poco::NumberFormatter::format(_period));
    
    // Only send the broadcast end period to market server listeners.
	std::map<std::string, std::vector<std::string> >::iterator it_market_server;
	it_market_server = _listeners_by_type.find("market_place");
	if ( it_market_server != _listeners_by_type.end() )
	{
		std::vector<std::string> list = it_market_server->second;
		std::vector<std::string>::iterator it_strings;
		it_strings = list.begin();
		
		while (it_strings != list.end())
		{
			std::map<std::string, Listener *>::iterator it = _listeners_by_id.find(*it_strings);
			if( it != _listeners_by_id.end() )
			{
				if ((*(it->second)).getStatus() == 1 ) // the listener is connected
				{
					(*(it->second)).write (endPeriod.to_string());
				}
			}	
			++it_strings;
		}
	}

	lstr << "Ending broadcastPeriodEnd" << _period << std::endl;
}

void ClockSys::activateCustomers(int period)
{ 

	Poco::Util::Application& app = Poco::Util::Application::instance();

    // Send the messsage to consumer listeners.
    double new_demand = 0;
    unsigned num_agents = 0;
    ListenerType type = CONSUMER;
    _services_activation.clear();
   
    // Forecast the demand for the new period
    for(int i=0; i < _services_to_execute.size(); ++i)
    {	
		new_demand = 0;
		Service * service = getService(_services_to_execute[i]);
		new_demand = service->getForecast(period);
		num_agents = service->getRequiredAgents(new_demand);

		app.logger().debug(Poco::format("Demand:%f Num Agents:%d", new_demand, (int) num_agents) );
				
		if (num_agents > 0) 
		{
			// Builds the message with the current period
			Message * activate = new Message();
			Method method_act = activate_consumer;
			activate->setMethod(method_act);
			activate->setParameter("Service", service->getId());
			activate->setParameter("Period", (int) period);    
			activate->setParameter("Quantity", Poco::NumberFormatter::format(new_demand / num_agents));
			// Creates the structure.
			ServiceActivation activation;
			activation.num_agents = num_agents;
			activation.message_to_send = activate;
			_services_activation.insert(std::pair<std::string, ServiceActivation> (service->getId(), activation));
		}
	}
	
	Listeners::iterator it;
	unsigned num_initiated = 0;
	it = _listeners.begin();
	while( it != _listeners.end() )
	{
					  
		if ( ( (*(it->second)).getStatus() == 1 ) and 
			 ((*(it->second)).getType() == type )  )
		{ 
			// the listener is connected and is consumer.

			 std::cout << "Sending the activation message" << std::endl;
			 std::cout << "Listener:" << (*(it->second)).getId() << "Status:" 
					  << (*(it->second)).getStatus() << "Type:" 
				      << (*(it->second)).getType() << std::endl; 

			Message * message = getActivationMessage();
				
			if (message != NULL)
			{
				(*(it->second)).write (message->to_string());
				decreaseActivationMessageCount(message->getParameter("Service"));
			}
			else
			{
				// All required agents have been activated.
				break;
			}
		}
		++it;
	}

}

void ClockSys::broadcastPeriodStart(void)
{
	Listeners::iterator it;

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug(Poco::format("Start broadcastPeriodStart %d", (int) _period));

    
    ListenerType type = CONSUMER;
   
    // Sends the message for the rest of the listeners.
    // Send the messsage to all connected listeners.
    // Builds the message with the current period
    Message startPeriod;
    Method method = start_period;
    startPeriod.setMethod(method);
    startPeriod.setParameter("Period", Poco::NumberFormatter::format(_period));
    it = _listeners.begin();
	while( it!=_listeners.end() )
	{
		if ( ( (*(it->second)).getStatus() == 1 ) and 
		     ((*(it->second)).getType() != type )  ){ 
			// the listener is connected and it is not consumer.
			(*(it->second)).write (startPeriod.to_string());			
		}
		++it;
	}
   
	if ((_period % _intervals_per_cycle) == _interval_for_customer_activation){
		activateCustomers(_period / _intervals_per_cycle);
	}
  
	app.logger().debug(Poco::format("Ending broadcastPeriodStart %d", (int) _period));
       
}

Message * ClockSys::getActivationMessage(void)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());
	lstr << "Start getActivationMessage" << std::endl;

	std::map<std::string, ServiceActivation>::iterator it;
   it = _services_activation.begin();
   if (it != _services_activation.end())
   {
	 ServiceActivation activation = it->second;
	 return activation.message_to_send;
   }
   else
   {
	  return NULL;
   }

   lstr << "Ending getActivationMessage" << std::endl;

}

void ClockSys::decreaseActivationMessageCount(std::string serviceId)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());
	lstr << "Start decreaseActivationMessageCount" << std::endl;

	std::map<std::string, ServiceActivation>::iterator it;
	it = _services_activation.find(serviceId);
	if (it != _services_activation.end())
	{
		(it->second).num_agents = (it->second).num_agents - 1;
		if ((it->second).num_agents <= 0 )
		{
			delete (it->second).message_to_send;
			_services_activation.erase(it);
		}
	}

	lstr << "Ending decreaseActivationMessageCount" << std::endl;

}

void ClockSys::broadcastTerminate(void)
{
	Listeners::iterator it;
	

	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());
	lstr << "Start broadcastTerminate" << _period << std::endl;

	
    // Builds the message with the current period
    Message m_disconnect;
    Method method = disconnect;
    m_disconnect.setMethod(method);

    // Send the messsage to all connected listeners.
	for ( it = _listeners.begin(); it!=_listeners.end(); ++it )
	{
		
		if ((*(it->second)).getStatus() == 1 ) // the listener is connected
		{
			try
			{
				(*(it->second)).write (m_disconnect.to_string());
			}
			
			catch (FoundationException &e)
			{
				Poco::Util::Application& app = Poco::Util::Application::instance();
				std::string msg = "The listener ";
				msg.append((*(it->second)).getId());
				msg.append("is not listening anymore");
				app.logger().information( msg );
				continue;
			}
			catch (...) {
				
				Poco::Util::Application& app = Poco::Util::Application::instance();
				app.logger().information( "Other exception" );
				continue;
			}
		}
		
	}	

	lstr << "Ending broadcastTerminate" << _period << std::endl;

}

int ClockSys::getInterval(void)
{
	return _interval;
}

void ClockSys::incrementInterval(void)
{
	_interval = _interval + 1;
}

int ClockSys::getPeriod(void)
{
	return _period;
}

void ClockSys::incrementPeriod(void)
{
	_period = _period + 1;
}

void ClockSys::deleteListener( Poco::Net::SocketAddress socketAddress,
						      Message & messageResponse )
{    

	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());
	lstr << "Start deleteListener" << _period << std::endl;

	// Find the listener
	Listeners::iterator it;
	it = _listeners.begin();
	bool found = false;
    std::string sockAddrPar = socketAddress.toString();
	while( (it!=_listeners.end()) && (found ==false) )
	{
		Poco::Net::SocketAddress sa = (*(it->second)).getSocketAddress();
		std::string sockAddrStr = sa.toString();
		if ( sockAddrPar.compare(sockAddrStr) == 0 ){
		   found ==true;
		   break;
		}
		++it;
	}
    
    if (found == true){
		// Disconnect the socket that is waiting for periods
		(*(it->second)).Disconnect();
		// Erase the agent from the list of listeners
		_listeners.erase(it);
		messageResponse.setResponseOk();
	}
	else
	{
	    // Build an exception to the agent saying that it is not a current listener.
	    throw ClockServerException("The agent is not inscribed as listener", 303);
	}

	lstr << "Ending deleteListener" << _period << std::endl;

}

void ClockSys::getServices(std::string serviceId, Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());
	lstr << "Start getServices" << _period << std::endl;

	messageResponse.setResponseOk();
	// creates an empty xml document
	// Creates an xml message with element "bestBids" as root
	Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
	Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement("getServices");
	pDoc->appendChild(pRoot);
	
	// std::cout << "Requested service:" << serviceId << std::endl;
		
	ServiceContainer::iterator it;
	it = _services.begin();
	while (it != _services.end()) 
	{
		if ((it->first).compare(serviceId) == 0 )
			(*(it->second)).to_XML(pDoc, pRoot);
		++it;
	}
    // Writes the message to string
	Poco::XML::DOMWriter writer;
	writer.setNewLine("\n");
	writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
	std::stringstream  output; 
	writer.writeNode(output, pDoc);	
	messageResponse.setBody(output.str());
	// std::cout << "message: " << std::endl << output.str() << std::endl;

	lstr << "Ending getServices" << _period << std::endl;

}


    
void ClockSys::getServices(Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	LogStream lstr(app.logger());
	lstr << "Start getServices" << _period << std::endl;


	messageResponse.setResponseOk();
	// creates an empty xml document
	// Creates an xml message with element "bestBids" as root
	Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
	Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement("getServices");
	pDoc->appendChild(pRoot);
	
	ServiceContainer::iterator it;
	it = _services.begin();
	while (it != _services.end()) {
		(*(it->second)).to_XML(pDoc, pRoot);
		++it;
	}
    // Writes the message to string
	Poco::XML::DOMWriter writer;
	writer.setNewLine("\n");
	writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
	std::stringstream  output; 
	writer.writeNode(output, pDoc);	
	messageResponse.setBody(output.str());

	lstr << "Ending getServices" << _period << std::endl;

}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
