#include <map>
#include <iostream>
#include <sstream>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/NumberFormatter.h>

#include "PurchaseServiceInformation.h"
#include "PurchaseInformation.h"

namespace ChoiceNet
{
namespace Eco
{

PurchaseInformation::PurchaseInformation()
{
	
}
PurchaseInformation::~PurchaseInformation()
{
	// std::cout << "Deleting the purchase in the system" << std::endl;
	
	// Release the memory assigned to service information objects
	PurchaseServiceInformationContainer::iterator it_service;
	it_service = _service_information.begin();
	while( (it_service!=_service_information.end())  )
	{
		// std::cout << "Deleting service purchase in the system" << std::endl;
		delete it_service->second;
		_service_information.erase(it_service);
		++it_service;
	}

	// std::cout << "Deleted the purchase in the system" << std::endl;

}

void PurchaseInformation::addService(std::string serviceId)
{
	PurchaseServiceInformationContainer::iterator it; 
	it = _service_information.find(serviceId);
	if (it == _service_information.end())
	{
		PurchaseServiceInformation * purchaseInfoPtr = new PurchaseServiceInformation();
		_service_information.insert(std::pair<std::string, PurchaseServiceInformation *> (serviceId,purchaseInfoPtr));
	}
	
}
	
void PurchaseInformation::addPurchaseToService(Purchase * purchasePtr, bool purchaseFound)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug(Poco::format("Starting addPurchaseToService:%b", purchaseFound));

	// Verifies if the service is in the container
	std::string serviceId = (*purchasePtr).getService();

	app.logger().debug(Poco::format("addPurchaseToService - service:%s", serviceId));

	PurchaseServiceInformationContainer::iterator it; 
	it = _service_information.find(serviceId);
	if (it == _service_information.end())
	{
		// If not it creates a new node for the service
		PurchaseServiceInformation * purchaseInfoPtr = new PurchaseServiceInformation();
		_service_information.insert(std::pair<std::string, PurchaseServiceInformation *> (serviceId,purchaseInfoPtr));
	}
	
	// In any case the procedure insert the purchase in the service node
	it = _service_information.find(serviceId);
	PurchaseServiceInformation *ptr;
	ptr = it->second;
	(*ptr).addPurchase(purchasePtr, purchaseFound);

	app.logger().debug("Ending addPurchaseToService");

}

void PurchaseInformation::getPurchasesForProvider( Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
												   Poco::XML::AutoPtr<Poco::XML::Element> pParent,
												   std::map<std::string, std::vector<std::string> > &bids)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Starting getPurchasesForprovider");
	
	PurchaseServiceInformationContainer::iterator it; 
	it = _service_information.begin();
	while (it != _service_information.end())
	{
		 // std::cout << "Inside the while in getPurchasesForProvider" << std::endl;
		(*(it->second)).getPurchases(pDoc, pParent, bids);
		++it;
	}
		
	app.logger().debug("Ending getPurchasesForprovider");

}

void PurchaseInformation::toDatabase(Poco::Data::SessionPool * _pool, int execution_count, int period)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug(Poco::format("Starting purchase information toDatabase exec_count:%d period:%d", execution_count, period));


	PurchaseServiceInformationContainer::iterator it;
	it = _service_information.begin();
	while (it != _service_information.end())
	{
		(it->second)->toDatabase(_pool, execution_count, period, it->first);
		++it;
	}
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace

