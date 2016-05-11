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
		
private:
    
    std::map<std::string, double> _summaries_by_bid;
    std::vector<std::string> _detail;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // PurchaseServiceInformation

