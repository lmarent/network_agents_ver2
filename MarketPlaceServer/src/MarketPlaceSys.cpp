#include <vector>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Exception.h>
#include <Poco/Net/NetException.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Types.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/LogStream.h>


#include "MarketPlaceException.h"
#include "FoundationException.h"
#include "MarketPlaceSys.h"
#include "Message.h"
#include "Bid.h"
#include "Purchase.h"


namespace ChoiceNet
{
namespace Eco
{

using Poco::NumberFormatter;

MarketPlaceSys::MarketPlaceSys(void):
FoundationSys(MARKET_SERVER),
_clockSocket(NULL),
_current_bids(NULL),
_current_purchases(NULL),
_intervals_per_cycle(0),
_send_interval(0),
_pool(NULL)
{
	Poco::Data::MySQL::Connector::registerConnector();
}

MarketPlaceSys::~MarketPlaceSys(void)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information("Eliminating the market place system");

	// Release the memory assigned to socket and current bids pointers

	if (_clockSocket != NULL)
		(*_clockSocket).shutdown();
		(*_clockSocket).close();
		delete _clockSocket;

	if (_current_bids != NULL)
		delete _current_bids;

	if (_current_purchases != NULL)
		delete _current_purchases;

	app.logger().debug("Eliminating bids registered");
	// erase elements in the container bids to broadcast.
	BidContainer::iterator it;
	for (it = _bids_to_broadcast.begin(); it != _bids_to_broadcast.end() ; ++it)
	{
		_bids_to_broadcast.erase(it);
	}

	// Release the memory assigned to the bid container
	BidContainer::iterator it_container_bid;
	for (it_container_bid = _bids.begin(); it_container_bid != _bids.end() ; ++it_container_bid)
	{
		delete it_container_bid->second;
		_bids.erase(it_container_bid);
	}

	app.logger().information("Eliminating listeners registered");
	// Release the memory assigned to agents listener
	Listeners::iterator it_listeners;
	it_listeners = _listeners.begin();
	while( (it_listeners != _listeners.end())  )
	{
		// Disconnect the sockets to agents.
		(*(it_listeners->second)).Disconnect();
		delete it_listeners->second;
		_listeners.erase(it_listeners);
		++it_listeners;
	}


	app.logger().information("Eliminating providers registered");
	// Release the memory assigned to providers
	std::map<std::string, Provider *>::iterator it_providers;
	it_providers = _providers.begin();
	while ( it_providers != _providers.end() )
	{
		delete it_providers->second;
		_providers.erase(it_providers);
		++it_providers;
	}


	app.logger().information("Eliminating purchases registered");
	// Release the memory assigned to purchase objects
	PurchaseHistory::iterator it_history;
	it_history = _purchase_history.begin();
	while( (it_history != _purchase_history.end())  )
	{
		delete it_history->second;
		_purchase_history.erase(it_history);
		++it_history;
	}

	app.logger().debug("Disconnecting from the database");
	if (_pool != NULL)
		delete _pool;

	Poco::Data::MySQL::Connector::unregisterConnector();

	app.logger().information("Eliminating the market sys - Finished");
}

void MarketPlaceSys::sendMessageToClock(Message & message, Message & response)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Starting sendMessageToClock");

	std::string msgStr = message.to_string();
	char * message_char = new char [msgStr.length()];
	strcpy (message_char, msgStr.c_str() );
	(*_clockSocket).sendBytes(message_char, msgStr.length());

	// Receive the response and based on that continues
	char buffer[1024];
	int bytes_received = (*_clockSocket).receiveBytes(buffer, sizeof(buffer)-1);
	std::string recvline(buffer);
	response.setData(recvline);

	app.logger().debug("ending sendMessageToClock");
}

void MarketPlaceSys::initialize()
{
    int statusCd = 0;
	Poco::Util::Application& app = Poco::Util::Application::instance();
    app.logger().debug("Initialization market place System");

    unsigned short port = (unsigned short)
                app.config().getInt("clock_port", 3333);

    unsigned pareto_fronts_to_send = (unsigned)
                app.config().getInt("pareto_fronts_to_send", 3);

    Poco::UInt16 u16Port = (Poco::UInt16) port;

	// Get the interval to send information
	unsigned short send_interval = (unsigned short)
					app.config().getInt("send_information_on_interval", 1);

	_send_interval = send_interval;


	// Get the number of intervals for making a round of bids
	unsigned short intervals_per_cycle = (unsigned short)
					app.config().getInt("intervals_per_cycle", 2);

	// initialize variable
	_intervals_per_cycle = intervals_per_cycle;

	try{//
		// Connection string to POCO
		std::string db_host = (std::string)
					app.config().getString("db_host");

		unsigned short db_port = (unsigned short)
					app.config().getInt("db_port",3306);

		std::string db_user = (std::string)
					app.config().getString("db_user","root");

		std::string db_password = (std::string)
					app.config().getString("db_password","password");

		std::string db_name = (std::string)
					app.config().getString("db_name","Network_Simulation");

		std::string sPort = Poco::NumberFormatter::format(db_port);
		std::string connectionStr = "host=" + db_host + ";port=" + sPort + ";user=" + db_user + ";password=" + db_password + ";db=" + db_name;

		std::cout << "connectionStr:" << connectionStr << std::endl;
		app.logger().information("Connecting with the database");
		_pool = new Poco::Data::SessionPool("MySQL", connectionStr);

		if (_current_bids == NULL){
			_current_bids = new BidInformation();
		}


	} catch (Poco::NotFoundException &e) {
    	throw MarketPlaceException("Database connection information not found");
	} catch (Poco::InvalidArgumentException &e) {
		throw MarketPlaceException(e.what(), e.code());
	}


	FoundationSys::initialize(app, 0, pareto_fronts_to_send);

    try{
    	std::string clock_address = app.config().getString("clock_server_address");

    	// Establish the connection with the clock server.
    	Poco::Net::IPAddress ipadd(clock_address, Poco::Net::IPAddress::IPv4);
    	Poco::Net::SocketAddress sockadd(ipadd, u16Port);

		_clockSocket = new Poco::Net::StreamSocket(sockadd);

    } catch (Poco::NotFoundException &e) {
    	throw MarketPlaceException("Clock server address not found");
	} catch (Poco::InvalidArgumentException &e) {
		throw MarketPlaceException(e.what(), e.code());
	} catch (Poco::Net::ConnectionRefusedException &e) {
		throw MarketPlaceException("Connection refused by ClockServer", e.code());
	}

    try {
    	std::string name = app.config().getString("name");
		// Initialize the name of the market place
		p_cName = name;


		Message connect_msg;
		Message response;
		Method method = connect;
		connect_msg.setMethod(method);
		connect_msg.setParameter("Agent", p_cName);
		sendMessageToClock(connect_msg, response);

		// If the message is not ok, show the message description and raise an exception
		if (!(response.isMessageStatusOk())){
			std::string statusDescr = response.getParameter("Status_Description");
			throw MarketPlaceException(statusDescr, statusCd);
		}
		// Finally get the current period
		Message current_period_msg;
		Message response2;
		Method meth_cur = get_current_period;
		current_period_msg.setMethod(meth_cur);
		sendMessageToClock(current_period_msg, response2);

		// If the message is not ok, show the message description and raise an exception
		if (!(response.isMessageStatusOk())){
			std::string statusDescr = response2.getParameter("Status_Description");
			throw MarketPlaceException(statusDescr, statusCd);
		}
		else{
			// get the parameter from the response and put it on the variable
			 int period = atoi((response2.getParameter("Period")).c_str());
			 initializePeriodSession(period);
		}

    } catch (Poco::NotFoundException &e) {
    	throw MarketPlaceException("name not found");
	}

	app.logger().debug("Market place System initialized");

}

void MarketPlaceSys::addAsClockListener(Poco::UInt16 port, std::string type)
{

	int statusCd = 0;
	Message port_msg;
	Message messageResponse;
	Method method = send_port;
	port_msg.setMethod(method);
	std::string portStr = Poco::NumberFormatter::format(port);
	port_msg.setParameter("Port", portStr);
	port_msg.setParameter("Type", type);
	sendMessageToClock(port_msg, messageResponse);

    // If the message is not ok, show the message description and rise an exception
    if (!(messageResponse.isMessageStatusOk())){
		std::string statusDescr = messageResponse.getParameter("Status_Description");
		throw MarketPlaceException(statusDescr, statusCd);
	}
}

const char* MarketPlaceSys::name() const
{
    if (p_cName.size() > 0)
		p_cName.c_str();
    else
		return "Subsystem Market Place ";
}

bool MarketPlaceSys::isAlreadyListener(std::string idListener)
{
	bool val_return;
	if (_listeners_by_id.count(idListener) > 0)
	    val_return = true;
	else
		val_return = false;
	return val_return;
}

bool MarketPlaceSys::isAlreadyListener(Poco::Net::SocketAddress socketAddress)
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

void MarketPlaceSys::getMessage(Poco::FIFOBuffer & fifoIn,
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
}

bool MarketPlaceSys::getMessage(Poco::Net::SocketAddress socketAddress, Message & message)
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
		throw MarketPlaceException("The agent is not a listener", 303);
	}
	return val_return;
}


void MarketPlaceSys::addStagedData(Poco::Net::SocketAddress socketAddress,
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
		throw MarketPlaceException("The agent is not a listener", 303);
	}
}

void MarketPlaceSys::insertListener(std::string idListener,
							 Poco::Net::SocketAddress socketAddress,
							 Message & messageResponse )
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information(Poco::format("insert listener id: %s", idListener));

	if (isAlreadyListener(idListener))
	{
		throw MarketPlaceException("The agent is already a listener", 302);
	}
	else
	{
		Listener *listener = new Listener(idListener, socketAddress);

		_listeners.insert( std::pair<Poco::Net::SocketAddress, Listener *>(socketAddress,listener));
		_listeners_by_id.insert( std::pair<std::string, Listener *>(idListener,listener));

		messageResponse.setResponseOk();
		app.logger().information(Poco::format("listener %s inserted", idListener));
	}
}

void MarketPlaceSys::startListening(Poco::Net::SocketAddress socketAddress,
							 Poco::UInt16 port, std::string type,
							 ProviderCapacityType capacity_type,
							 Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information(Poco::format("Start Listening by port:%u, type:%s", port, type ));


	Listeners::iterator it;
	it = _listeners.find(socketAddress);
	if (it != _listeners.end())
	{
		Poco::Net::SocketAddress sa = (*(it->second)).getSocketAddress();
		try
		{
			Poco::Net::SocketAddress sockadd(sa.host(), port);

			app.logger().debug("Socket address:" + sa.toString());

			(*(it->second)).Connect(sockadd);
			(*(it->second)).setListeningPort(port);
			(*(it->second)).setType(type);
			// If the listener is a provider, we added to the list of providers.
			if (type.compare("provider") == 0 )
			{
				std::string providerId = (it->second)->getId();
				app.logger().information(Poco::format("Connecting provider with Id: %s", providerId) );
				Provider * provider = new Provider(providerId, capacity_type);
				_providers.insert(std::pair<std::string, Provider *>( providerId, provider));
			}
			insertListenerBytype(type, (*(it->second)).getId());
			messageResponse.setParameter("Period", (int) _period);
			messageResponse.setResponseOk();

		} catch(const Poco::InvalidArgumentException &ex) {
			throw MarketPlaceException("Invalid host", 307);
		} catch(const Poco::Net::ServiceNotFoundException &ex){
			throw MarketPlaceException("Invalid port", 301);
		}
	}
    else
    {
		throw MarketPlaceException("The agent is not connected", 302);
	}

	app.logger().information(Poco::format("Ending Start Listening by port:%u, type:%s", port, type ));
}

void MarketPlaceSys::insertListenerBytype(std::string type, std::string listenerId)
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

void MarketPlaceSys::reinitiateDataContainers(MARKET_HISTORY_PERIOD subperiod)
{

	BidContainer::iterator it;
	for (it = _bids_to_broadcast.begin(); it != _bids_to_broadcast.end() ; ++it)
	{
		_bids_to_broadcast.erase(it);
	}

    if (_current_purchases != NULL){
		std::string periodStd = Poco::NumberFormatter::format((int) _period);
		std::string subPeriodStd = Poco::NumberFormatter::format((int) subperiod);
		periodStd = periodStd + subPeriodStd;

	 	_purchase_history.insert( std::pair< std::string, PurchaseInformation *> (periodStd, _current_purchases) );
		// Initialize again current purchases
		_current_purchases = NULL;

	}

	// Creates the new structure for the information.
	if (_current_purchases == NULL){
		_current_purchases = new PurchaseInformation();
	}

	ServiceContainer::iterator it_services;
	for (it_services = _services.begin(); it_services != _services.end(); ++it_services)
		_current_purchases->addService(it_services->first);

}


void MarketPlaceSys::initializePeriodSession(unsigned interval)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information(Poco::format("initialize interval session -----------------  : %d", (int) interval));

	if (_intervals_per_cycle > 0)
		_period = interval / _intervals_per_cycle;
	else
		_period = interval;


	if (sendInformation(interval)) {
		saveInformation();
		disseminateInformation();
		reinitiateDataContainers(START);
	}

	app.logger().information(Poco::format("Ending initialize interval session: %d", (int) interval));

}

void MarketPlaceSys::broadCastInformation(Message & message, std::string type)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("starting broadCastInformation");
	app.logger().debug(Poco::format("message: %s", message.to_string()));

	std::map<std::string, std::vector<std::string> >::iterator it_type;

	it_type = _listeners_by_type.find(type);
	if ( it_type != _listeners_by_type.end() )
	{
		// std::cout << "Type is listening" << type << std::endl;
		std::vector<std::string> list = it_type->second;
		std::vector<std::string>::iterator it_strings;
		it_strings = list.begin();

		while (it_strings != list.end())
		{
			std::map<std::string, Listener *>::iterator it = _listeners_by_id.find(*it_strings);
			if( it != _listeners_by_id.end() )
			{
				if ((*(it->second)).getStatus() == 1 ) // the listener is connected
				{
					try
					{
						(*(it->second)).write(message.to_string());
					}
					catch (FoundationException &e)
					{
						std::string msg = "Ther listener: ";
						msg.append(it->first);
						msg.append("is not listening anymore");
						app.logger().error(msg);

						// Remove the listener from the list.
					}
				}
			}
			++it_strings;
		}
	}

	app.logger().debug("ending broadCastInformation");
}

void MarketPlaceSys::saveBidInformation(void)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information("Starting saveBidInformation");

	if (_period > 0){

		BidContainer::iterator it;
		for (it = _bids_to_broadcast.begin(); it != _bids_to_broadcast.end() ; ++it)
		{

			app.logger().information("saving Bid information");

			Bid * bid = (it->second);
			// It saves the information at the start of the following period.
			bid->toDatabase(_pool, FoundationSys::getExecutionCount(), (int) _period - 1);
		}
	}
	else {
		app.logger().debug("Invalid period during saveBidInformation - period 0");
	}
	app.logger().information("Ending saveBidInformation");

}

void MarketPlaceSys::broadCastBidInformation(void)
{

	Message message;
	Method method = receive_bid_information;
	message.setMethod(method);
	message.setParameter("Period", (int) _period);

	Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
	Poco::XML::AutoPtr<Poco::XML::Element> pCompetitorBids = pDoc->createElement("New_Bids");

	BidContainer::iterator it;
	for (it = _bids_to_broadcast.begin(); it != _bids_to_broadcast.end() ; ++it)
	{
		Bid * bid = (it->second);
		bid->to_XML(pDoc, pCompetitorBids);
	}

	pDoc->appendChild(pCompetitorBids);
	Poco::XML::DOMWriter writer;
	writer.setNewLine("\n");
	writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
	std::stringstream  output;
	writer.writeNode(output, pDoc);
	message.setBody(output.str());

	// Send the message to providers and presenters.
	broadCastInformation(message, "provider");
	broadCastInformation(message, "presenter");

}


void MarketPlaceSys::sendProviderPurchaseInformation(void)
{

	// Iterate over the providers and send the information of purchases for the period.
    std::map<std::string, Provider *>::iterator it_provider;
    for(it_provider = _providers.begin(); it_provider != _providers.end(); it_provider++)
    {
		// std::cout << "In sendProviderPurchaseInformation: Provider" << it_provider->first << std::endl;
		std::map<std::string, Listener *>::iterator it_listeners;
		it_listeners = _listeners_by_id.find(it_provider->first);
		if (it_listeners != _listeners_by_id.end())
		{

			if (_current_bids != NULL)
			{
				Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
				Poco::XML::AutoPtr<Poco::XML::Element> pParentPurchaseUsage = pDoc->createElement("Receive_Purchases");

				// Get provider's bids and for each of them gets its neighbors
				std::map<std::string, std::vector<std::string> > bids;

				(*_current_bids).getProviderBids(it_provider->first, bids);

				(*_current_purchases).getPurchasesForProvider(pDoc, pParentPurchaseUsage, bids);

				pDoc->appendChild(pParentPurchaseUsage);
				Poco::XML::DOMWriter writer;
				writer.setNewLine("\n");
				writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
				std::stringstream  output;
				writer.writeNode(output, pDoc);
				Message message;
				Method method = receive_purchase_feedback;
				message.setMethod(method);
				message.setParameter("Period", (int) _period);
				message.setBody(output.str());
				// std::cout << "Message Receive Purchases to send:" << message.to_string() << std::endl;
				try
				{
					(*(it_listeners->second)).write(message.to_string());
				}
				catch (FoundationException &e)
				{
					Poco::Util::Application& app = Poco::Util::Application::instance();
					std::string msg = "Ther listener: ";
					msg.append(it_listeners->first);
					msg.append("is not listening anymore");
					app.logger().error(msg);
				}

				broadCastInformation(message, "presenter");
			}
		}
	}
}

void MarketPlaceSys::activatePresenter(void)
{
	Message message;
	Method method = activate_presenter;
	message.setMethod(method);
	message.setParameter("Period", (int) _period);
	broadCastInformation(message, "presenter");
}

void MarketPlaceSys::saveInformation()
{

	saveBidInformation();
	storeCurrentPurchaseInformation();

}

void MarketPlaceSys::disseminateInformation()
{

	broadCastBidInformation();
	sendProviderPurchaseInformation();

}

void MarketPlaceSys::finalizePeriodSession(unsigned  period,  Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information(Poco::format("finalize period session ------------current_period:%d given_eriod:%d", (int) _period, (int) period));

	saveInformation();
	disseminateInformation();
	reinitiateDataContainers(END);

	activatePresenter();
	messageResponse.setResponseOk();
}

void MarketPlaceSys::addBid(Bid * bidPtr, Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information("Starting add Bid");

	// First verify that the bid was not included
	std::map<std::string, Bid *>::iterator it;

	it = _bids.find((*bidPtr).getId());
	if (it == _bids.end())
	{
		// Verify if the service exist
		bool exist = (*_current_bids).existService((*bidPtr).getService());

		if (!exist){
			// When it does not exist then creates the service
			// std::cout << "adding service in the market place" << std::endl;
			(*_current_bids).addService((*bidPtr).getService());
		}

		// In any case inserts the bid into the service container, this part
		// also verifies whether or not the bid given belongs to the best bids.
		(*_current_bids).addBidToService(bidPtr);
		// std::cout << "Bid inserted in the market place" << std::endl;

		// Insert the bid in the container
		_bids.insert(std::pair<std::string, Bid *> ((*bidPtr).getId(), bidPtr));

		// Insert in the brodcast container
		_bids_to_broadcast.insert(std::pair<std::string, Bid *> ((*bidPtr).getId(), bidPtr));

		app.logger().information("New Bid added");

		// set the response as Ok
		messageResponse.setResponseOk();
	}
	else
	{
		app.logger().information("Bid is already included in the container");
		throw MarketPlaceException("Bid is already included in the container", 310);
	}
}

void MarketPlaceSys::deleteBid(Bid * bidPtr, Message & messageResponse)
{
	// First verify that the bid was not included
	std::map<std::string, Bid *>::iterator it;

	it = _bids.find((*bidPtr).getId());
	if (it != _bids.end())
	{

		// In any case deletes the bid into the service container, so the bid
		// does not continue in the pareto front.
		(*_current_bids).deleteBidToService(bidPtr);
		// std::cout << "Bid inserted in the market place" << std::endl;

		// Deletes all the neighbors of the bid
		Bid * bidSearched = it->second;
		std::vector<std::string> neigbors;
		(*bidSearched).getNeighbors(neigbors);
		std::vector<std::string>::iterator it_neighbors;
		for (it_neighbors= neigbors.begin(); it_neighbors != neigbors.end() ; ++it_neighbors)
		{
			(*bidSearched).deleteNeighbor(*it_neighbors);
			std::map<std::string, Bid *>::iterator it_neig;
			it_neig = _bids.find(*it_neighbors);
			if (it_neig != _bids.end())
			{
				(*(it_neig->second)).deleteNeighbor((*bidSearched).getId());
			}
		}

		// Insert in the brodcast container
		_bids_to_broadcast.insert(std::pair<std::string, Bid *> ((*bidPtr).getId(), bidPtr));

		// set the response as Ok
		messageResponse.setResponseOk();
	}
	else
	{
		throw MarketPlaceException("Bid is NOT included in the container", 310);
	}
}

void MarketPlaceSys::addPurchaseBulkCapacity(Provider *provider, Service *service, Bid * bid, Purchase * purchasePtr, bool purchaseFound, Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering addPurchaseBulkCapacity");

	if (provider->isBulkAvailable(_period, service, purchasePtr, bid))
	{
		// In any case inserts the purchase into the service container.
		(*_current_purchases).addPurchaseToService(purchasePtr, purchaseFound);

		// std::cout << "Purchase inserted in the market place" << std::endl;
		_purchases.insert(std::pair<std::string, Purchase *>((*purchasePtr).getId(), purchasePtr) );

		// Deducts from the availability
		provider->deductAvailability(_period, service, purchasePtr, bid);

		// Establishes the quantity purchased as a feedback to the agent.
		messageResponse.setParameter("Quantity_Purchased", purchasePtr->getQuantityStr());
	}
	else
	{
		// Set the quantity to purchase equal to zero as it is not going to purchase anything.
		purchasePtr->setQuantityBacklog(purchasePtr->getQuantity());
		purchasePtr->setQuantity(0);

		// In any case inserts the purchase into the service container.
		(*_current_purchases).addPurchaseToService(purchasePtr, purchaseFound);

		messageResponse.setParameter("Quantity_Purchased", "0");
	}

	app.logger().debug("Ending addPurchaseBulkCapacity");

}

void MarketPlaceSys::addPurchaseByBidCapacity(Provider *provider, Service *service, Bid * bid, Purchase * purchasePtr, bool purchaseFound,  Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering addPurchaseByBidCapacity");

	if (bid->getCapacity() >= purchasePtr->getQuantity())
	{
		// In any case inserts the purchase into the service container.
		(*_current_purchases).addPurchaseToService(purchasePtr, purchaseFound);

		_purchases.insert(std::pair<std::string, Purchase *>((*purchasePtr).getId(), purchasePtr) );

		bid->setCapacity(bid->getCapacity() - purchasePtr->getQuantity());

		messageResponse.setParameter("Quantity_Purchased", purchasePtr->getQuantityStr());

	} else {

		// It establish the backlog for that bid.
		if (purchaseFound == false) {
			purchasePtr->setQuantityBacklog(purchasePtr->getQuantity() - bid->getCapacity());
		}

		// It buys the quantity available.
		purchasePtr->setQuantity(bid->getCapacity());

		// In any case inserts the purchase into the service container.
		(*_current_purchases).addPurchaseToService(purchasePtr, purchaseFound);

		// std::cout << "Purchase inserted in the market place" << std::endl;
		_purchases.insert(std::pair<std::string, Purchase *>((*purchasePtr).getId(), purchasePtr) );

		bid->setCapacity(bid->getCapacity() - purchasePtr->getQuantity());
		messageResponse.setParameter("Quantity_Purchased", purchasePtr->getQuantityStr());

	}
}

void MarketPlaceSys::addPurchase(Purchase * purchasePtr, Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information(Poco::format("Add Purchase Id:%s :Period:%d :BidId:%s qtyRequested:%f", purchasePtr->getId(), (int) _period, purchasePtr->getBid(), purchasePtr->getQuantity() ));


	try {

		// Search if the purchased has already sent for another bid.
		bool purchaseFound = false;

		std::map<std::string, int >::iterator it;
		it = request_purchases.find(purchasePtr->getId());
		if ( it !=  request_purchases.end()){
			it->second = it->second + 1;
			purchaseFound = true;
		} else {
			request_purchases.insert(std::pair<std::string,int>(purchasePtr->getId(),1));
		}

		Bid * bid = getBid(purchasePtr->getBid());
		bool isActive = (*_current_bids).isBidActive(bid->getService(), bid->getProvider(), bid->getId());

		if (isActive)
		{
			Service * service = getService(purchasePtr->getService());
			Provider * provider = getProvider(bid->getProvider());

			if (provider->getCapacityType() == BULK_CAPACITY)
				addPurchaseBulkCapacity(provider, service, bid, purchasePtr, purchaseFound, messageResponse);
			else
				addPurchaseByBidCapacity(provider, service, bid, purchasePtr, purchaseFound, messageResponse);

			// set the response as Ok
			messageResponse.setResponseOk();
		}
		else
		{
			app.logger().error("could not purchase - bid is not active");
			// The bid is not active, so you cannot sell anymore.
			messageResponse.setParameter("Quantity_Purchased", "0");
			// set the response as Ok
			messageResponse.setResponseOk();
		}
	} catch (MarketPlaceException &e) {
		app.logger().error(Poco::format("could not purchase -raise exception error:%s", e.message()) );
		throw e;

	}
}

void MarketPlaceSys::setProviderAvailability(std::string providerId,
											 std::string resourceId,
											 double quantity,
											 Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering MarketPlaceSys - setProviderAvailability");

	Provider * provider = getProvider(providerId);
	Resource * resource = getResource(resourceId);
	provider->setInitialAvailability(resource, quantity);
	messageResponse.setResponseOk();
	app.logger().debug("Ending -------- MarketPlaceSys - setProviderAvailability");
}

void MarketPlaceSys::getBestBids(std::string providerId, std::string serviceId,
								Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering getBestBids");

	int fronts = getParetoFrontsToExchange();

	messageResponse.setResponseOk();
	// Constructs a message with header and body,
	// the header is composed of the service and the number of pareto fronts
	// delivered. The body corresponds to the bids delivered.
    std::string xml = (*_current_bids).getBestBids(serviceId, fronts);
	messageResponse.setParameter("Service", serviceId);
	messageResponse.setParameter("Fronts", fronts);
    messageResponse.setBody(xml);

	app.logger().debug("Ending getBestBids");

}

void MarketPlaceSys::getBestBids(std::string providerId, std::string serviceId,
								 int fronts, Message & messageResponse)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug(Poco::format("Entering getBestBids - fronts: %d", fronts));

	messageResponse.setResponseOk();

	// Constructs a message with header and body,
	// the header is composed of the service and the number of pareto fronts
	// delivered. The body corresponds to the bids delivered.
    std::string xml = (*_current_bids).getBestBids(serviceId, fronts);
	messageResponse.setParameter("Service", serviceId);
	messageResponse.setParameter("Fronts", fronts);
    messageResponse.setBody(xml);

    app.logger().debug("Ending getBestBids");

}

void MarketPlaceSys::sendBid(std::string bidId, Message & messageResponse)
{

	messageResponse.setResponseOk();

    Bid * bid = getBid(bidId);
	bid->toMessage(messageResponse);
}

void MarketPlaceSys::sendProviderChannel(std::string providerId, Message & messageResponse)
{
	std::map<std::string, Listener *>::iterator it = _listeners_by_id.find(providerId);
	if( it != _listeners_by_id.end() )
	{
		if ((*(it->second)).getStatus() == 1 ) // the listener is connected
		{
			messageResponse.setResponseOk();
			Poco::Net::SocketAddress socketAddress = (*(it->second)).getSocketAddress();
			std::string address = (socketAddress.host()).toString();
			std::string portString = Poco::NumberFormatter::format((*(it->second)).getListeningPort());
			messageResponse.setParameter("Address", address);
			messageResponse.setParameter("Port", portString);
		}
		else
		{
			throw MarketPlaceException("The provider is not listening", 302);
		}
	}
	else
	{
		throw MarketPlaceException("The provider is not listening", 302);
	}
}

Provider * MarketPlaceSys::getProvider(std::string providerId)
{
	std::map<std::string, Provider *>::iterator it;
	it = _providers.find(providerId);
	if(it != _providers.end()) {
		return it->second;
	}
	else
	{
		Poco::Util::Application& app = Poco::Util::Application::instance();
		app.logger().debug("Provider is not included in the container %s", providerId);
		throw MarketPlaceException("Provider is not included in the container");
	}
}

Bid * MarketPlaceSys::getBid(std::string bidId)
{
	BidContainer::iterator it;
	it = _bids.find(bidId);
	if ( it != _bids.end())
	{
		return it->second;
	}
	else
	{
		Poco::Util::Application& app = Poco::Util::Application::instance();
		app.logger().debug("Bid is not included in the container");
		throw MarketPlaceException("Bid is not included in the container", 310);
	}
}

void MarketPlaceSys::storeCurrentPurchaseInformation()
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Starting storeCurrentPurchaseInformation");

	if (_period > 0){
		if (_current_purchases != NULL)
		{
			_current_purchases->toDatabase(_pool, FoundationSys::getExecutionCount() , (int) _period -1 );
		}
		else{
			app.logger().debug(Poco::format("no current purchases to store - Period:%d", (int) _period) );
		}
	}
	else {
		app.logger().debug(Poco::format("invalid period - Period:%d", (int) _period) );
	}
	app.logger().debug("Ending storeCurrentPurchaseInformation");
}

void MarketPlaceSys::deleteListener( Poco::Net::SocketAddress socketAddress,
						      Message & messageResponse )
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	Poco::LogStream lstr(app.logger());
	lstr << "Start deleteListener" << std::endl;
	Listener *list = NULL;


	// Find the listener
	Listeners::iterator it;
	bool found = false;

	lstr << "Nbr Listeners:" << _listeners.size() << std::endl;

    std::string sockAddrPar = socketAddress.toString();

    lstr << "Sock Address Par:" << sockAddrPar << std::endl;
	it = _listeners.begin();
	while( (it!=_listeners.end()) && (found ==false) )
	{
		Poco::Net::SocketAddress sa = (*(it->second)).getSocketAddress();
		std::string sockAddrStr = sa.toString();

		lstr << "Ssock Address:" << sockAddrStr << std::endl;

		if ( sockAddrPar.compare(sockAddrStr) == 0 ){
		   found = true;
		   break;
		}
		++it;
	}

    lstr << "address found:" << found << std::endl;

    if (found == true){
		list = it->second;
		_listeners.erase(it);
		std::string idListener = list->getId();
		std::string type = list->getTypeStr();

    	// Delete the listener from the list of listeners by type.
		std::map<std::string, std::vector<std::string> >::iterator it2;
		it2 = _listeners_by_type.find(type);
		if (it2 != _listeners_by_type.end()){
			std::vector<std::string>::iterator it3;
			for (it3 = (it2->second).begin(); it3 != (it2->second).end(); ++it3 ){
				if (it3->compare(idListener) == 0){
					(it2->second).erase(it3);
					break;
				}
			}
		}

		// These two containers act as indexes for the listeners container.
		std::map<std::string, Listener *>::iterator it4;
		it4 = _listeners_by_id.find(idListener);
		if (it4 != _listeners_by_id.end()){
			_listeners_by_id.erase(it4);
		}

		// Delete from  provider.
		if ( list->getType() == PROVIDER){
			std::map<std::string, Provider *>::iterator it5;
			for (it5 = _providers.begin(); it5 != _providers.end(); ++it5){
				delete (it5->second);
				_providers.erase(it5);
				break;
			}
		}

		// Disconnect the socket.
		list->Disconnect();

		// Finally dispose the listener object
		delete list;
    }

	app.logger().debug("Ending removeListener");

}

bool MarketPlaceSys::sendInformation(unsigned interval)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug(Poco::format("Starting sendInformation, %d", (int) interval ));

	bool val_return = false;
	if ( (interval % _intervals_per_cycle) == _send_interval)
		val_return = true;

	app.logger().debug(Poco::format("Ending sendInformation %b", val_return));
	return val_return;

}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
