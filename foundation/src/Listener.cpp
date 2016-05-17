
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/IPAddress.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/Util/ServerApplication.h>
#include <string>
#include <iostream>
#include "FoundationException.h"
#include "Listener.h"

namespace ChoiceNet
{
namespace Eco
{
const char * ListenerTypeDesc[] = { "undefined",
			 						 "market_place",
									 "demand_server",
									 "provider",
									 "consumer",
									 "presenter" };



Listener::Listener(std::string idParam, Poco::Net::SocketAddress& ipAddressParam):
_id(idParam), _type(UNDEFINED_TYPE), _ipAddress(ipAddressParam), _status(DISCONNECTED), _socket(new Poco::Net::StreamSocket), _listeneningPort(0)
{

}

Listener::~Listener()
{
}

void Listener::Connect(Poco::Net::SocketAddress& addressParam)
{
	// std::cout << "socket ini" << std::endl;
	(*_socket).connect(addressParam);
	_status = CONNECTED;
	// std::cout << "socket end" << std::endl;
	
}

void Listener::setListeningPort(Poco::UInt16 port)
{
	_listeneningPort = port;
}

Poco::UInt16 Listener::getListeningPort(void)
{
	return _listeneningPort;
}

void Listener::setType(std::string type)
{
	if (type.compare("consumer") == 0){
		_type = CONSUMER;
	}
	else if (type.compare("provider") == 0){
		_type = PROVIDER;
	}
	else if (type.compare("market_place") == 0){
		_type = MARKET_PLACE;
	}
	else if (type.compare("demand_server") == 0){
		_type = DEMAND_SERVER;
	}
	else if (type.compare("presenter") == 0){
		_type = PRESENTER;
	}
	else{
		_type = UNDEFINED_TYPE;
	}
}

std::string
Listener::getTypeStr()
{
	return std::string(ListenerTypeDesc[_type]);
}

void Listener::Disconnect()
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	try
	{
		(*_socket).shutdown();
		(*_socket).close();
	}
	catch(...)
	{
		app.logger().error("Leaving Listener - Disconnect");
		app.logger().error("Agent already disconnected");
	}
	_status = DISCONNECTED;
}

void Listener::write (std::string text)
{
	try
	{
		int sendBytes = (*_socket).sendBytes(text.c_str(), text.size());
	}
	catch(Poco::IOException &e)
	{
		throw FoundationException("The message could not be sended to the listener", 320);
	}
	
}

Poco::Net::SocketAddress Listener::getSocketAddress()
{
	return _ipAddress;
}

ListenerStatus Listener::getStatus(void)
{
	return _status;
}

ListenerType Listener::getType(void)
{
	return _type;
}

std::string Listener::getId()
{
	return _id;
}

void Listener::addStreamStagedForProcessing(Poco::FIFOBuffer & fifoIn, int len)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering Listener - addStreamStagedForProcessing");
	std::size_t i = 0;
	for (; i < fifoIn.used() ; ++i){
		_waitingProcessing += (*(fifoIn.begin() + i));
	}
	fifoIn.drain(len);
	app.logger().debug("Leaving Listener - addStreamStagedForProcessing");
	app.logger().debug(_waitingProcessing);
}

bool Listener::getMessage(Message & message)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering get Message");
	
	bool val_return = false;
	unsigned size;
	
	if (_waitingProcessing.size() > 0) {
		std::size_t found = _waitingProcessing.find("Method");
		if (found == 0)
		{
			std::size_t found2 = _waitingProcessing.find("Method", found + 1);
			if (found2 != std::string::npos)
			{
				std::string messageData = _waitingProcessing.substr(found, found2);
				message.setData(messageData);
				// Even that the message could have errors is complete
				_waitingProcessing.erase(0,found2);
				app.logger().debug("get Message from:%z to:%z", found, found2);
				val_return = true;
			}
			else
			{
				std::string messageData = _waitingProcessing.substr(found, std::string::npos);
				message.setData(messageData);
				if (message.isComplete(_waitingProcessing.size()) )
				{
					_waitingProcessing.erase(0, std::string::npos);
					val_return = true;
				}
				else{
					val_return = false;
				}
			}
		}
		else
		{
			// The message is not well formed, so we create a message with method
			// not specified

			app.logger().debug("undefined:%z", found);

			Method method = undefined;
			message.setMethod(method);
			message.setParameter("Message_Size", (int) found);
			_waitingProcessing.erase(0,found-1);
			val_return = true;
		}
	} else {
		app.logger().debug("buffer is empty nothing to do");
		val_return = false;
	}
	if (val_return)
		app.logger().debug("Leaving - get Message: true");
	else
		app.logger().debug("Leaving - get Message: false");

	return val_return;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
