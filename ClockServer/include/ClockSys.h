#ifndef ClockSys_INCLUDED
#define ClockSys_INCLUDED

#include <Poco/Util/Application.h>
#include <Poco/Util/Subsystem.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/AutoPtr.h>
#include <vector>
#include <map>
#include <iostream>

#include "FoundationSys.h"
#include "ProbabilityDistribution.h"
#include "DecisionVariable.h"
#include "Service.h"
#include "Listener.h"
#include "Message.h"

namespace ChoiceNet
{
namespace Eco
{

class ClockSys : public FoundationSys
{
public:
    ClockSys(void);
    ~ClockSys(void);
 
    bool isAlreadyListener(std::string idListener);
    
    bool isAlreadyListener(Poco::Net::SocketAddress socketAddress);
    
    void insertListener(std::string idListener, 
					   Poco::Net::SocketAddress socketAddress,
					   ChoiceNet::Eco::Message & messageResponse );

	void startListening(Poco::Net::SocketAddress socketAddress, 
					   Poco::UInt16 port, std::string type,
					   ChoiceNet::Eco::Message & messageResponse);
	
	void insertListenerBytype(std::string type, std::string listenerId);
	
	void addStagedData(Poco::Net::SocketAddress socketAddress, 
					   Poco::FIFOBuffer & fifoIn, 
					   int len);
	
	void getMessage(Poco::FIFOBuffer & fifoIn, int len, Message &message);
	
	bool getMessage(Poco::Net::SocketAddress socketAddress, 
					ChoiceNet::Eco::Message & message);
	
    void setDemandForecaster(PointSetDemandForecaster * demand_forecaster);

    void setTrafficConverter(SimplestTrafficConverter * traffic_converter);


	void sendCurrentPeriod(Poco::Net::SocketAddress socketAddress, 
							ChoiceNet::Eco::Message & messageResponse);
	
	void broadcastPeriodEnd(void);
	
	void broadcastPeriodStart(void);
	
	ChoiceNet::Eco::Message * getActivationMessage(void);
	
	void decreaseActivationMessageCount(std::string serviceId);
	
	void broadcastTerminate(void);
	
	int getPeriod(void);
	
	void incrementPeriod(void);
	
	int getInterval(void);
	
	void incrementInterval(void);
	
	void deleteListener( Poco::Net::SocketAddress socketAddress,
						 ChoiceNet::Eco::Message & messageResponse );
    
    void getServices(std::string serviceId, ChoiceNet::Eco::Message & messageResponse);
    
    void getServices(ChoiceNet::Eco::Message & messageResponse);
    
    void loadDemand(void);

    void loadTrafficConverter(void);
    
    void initialize(Poco::Util::Application &app);
    
	void activateCustomers();
    
protected:

    const char* name() const;
        
private:
    char p_cName;
    
    struct ServiceActivation
	{
		int num_agents;
		Message * message_to_send;
	};
    
    typedef std::map<Poco::Net::SocketAddress, Listener *> Listeners;
    
    Listeners _listeners;
    std::map<std::string, Listener *> _listeners_by_id;
    std::map<std::string, std::vector<std::string> > _listeners_by_type;
    std::map<std::string, ServiceActivation> _services_activation;
    int _interval;
    int _period;
    int _intervals_per_cycle;
    int _interval_for_customer_activation;
    int _executionNumber;
    
};


} /// End Eco namespace

}  /// End ChoiceNet namespace


#endif // ClockSys_INCLUDED
