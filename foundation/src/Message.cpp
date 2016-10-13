#include <string>
#include <map>
#include <iostream>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberFormatter.h>
#include "Message.h"
#include "FoundationException.h"
#include <stdlib.h>

namespace ChoiceNet
{
namespace Eco
{

Message::Message():_method(undefined)
{
}

Message::Message(std::string data):
_method(undefined)
{
	setData(data);
}

Message::~Message()
{

}

void Message::setData(std::string data)
{
	Poco::StringTokenizer lines(data, LINE_SEPARATOR);
	int count = lines.count();
	if (count > 0)
	{
		// Get the method
		std::string methodLine = lines[0];
		Poco::StringTokenizer methodParam(methodLine, ":",Poco::StringTokenizer::TOK_TRIM);
		if (methodParam.count() == 2){
			if (methodParam[1].compare("receive_bid") == 0){
				_method = receive_bid;
			}
			else if (methodParam[1].compare("connect") == 0){
				_method = connect;
			}
			else if (methodParam[1].compare("send_port") == 0){
				_method = send_port;
			}
			else if (methodParam[1].compare("start_period") == 0){
				_method = start_period;
			}
			else if (methodParam[1].compare("end_period") == 0){
				_method = end_period;
			}
			else if (methodParam[1].compare("receive_purchase") == 0){
				_method = receive_purchase;
			}
			else if (methodParam[1].compare("get_best_bids") == 0){
				_method = get_best_bids;
			}
			else if (methodParam[1].compare("get_current_period") == 0){
				_method = get_current_period;
			}
			else if (methodParam[1].compare("disconnect") == 0){
				_method = disconnect;
			}
			else if (methodParam[1].compare("get_services") == 0){
				_method = get_services;
			}
			else if (methodParam[1].compare("activate_consumer") == 0){
				_method = activate_consumer;
			}
			else if (methodParam[1].compare("receive_purchase_feedback") == 0){
				_method = receive_purchase_feedback;
			}
			else if (methodParam[1].compare("send_availability") == 0){
				_method = send_availability;
			}
			else if (methodParam[1].compare("receive_bid_information") == 0){
				_method = receive_bid_information;
			}
			else if (methodParam[1].compare("get_bid") == 0){
				_method = get_bid;
			}
			else if (methodParam[1].compare("get_provider_channel") == 0){
				_method = get_provider_channel;
			}
			else if (methodParam[1].compare("get_unitary_cost") == 0){
				_method = get_unitary_cost;
			}
			else if (methodParam[1].compare("activate_presenter") == 0){
				_method = activate_presenter;
			}
			else if (methodParam[1].compare("get_availability") == 0){
				_method = get_availability;
			}
			else{
				_method = undefined;
			}
		}

		// Repeat for each line to get the parameters and body
		if (count > 1){
			int i = 1;
			while (i < count) {
				std::string paramLine = lines[i];
				if (paramLine.compare(LINE_SEPARATOR) != 0)
				{
					Poco::StringTokenizer linesParam(paramLine, ":",Poco::StringTokenizer::TOK_TRIM);
					if (linesParam.count() == 2){
						_parameters.insert ( std::pair<std::string,std::string>
									   (linesParam[0],linesParam[1]) );
					}
				}
				else
				{
					break;
				}
				i++;
			}

			// If i < count the rest of the lines are the body so we
			// have to concatenate them.
			while (i < count) {
				_body.append(lines[i]);
				++i;
			}
		}
	}
}

Method Message::getMethod()
{
	return _method;
}

void Message::setMethod(Method method)
{
	_method = method;

}
void Message::setParameter(std::string parameterKey, std::string parameterValue)
{
	// first we ask if the parameter key is already on the list
	std::map<std::string, std::string>::iterator it;
	it = _parameters.find(parameterKey);
	if(it != _parameters.end()) {
		// if it is already we create an exception
		throw FoundationException("Parameter is already included");
	}
	else{
		// if not, we add the parameter to the list.
		_parameters.insert ( std::pair<std::string,std::string>
									   (parameterKey, parameterValue) );
	}
}

void Message::setParameter(std::string parameterKey, int parameterValue)
{
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, parameterValue);
	setParameter(parameterKey, valueStr);
}


void Message::setParameter(std::string parameterKey, double parameterValue)
{
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, parameterValue);
	setParameter(parameterKey, valueStr);
}


std::string Message::getStrMethod()
{
    std::string result;
    switch (_method)
    {
       case undefined:
          result = "undefined";
          break;
	   case connect:
          result = "connect";
          break;
       case receive_bid:
          result = "receive_bid";
          break;
       case send_port:
          result = "send_port";
          break;
       case start_period:
          result = "start_period";
          break;
       case end_period:
          result = "end_period";
          break;
       case receive_purchase:
          result = "receive_purchase";
          break;
       case get_best_bids:
		  result = "get_best_bids";
		  break;
	   case get_current_period:
	      result = "get_current_period";
		  break;
	   case disconnect:
	      result = "disconnect";
		  break;
	   case get_services:
		  result = "get_services";
		  break;
	   case activate_consumer:
		  result = "activate_consumer";
		  break;
	   case activate_presenter:
		  result = "activate_presenter";
		  break;
	   case receive_purchase_feedback:
		  result = "receive_purchase_feedback";
		  break;
	   case send_availability:
		  result = "send_availability";
		  break;
	   case receive_bid_information:
		  result = "receive_bid_information";
		  break;
	   case get_bid:
		  result = "get_bid";
		  break;
	   case get_provider_channel:
		  result = "get_provider_channel";
		  break;
	   case get_unitary_cost:
		  result = "get_unitary_cost";
		  break;
	   case get_availability:
	   	  result = "get_availability";
	   	  break;
    }
    return result;
}

std::string Message::getParameter(std::string param)
{
	std::string val_return;
	// std::cout << "Requested parameter:" << param << std::endl;
	std::map<std::string, std::string>::iterator it;
	it = _parameters.find(param);
	if(it != _parameters.end()) {
		val_return = it->second;
	}
	else{
			std::string messageStr = "Parameter ";
			messageStr.append(param);
			messageStr.append(" not found");
			throw FoundationException(messageStr, 308);
	}
	return val_return;
}

bool Message::existsParameter(std::string param)
{
	bool exists = false;
	std::map<std::string, std::string>::iterator it;
	it = _parameters.find(param);
	if(it != _parameters.end()) {
		exists = true;
	}
	return exists;
}

std::string Message::to_string()
{
	std::string result;
	result = "Method:";
	result.append(getStrMethod());
	result.append(LINE_SEPARATOR);

	std::string result2;
	std::map<std::string,std::string>::iterator it;
	for (it=_parameters.begin(); it!=_parameters.end(); ++it)
	{
	   result2.append(it->first);
	   result2.append(":");
	   result2.append(it->second);
	   result2.append(LINE_SEPARATOR);
	}
	result2.append(LINE_SEPARATOR);
	result2.append(_body);

	// Append the size of the message, so we can know when to stop waiting for data
	// in the other size of the socket.

	result.append("Message_Size:");
	unsigned size = result2.size() + result.size() + 2 + MESSAGE_SIZE;
	Poco::NumberFormatter::append(result, size, MESSAGE_SIZE);
	result.append(LINE_SEPARATOR);
	result.append(result2);
	// std::cout << "Message size:" << size << "final string size" << result.size();
	return result;
}

void Message::setResponseOk()
{
    setParameter("Status_Code", "200");
    setParameter("Status_Description", "OK");
}

bool Message::isMessageStatusOk()
{
	bool val_return;
	int status = atoi((getParameter("Status_Code")).c_str());
	if (status != 200){
		val_return = false;
	}else{
		val_return = true;
	}
	return val_return;
}

void Message::setBody(std::string body)
{
	_body = body;
}

bool Message::isComplete(size_t lenght)
{
	int messageSize = atoi((getParameter("Message_Size")).c_str());
	// std::cout << "Parameter:" << lenght << "Size:" << messageSize << std::endl;
	if (lenght == messageSize)
		return true;
	else
		return false;
}


}  /// End Eco namespace

}  /// End ChoiceNet namespace

