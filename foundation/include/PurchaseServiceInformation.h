#ifndef PurchaseServiceInformation_INCLUDED
#define PurchaseServiceInformation_INCLUDED

#include <map>
#include <vector>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>

#include "Purchase.h"


namespace ChoiceNet
{
namespace Eco
{

struct PurchaseServiceBidStruct
{
	int 		_period;
	std::string _serviceId;
	std::string _bidId;
	double 		_quantity;
	int 		_execution_count;
};

class PurchaseServiceInformation
{

public:

    PurchaseServiceInformation();
    ~PurchaseServiceInformation();

	void addPurchase(Purchase *purchasePtr);
	void getPurchases(Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
					  Poco::XML::AutoPtr<Poco::XML::Element> pParent, 
					  std::map<std::string, std::vector<std::string> > & bids);
	
	void createCompetitorBidNode(Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
								 Poco::XML::AutoPtr<Poco::XML::Element> pParent, 
								 std::string id, double quantity);
								 
	void createBidNode(Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
					   Poco::XML::AutoPtr<Poco::XML::Element> pParent, 
					   std::string id, double quantity);

    // Store purchases for the service in the database pool.
    void toDatabase(Poco::Data::SessionPool * _pool, int execution_count, int period, std::string serviceId);

private:
    
    std::map<std::string, double> _summaries_by_bid;
    std::vector<std::string> _detail;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // PurchaseServiceInformation

