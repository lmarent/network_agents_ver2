#ifndef PurchaseInformation_INCLUDED
#define PurchaseInformation_INCLUDED

#include <map>
#include <Poco/RefCountedObject.h>
#include <Poco/AutoPtr.h>
#include "PurchaseServiceInformation.h"

namespace ChoiceNet
{
namespace Eco
{

class PurchaseInformation
{

public:
	PurchaseInformation();
	~PurchaseInformation();
	
	void addPurchaseToService(Purchase * purchasePtr);
	void addService(std::string serviceId);
	bool existService(std::string serviceId);
	void getPurchasesForProvider( Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
										 Poco::XML::AutoPtr<Poco::XML::Element> pParent,
										 std::map<std::string, std::vector<std::string> > &bids);

private:

	typedef std::map<std::string, PurchaseServiceInformation *> PurchaseServiceInformationContainer;
	PurchaseServiceInformationContainer _service_information;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // PurchaseInformation_INCLUDED
