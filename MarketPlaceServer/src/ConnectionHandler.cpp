//
// ConnectionHandler.cpp
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
#include <Poco/StringTokenizer.h>
#include <Poco/Types.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>

#include "ConnectionHandler.h"
#include "MarketPlaceServer.h"
#include "MarketPlaceSys.h"
#include "Message.h"
#include "MarketPlaceException.h"
#include "FoundationException.h"


namespace ChoiceNet
{
namespace Eco
{

ConnectionHandler::ConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor):
_socket(socket),
_reactor(reactor),
_fifoIn(BUFFER_SIZE, true),
_fifoOut(BUFFER_SIZE, true)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	
	app.logger().debug("Connection from " + socket.peerAddress().toString());

	_reactor.addEventHandler(_socket, 
		Poco::NObserver<ConnectionHandler,Poco::Net::ReadableNotification>(*this, &ConnectionHandler::onSocketReadable)
		);
				
	_reactor.addEventHandler(_socket, 
		Poco::NObserver<ConnectionHandler, Poco::Net::ShutdownNotification>(*this, &ConnectionHandler::onSocketShutdown)
		);	

	_reactor.addEventHandler(_socket, 
		Poco::NObserver<ConnectionHandler, Poco::Net::ErrorNotification>(*this, &ConnectionHandler::onSocketError)
		);	

    // std::cout << "Adding the hadler for idle notification" << std::endl;

	_reactor.addEventHandler(_socket, 
		Poco::NObserver<ConnectionHandler, Poco::Net::IdleNotification>(*this, &ConnectionHandler::onSocketIdle)
		);			

	_reactor.addEventHandler(_socket,
		Poco::NObserver<ConnectionHandler, Poco::Net::WritableNotification>(*this, &ConnectionHandler::onSocketWritable)
		);

	_reactor.addEventHandler(_socket,
		Poco::NObserver<ConnectionHandler, Poco::Net::TimeoutNotification>(*this, &ConnectionHandler::onSocketTimeout)
		);


	_fifoOut.readable += Poco::delegate(this, &ConnectionHandler::onFIFOOutReadable);
	_fifoIn.writable += Poco::delegate(this, &ConnectionHandler::onFIFOInWritable);
				
}

ConnectionHandler::~ConnectionHandler()
{
	Poco::Util::Application& app = Poco::Util::Application::instance();

	try{
		_reactor.removeEventHandler(_socket, Poco::NObserver<ConnectionHandler,
					Poco::Net::ReadableNotification>(*this, &ConnectionHandler::onSocketReadable));
		_reactor.removeEventHandler(_socket, Poco::NObserver<ConnectionHandler,
					Poco::Net::WritableNotification>(*this, &ConnectionHandler::onSocketWritable));
		_reactor.removeEventHandler(_socket, Poco::NObserver<ConnectionHandler,
					Poco::Net::ShutdownNotification>(*this, &ConnectionHandler::onSocketShutdown));

		_fifoOut.readable -= Poco::delegate(this, &ConnectionHandler::onFIFOOutReadable);
		_fifoIn.writable -= Poco::delegate(this, &ConnectionHandler::onFIFOInWritable);

	} catch (Poco::SystemException &e){
		throw MarketPlaceException( e.message());
	}
}

void ConnectionHandler::onFIFOOutReadable(bool& b)
{
	if (b)
		_reactor.addEventHandler(_socket, Poco::NObserver<ConnectionHandler, 
				Poco::Net::WritableNotification>(*this, &ConnectionHandler::onSocketWritable));
	else
		_reactor.removeEventHandler(_socket, Poco::NObserver<ConnectionHandler, 
				Poco::Net::WritableNotification>(*this, &ConnectionHandler::onSocketWritable));
}

void ConnectionHandler::onFIFOInWritable(bool& b)
{
	if (b)
		_reactor.addEventHandler(_socket, Poco::NObserver<ConnectionHandler, 
				Poco::Net::ReadableNotification>(*this, &ConnectionHandler::onSocketReadable));
	else
		_reactor.removeEventHandler(_socket, Poco::NObserver<ConnectionHandler, 
				Poco::Net::ReadableNotification>(*this, &ConnectionHandler::onSocketReadable));
}

void ConnectionHandler::onSocketReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf)
{
	if (_socket.available())
	{
		
		int len = _socket.receiveBytes(_fifoIn.next(), _fifoIn.available() );
		_fifoIn.advance(len);
		
		Poco::Util::Application& app = Poco::Util::Application::instance();
		MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
		MarketPlaceSys *sys = server.getMarketPlaceSubsystem();

		// Receive the message(s)
		Message message;
		if ((*sys).isAlreadyListener(_socket.peerAddress()))
		{
			(*sys).addStagedData(_socket.peerAddress(), _fifoIn, len);		
			while ((*sys).getMessage(_socket.peerAddress(), message))
			{
				doProcessing(_socket.peerAddress(), message);
			}
		}
		else
		{
			std::cout << "the agent is not listening" <<  (_socket.peerAddress()).toString() << std::endl;
			std::string received;
			(*sys).getMessage(_fifoIn, len, message);
			// std::cout << message.to_string() << std::endl;
			doProcessing(_socket.peerAddress(), message);
		}
	}	
	else
	{
		// std::cout << "Socket is unavailable" << std::endl;
	}
}

void ConnectionHandler::onSocketWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("On socket writable");
	
	int len = _socket.sendBytes((_fifoOut.buffer()).begin(), _fifoOut.used());
	_fifoOut.drain(len);
}

void ConnectionHandler::onSocketTimeout(const Poco::AutoPtr<Poco::Net::TimeoutNotification >& pNf)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("On socket timeout");

}


void ConnectionHandler::onSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Socket shutdown");
	delete this;
}

void ConnectionHandler::onSocketError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Socket error");
	std::cout << "Error occurs" << std::endl;
	delete this;
}

void ConnectionHandler::onSocketIdle(const Poco::AutoPtr<Poco::Net::IdleNotification>& pNf)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("On idle notification");
	std::cout << "On idle notification" << std::endl;
	sleep(0.00001);
}

void ConnectionHandler::doProcessing(Poco::Net::SocketAddress socketAddress, 
									 Message & message)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("do processing");

	Method meth = message.getMethod();
		
	// This object will be the response for the calling application.
	Message messageResponse;
	messageResponse.setMethod(meth);
	try 
	{
		switch (meth){
		   case connect:
			 {
				
				// Verify the required parameters: provider
				app.logger().debug("In Provider connect");
				providerConnect( socketAddress, message, messageResponse );
				break;
			 }	
		   case send_port:
			 {
				// Verify the required parameters: port
				app.logger().debug("In providerStartListening");
				providerStartListening( socketAddress, message, messageResponse );
				break;
			 }
		   case start_period:
			 {
				// Verify the required parameters: Period
				app.logger().debug("In initializePeriodSession");
				initializePeriodSession( socketAddress, message, messageResponse );
				break;
			 }
		   case end_period:
			 {
				// Verify the required parameters: Period
				app.logger().debug("In finalizePeriodSession");
				finalizePeriodSession( socketAddress, message, messageResponse );
				break;
			 }
		   case receive_bid:
			 {
				// Handled the request for a bid into the container
				app.logger().debug("In receiveBid");
				receiveBid( socketAddress, message, messageResponse );
				break;
			 }
		   case get_best_bids:
			 {
				app.logger().debug("In getBestBids");
				getBestBids( socketAddress, message, messageResponse );
				break;
			 }
		   case receive_purchase:
			 {
  				app.logger().debug("In addPurchase");
				addPurchase( socketAddress, message, messageResponse );
				break;
		     }
		   case send_availability:
			 {			
  				app.logger().debug("In send_availability");
				setProviderAvailability(socketAddress, message, messageResponse);
				break;
			 }
		   case get_bid:
		 	 {
			    // get the information for the bid requested.
  				app.logger().information("In getBid");
			    getBid(socketAddress, message, messageResponse);
			    break;
			 }
		   
		   case get_provider_channel:
		 	 {
			    // Ends the execution of the process.
  				app.logger().debug("In get_provider_channel");
			    getProviderChannel(socketAddress, message, messageResponse);
			    break;
			 }
		   case disconnect:
		 	 {
			    // Ends the execution of the process.
  				std::cout << "In terminateProcess" << std::endl;
  				app.logger().information("In terminateProcess");
			    terminateProcess();
			    break;
			 }
		   default:
			 {
				// Generates an error	
  				app.logger().debug("In errorProcedure");
				errorProcedure(messageResponse);
				break;						
			 }
		}
	}
	catch(FoundationException &e){
		std::string codeStr;
		Poco::NumberFormatter::append(codeStr,e.code());
		messageResponse.setParameter("Status_Code", codeStr);
		messageResponse.setParameter("Status_Description", e.message());		
	}
	catch(MarketPlaceException &e){
		std::string codeStr;
		Poco::NumberFormatter::append(codeStr,e.code());
		messageResponse.setParameter("Status_Code", codeStr);
		messageResponse.setParameter("Status_Description", e.message());
	}
	
	// send the response for the calling agent
	std::string responseStr = messageResponse.to_string();
	size_t charactersWritten = 0;
	if ((responseStr.length() + 1) > (_fifoOut.size() + _fifoOut.used()))
	{
	   _fifoOut.resize(_fifoOut.used() + responseStr.length() + 1, true);
	}
    charactersWritten = _fifoOut.write(responseStr.c_str(), responseStr.length());		

}


void ConnectionHandler::providerConnect(Poco::Net::SocketAddress socketAddress, 
									    Message & messageRequest,
									    Message & messageResponse)
{
	std::string providerId = messageRequest.getParameter("Provider");
	Poco::Util::Application& app = Poco::Util::Application::instance();
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();
	(*sys).insertListener(providerId, socketAddress, messageResponse);
	// std::cout << "Added listener" << std::endl;
}

void ConnectionHandler::providerStartListening(Poco::Net::SocketAddress socketAddress, 
											   Message & messageRequest,
											   Message & messageResponse)
{
	unsigned  Uport; 
	std::string port = messageRequest.getParameter("Port");
	bool v_result = Poco::NumberParser::tryParseUnsigned(port, Uport);
	std::string type = messageRequest.getParameter("Type");
	if ( Uport <= 0xFFFF){
		Poco::Util::Application& app = Poco::Util::Application::instance();
		MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
		MarketPlaceSys *sys = server.getMarketPlaceSubsystem();
		Poco::UInt16 u16Port = (Poco::UInt16) Uport;
		(*sys).startListening(socketAddress, u16Port, type, messageResponse);
	}
	else
	{
		throw MarketPlaceException("Invalid Port", 301);
	}	
	// std::cout << "Connected to listener" << std::endl;
}

void ConnectionHandler::initializePeriodSession(Poco::Net::SocketAddress socketAddress, 
												Message & messageRequest,
												Message & messageResponse)
{    
    // Create a new structure to hold listeners bids and put it as current
    // the structure is as follows:
    // a service has a best bid
    //           has many providers 
    //           		     each provider could have many bids
	Poco::Util::Application& app = Poco::Util::Application::instance();	
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();
	
	unsigned  period; 
	std::string periodStr = messageRequest.getParameter("Period");
	bool v_result = Poco::NumberParser::tryParseUnsigned(periodStr, period);
	if (v_result)
	{
		(*sys).initializePeriodSession((unsigned) period);
		messageResponse.setResponseOk();
		app.logger().information("Starting a new offering session" + periodStr);
	}	
	else
	{
		throw MarketPlaceException("Invalid period", 305);
	}    
}

void ConnectionHandler::finalizePeriodSession(Poco::Net::SocketAddress socketAddress, 
											  Message & messageRequest,
											  Message & messageResponse)
{ 

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Finalizing the session");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();
	(*sys).finalizePeriodSession(messageResponse);
	app.logger().debug("Session Finalized");
}


void ConnectionHandler::receiveBid(Poco::Net::SocketAddress socketAddress, 
								   Message & messageRequest, 
								   Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Adding bid in the market place");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();
	std::string serviceId = messageRequest.getParameter("Service");
	try{
		Service * service = (*sys).getService(serviceId);
		Bid * bidPtr = new Bid(service, messageRequest);
		if (bidPtr->isActive())
		{
			(*sys).addBid(bidPtr, messageResponse);
			app.logger().debug("Bid added in the market place");
		}
		else
		{
			(*sys).deleteBid(bidPtr, messageResponse);
			app.logger().debug("Bid deleted in the market place");
		}
	} catch (FoundationException &e){
		app.logger().error(e.message());
	}
}

void ConnectionHandler::addPurchase(Poco::Net::SocketAddress socketAddress,
									Message & messageRequest, 
									Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Adding purchase in the market place");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();
	Service * service = (*sys).getService(messageRequest.getParameter("Service"));
	Purchase * purchasePtr = new Purchase(service, messageRequest);
	(*sys).addPurchase(purchasePtr, messageResponse);
	app.logger().debug("Purchase added in the market place");
}

void ConnectionHandler::setProviderAvailability(Poco::Net::SocketAddress socketAddress,
												Message & messageRequest, 
												Message & messageResponse)
{
	double quantity=0;
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Set provider availability in the market place");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();

	std::string providerId = messageRequest.getParameter("Provider");
	std::string resourceId = messageRequest.getParameter("Resource");
	std::string quantityStr = messageRequest.getParameter("Quantity");
	bool v_result = Poco::NumberParser::tryParseFloat(quantityStr, quantity);
	if (v_result){
		(*sys).setProviderAvailability(providerId, resourceId, quantity, messageResponse);
		app.logger().debug("End set provider availability in the market place");
	}
	else
	{
		throw MarketPlaceException("Invalid quantity", 316);
	}
}

void ConnectionHandler::getBestBids(Poco::Net::SocketAddress socketAddress,
									Message & messageRequest, 
									Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Request for best bid");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();
	std::string providerId = messageRequest.getParameter("Provider");
	std::string serviceId = messageRequest.getParameter("Service");
	// Verifies if the sender sets the provider and service. 
	if ((providerId.empty()) || (serviceId.empty())){
		missingParametersProcedure(messageResponse);
	} 
	else 
	{
		(*sys).getBestBids(providerId, serviceId, messageResponse);
	}
	app.logger().debug("End request for best bid");
}


void ConnectionHandler::getBid(Poco::Net::SocketAddress socketAddress,
												Message & messageRequest, 
												Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information("get Bid");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();

	std::string bidId = messageRequest.getParameter("Bid");
	(*sys).sendBid(bidId, messageResponse);
	app.logger().information("End Get Bid");

}


void ConnectionHandler::getProviderChannel(Poco::Net::SocketAddress socketAddress,
												Message & messageRequest, 
												Message & messageResponse)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information("get Provider Channel");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	MarketPlaceSys *sys = server.getMarketPlaceSubsystem();

	std::string providerId = messageRequest.getParameter("ProviderId");
	(*sys).sendProviderChannel(providerId, messageResponse);
	app.logger().information("End Provider Channel");

}

void ConnectionHandler::terminateProcess(void)
{
	std::cout << "Terminating the server processing" << std::endl;
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information("Terminating the server processing");
	MarketPlaceServer &server = dynamic_cast<MarketPlaceServer&>(app);
	server.terminate();
	app.logger().information("Server processing finished");
}


void ConnectionHandler::errorProcedure(Message & messageResponse)
{
	messageResponse.setParameter("Status_Code", "300");
	messageResponse.setParameter("Status_Description", "Invalid Method");
}

void ConnectionHandler::missingParametersProcedure(Message & messageResponse)
{
	messageResponse.setParameter("Status_Code", "308");
	messageResponse.setParameter("Status_Description", "Missing Parameters");
}




}   /// End Eco namespace

}  /// End ChoiceNet namespace

