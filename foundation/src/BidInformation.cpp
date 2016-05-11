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

#include "BidServiceInformation.h"
#include "BidInformation.h"

namespace ChoiceNet
{
namespace Eco
{

BidInformation::BidInformation()
{
	
}
BidInformation::~BidInformation()
{
	// std::cout << "Deleting the bids in the system" << std::endl;
	
	// Release the memory assigned to service information objects
	BidServiceInformationContainer::iterator it_service;
	it_service = _service_information.begin();
	while( (it_service!=_service_information.end())  )
	{
		delete it_service->second;
		_service_information.erase(it_service);
		++it_service;
	}

	// std::cout << "Deleted the bids in the system" << std::endl;

}
	
void BidInformation::addBidToService(Bid * bidPtr)
{
	// Verifies if the service is in the container
	if (_service_information.count((*bidPtr).getService()) == 0){
		// If not it creates a new node for the service
		addService((*bidPtr).getService());
	}
	
	// In any case the procedure insert the bid in the service node
	std::map<std::string ,BidServiceInformation *>::iterator it;
	it = _service_information.find((*bidPtr).getService());
	BidServiceInformation *ptr;
	ptr = it->second;
	(*ptr).addProviderBid(bidPtr);
	(*ptr).calculateNeighbors(bidPtr);

}

void BidInformation::deleteBidToService(Bid * bidPtr)
{
	// In any case the procedure insert the bid in the service node
	std::map<std::string ,BidServiceInformation *>::iterator it;
	it = _service_information.find((*bidPtr).getService());
	if (it != _service_information.end() )
	{
		(*(it->second)).deleteProviderBid(bidPtr);
	}
}

void BidInformation::addService(std::string serviceId)
{
	BidServiceInformation *serviceInformationPtr = new BidServiceInformation();
	_service_information.insert ( std::pair<std::string,BidServiceInformation*>
									   (serviceId,serviceInformationPtr) );	
}

std::string BidInformation::getBestBids(std::string serviceIdParam, int fronts)
{
	
	// std::cout << "Beginning getBestBids" << std::endl;
	std::string val_return;
	std::map<std::string ,BidServiceInformation*>::iterator it;
	it = _service_information.find(serviceIdParam);
	if (it != _service_information.end())
	{
		//std::cout << "getBestBids - the service was found" << std::endl;
		BidServiceInformation *ptr = it->second;
		val_return = (*ptr).getBestBids(fronts);
	}
	else
	{
        //std::cout << "getBestBids - the service was not found" << std::endl;
        // The service is not in the container
		// creates an empty xml document
		// Creates an xml message with element "bestBids" as root
		Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
		Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement("bestBids");
		pDoc->appendChild(pRoot);
		Poco::XML::AutoPtr<Poco::XML::Element> pFront = 
										pDoc->createElement("Front");
		Poco::XML::AutoPtr<Poco::XML::Element> pParNum = 
										pDoc->createElement("Pareto_Number");
		Poco::XML::AutoPtr<Poco::XML::Text> pText1 = 
										pDoc->createTextNode("0");
		pParNum->appendChild(pText1);
		pFront->appendChild(pParNum);
		pRoot->appendChild(pFront);
		Poco::XML::DOMWriter writer;
		writer.setNewLine("\n");
		writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
		std::stringstream  output; 
		writer.writeNode(output, pDoc);	
		val_return = output.str();
	}
	//std::cout << "End getBestBids: \n" << val_return << std::endl;
	return val_return;
}

bool BidInformation::existService(std::string serviceId)
{
	bool val_return;
	// Find the service in the container
	if (_service_information.count(serviceId) == 0){
		val_return = false;
	}else{
		val_return = true;
	}
	return val_return;
}

void BidInformation::getProviderBids(std::string providerId, std::map<std::string, std::vector<std::string> > &bids)
{
	std::map<std::string ,BidServiceInformation*>::iterator it;
	it = _service_information.begin();
	while (it != _service_information.end())
	{
		//std::cout << "getBestBids - the service was found" << std::endl;
		BidServiceInformation *ptr = it->second;
		(*ptr).getProviderBids(providerId, bids);
		++it;
	}
	
}

bool BidInformation::isBidActive(std::string serviceId, std::string providerId, std::string bidId)
{
	std::map<std::string ,BidServiceInformation*>::iterator it;
	it = _service_information.find(serviceId);
	if (it != _service_information.end())
	{
		return (*(it->second)).isBidActive(providerId, bidId);
	}
	else
	{
		return false;
	}
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace

