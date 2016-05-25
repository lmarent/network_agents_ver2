#ifndef MarketPlaceSys_INCLUDED
#define MarketPlaceSys_INCLUDED

#include <Poco/Util/Application.h>
#include <Poco/Util/Subsystem.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/AutoPtr.h>
#include <Poco/Types.h>
#include <Poco/Data/SessionPool.h>
#include <vector>
#include <map>
#include <iostream>

#include "FoundationSys.h"
#include "Listener.h"
#include "BidInformation.h"
#include "PurchaseInformation.h"
#include "Provider.h"
#include "Bid.h"
#include "Resource.h"


namespace ChoiceNet
{
namespace Eco
{

enum MARKET_HISTORY_PERIOD
{ 
    START = 0,
    END = 1
};

class MarketPlaceSys : public FoundationSys
{
public:
    MarketPlaceSys(void);
    
    ~MarketPlaceSys(void);

    bool isAlreadyListener(std::string idListener);

	bool isAlreadyListener(Poco::Net::SocketAddress socketAddress);

	void getMessage(Poco::FIFOBuffer & fifoIn, 
					int len, Message &message );
	
	bool getMessage(Poco::Net::SocketAddress socketAddress, 
					Message & message);

	void addStagedData(Poco::Net::SocketAddress socketAddress,
					   Poco::FIFOBuffer & fifoIn, 
					   int len);

    void insertListener(std::string idListener, 
					   Poco::Net::SocketAddress socketAddress,
					   Message & messageResponse );

    void removeListener(std::string idListener);

    void startListening(Poco::Net::SocketAddress socketAddress, 
					   Poco::UInt16 port, std::string type,
					   ProviderCapacityType capacity_type,
					   Message & messageResponse );
					   
	void insertListenerBytype(std::string type, std::string listenerId);
	
	void addAsClockListener(Poco::UInt16 port, std::string type);
		
	void initializePeriodSession(unsigned period);
	
	void broadCastInformation(Message & message, std::string type);
	 
	void finalizePeriodSession(unsigned  period, Message & messageResponse);
	
	void sendProviderPurchaseInformation(void);
	
	void storeCurrentPurchaseInformation(void);

	void addBid(Bid * bidPtr, Message & messageResponse);
	
	void deleteBid(Bid * bidPtr, Message & messageResponse);
	
	void addPurchase(Purchase * purchasePtr, Message & messageResponse);
	
	void setProviderAvailability(std::string providerId,
							     std::string resourceId,
								 double quantity,
								 Message & messageResponse);
	
	void getBestBids(std::string providerId, std::string serviceId, 
					 Message & messageResponse);
					 
	void getBestBids(std::string providerId, std::string serviceId, 
								int fronts, Message & messageResponse);

    void sendBid(std::string bidId, Message & messageResponse);
    
    void sendProviderChannel(std::string providerId, Message & messageResponse);

    void initialize(Poco::Util::Application &app);
    
    Provider * getProvider(std::string providerId);
       
    Bid * getBid(std::string bidId);
	
	void addPurchaseBulkCapacity(Provider *provider, Service *service, Bid * bid, Purchase * purchasePtr, Message & messageResponse);
	
	void addPurchaseByBidCapacity(Provider *provider, Service *service, Bid * bid, Purchase * purchasePtr, Message & messageResponse);
	
	// Specificates if the information should be transmited to the provider.
	bool sendInformation(unsigned interval);
	
	void saveInformation();
	
	void disseminateInformation();
	
	void reinitiateDataContainers(MARKET_HISTORY_PERIOD subperiod);
    
    void saveBidInformation(void);

    void broadCastBidInformation(void);
    
    void activatePresenter(void);
    
protected:
    virtual const char* name() const;    
    void sendMessageToClock(Message & message, Message & response);
        
private:
    std::string p_cName;
    Poco::Net::StreamSocket *_clockSocket;
    
    // Container for listeners
    typedef std::map<Poco::Net::SocketAddress, Listener*> Listeners;
    Listeners _listeners;
    
    // These two containers act as indexes for the listeners container.
    std::map<std::string, std::vector<std::string> > _listeners_by_type;
    std::map<std::string, Listener *> _listeners_by_id;
    std::map<std::string, Provider *> _providers;
    
    BidInformation *_current_bids;
    
    PurchaseInformation *_current_purchases;
    
    typedef std::map<std::string, Purchase *> PurchaseContainer;
    
    PurchaseContainer _purchases;
    
    typedef std::map<std::string, Bid *> BidContainer;
    BidContainer _bids; // This Holds all bids 
    BidContainer _bids_to_broadcast; // Bids received in the current period and need to be broadcasted
    
    typedef std::map<std::string, PurchaseInformation*> PurchaseHistory;
    PurchaseHistory _purchase_history;
            
	unsigned _period;
	unsigned _intervals_per_cycle;
	unsigned _send_interval;

	Poco::Data::SessionPool * _pool;

};


}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // MarketPlaceSys_INCLUDED
