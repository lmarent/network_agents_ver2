#ifndef Listener_INCLUDED
#define Listener_INCLUDED

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/AutoPtr.h>
#include <Poco/FIFOBuffer.h>
#include <string>

#include "Message.h"

namespace ChoiceNet
{
namespace Eco
{
	
enum ListenerType
{
	UNDEFINED_TYPE = 0,
	MARKET_PLACE = 1,
	DEMAND_SERVER = 2,
	PROVIDER = 3,
	CONSUMER = 4,
	PRESENTER = 5
};

enum ListenerStatus
{
	CONNECTED = 1,
	DISCONNECTED = 2,
	ACTIVE = 3
};

class Listener
{

public:	
	Listener(std::string idParam, Poco::Net::SocketAddress& ipAddressParam);
	~Listener();

    void Connect(Poco::Net::SocketAddress& addressParam);
    void setListeningPort(Poco::UInt16 port);
    Poco::UInt16 getListeningPort(void);
    void setType(std::string type);
    void Disconnect();
	void write (std::string text);
	Poco::Net::SocketAddress getSocketAddress();
	ListenerStatus getStatus();
	ListenerType getType();
	std::string getId();
	void addStreamStagedForProcessing(Poco::FIFOBuffer & fifoIn, int len);
	bool getMessage(Message & messsage);
	

private:
	std::string _id;
	Poco::Net::SocketAddress _ipAddress;
	ListenerStatus _status;
	ListenerType _type;
	Poco::Net::StreamSocket * _socket;
	std::string _waitingProcessing;
	Poco::UInt16 _listeneningPort;

};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif
