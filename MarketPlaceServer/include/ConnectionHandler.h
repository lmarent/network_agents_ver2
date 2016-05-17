//
// ConnectionHandler.h
//
//
// Definition of the ConnectionHandler class.
//
// Copyright (c) 2014, ChoiceNet Project.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef Connection_Handler_INCLUDED
#define Connection_Handler_INCLUDED


#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/NObserver.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Delegate.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <iostream>


#include "Bid.h"
#include "Purchase.h"
#include "Message.h"

namespace ChoiceNet
{

namespace Eco
{

class ConnectionHandler
/// I/O handler class. This class (un)registers handlers for I/O based on
/// data availability. To ensure non-blocking behavior and alleviate spurious
/// socket writability callback triggering when no data to be sent is available,
/// FIFO buffers are used. I/O FIFOBuffer sends notifications on transitions
/// from [1] non-readable (i.e. empty) to readable, [2] writable to non-writable
/// (i.e. full) and [3] non-writable (i.e. full) to writable.
/// Based on these notifications, the handler member functions react by
/// enabling/disabling respective reactor framework notifications.
{
public:
	ConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);

	~ConnectionHandler();

	void onFIFOOutReadable(bool& b);

	void onFIFOInWritable(bool& b);

	void onSocketReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);

	void onSocketWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf);

	void onSocketTimeout(const Poco::AutoPtr<Poco::Net::TimeoutNotification >& pNf);

	void onSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf);

	void onSocketError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf);
	
	void onSocketIdle(const Poco::AutoPtr<Poco::Net::IdleNotification>& pNf);

	void doProcessing(Poco::Net::SocketAddress socketAddress, 
					  ChoiceNet::Eco::Message & message);
	
	void Connect(Poco::Net::SocketAddress socketAddress,
						 ChoiceNet::Eco::Message & messageRequest,
						 ChoiceNet::Eco::Message & messageResponse);
	
	void StartListening(Poco::Net::SocketAddress socketAddress,
						ChoiceNet::Eco::Message & messageRequest,
						ChoiceNet::Eco::Message & messageResponse);
	
	void initializePeriodSession(Poco::Net::SocketAddress socketAddress, 
								 ChoiceNet::Eco::Message & messageRequest,
								 ChoiceNet::Eco::Message & messageResponse);
	
	void finalizePeriodSession(Poco::Net::SocketAddress socketAddress, 
							   ChoiceNet::Eco::Message & messageRequest,
							   ChoiceNet::Eco::Message & messageResponse);
	
	void receiveBid(Poco::Net::SocketAddress socketAddress, 
					ChoiceNet::Eco::Message & messageRequest,
					ChoiceNet::Eco::Message & messageResponse);
	
	void addPurchase( Poco::Net::SocketAddress socketAddress,
					  ChoiceNet::Eco::Message & messageRequest,
					  ChoiceNet::Eco::Message & messageResponse);
	
	void setProviderAvailability(Poco::Net::SocketAddress socketAddress,
								 ChoiceNet::Eco::Message & messageRequest,
								 ChoiceNet::Eco::Message & messageResponse);
	
	void getBestBids( Poco::Net::SocketAddress socketAddress,
					  ChoiceNet::Eco::Message & messageRequest,
					  ChoiceNet::Eco::Message & messageResponse);
					   
	void getBid( Poco::Net::SocketAddress socketAddress,
					  ChoiceNet::Eco::Message & messageRequest,
					  ChoiceNet::Eco::Message & messageResponse);
	
	void getProviderChannel(Poco::Net::SocketAddress socketAddress,
							ChoiceNet::Eco::Message & messageRequest,
							ChoiceNet::Eco::Message & messageResponse);
	
	void terminateProcess(void);

	void missingParametersProcedure(ChoiceNet::Eco::Message & messageResponse);
	
	void errorProcedure(ChoiceNet::Eco::Message & messageResponse);
	
	
private:
	enum
	{
		BUFFER_SIZE = 16384
	};

	Poco::Net::StreamSocket _socket;
	Poco::Net::SocketReactor& _reactor;
	Poco::FIFOBuffer _fifoIn;
	Poco::FIFOBuffer _fifoOut;
	std::string _idListener;
};

}   /// End Eco namespace

}  /// End ChoiceNet namespace

#endif   // Connection_Handler
