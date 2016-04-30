#ifndef Message_INCLUDED
#define Message_INCLUDED

#include <string>
#include <map>

namespace ChoiceNet
{
namespace Eco
{

#define LINE_SEPARATOR "\r\n"
#define MESSAGE_SIZE 10

enum Method 
{ 
  undefined =1, 
  connect =2, 
  receive_bid=3, 
  get_best_bids=4, 
  send_port=5, 
  start_period=6, 
  end_period=7, 
  receive_purchase=8,
  get_current_period=9,
  disconnect=10,
  get_services=11,
  activate_consumer=12,
  receive_purchase_feedback=13,
  send_availability=14,
  receive_bid_information=15,
  get_bid = 16,
  get_provider_channel = 17,
  get_unitary_cost = 18,
  activate_presenter=19
};


class Message 
{
public:
	Message();
	Message(std::string data);
	
	~Message(); 
	
	Method getMethod();
	
	void setData(std::string data);
	
	std::string getParameter(std::string param);
	
	bool existsParameter(std::string param);
	
	void setMethod(Method method);
	
	std::string getStrMethod();
	
	void setParameter(std::string parameterKey, std::string parameterValue);
	
	void setParameter(std::string parameterKey, int parameterValue);

	void setParameter(std::string parameterKey, double parameterValue);
	
	std::string to_string();
	
	void setResponseOk();
	
	bool isMessageStatusOk();
	
	void setBody(std::string body);
	
	bool isComplete(size_t lenght);

private:
	Method _method;
	std::map<std::string, std::string> _parameters;
	std::string _body;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // Message_INCLUDED
