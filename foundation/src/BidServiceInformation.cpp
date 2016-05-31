#include <map>
#include <iostream>
#include <sstream>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/XML/XMLStream.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Util/ServerApplication.h>



#include "BidProviderInformation.h"
#include "BidServiceInformation.h"
#include "NondominatedsortAlgo.h"
#include "ParetoAlgo.h"

namespace ChoiceNet
{
namespace Eco
{
	
BidServiceInformation::BidServiceInformation()
{

}
BidServiceInformation::~BidServiceInformation()
{
	// std::cout << "deleting the service information" << std::endl;
			
	// release the memory for objects in the container
	std::map<std::string, BidProviderInformation *>::iterator it_provider;
	it_provider = _provider_information.begin();
	while( (it_provider!=_provider_information.end())  )
	{
		delete it_provider->second;
		_provider_information.erase(it_provider);
		++it_provider;
	}
		
	// std::cout << "deleted the service information" << std::endl;
		
}

void BidServiceInformation::addProvider(std::string providerId)
{
	BidProviderInformation *ptr = new BidProviderInformation();
	_provider_information.insert ( std::pair<std::string,BidProviderInformation*>
									   (providerId, ptr) );
}

void BidServiceInformation::organizeByParetoFrontiers(void)
{
	int status = 0;
	Poco::Util::Application& app = Poco::Util::Application::instance();	
	app.logger().debug("Entering organizeByParetoFrontiers");
	
	// eliminates the current organization if it exists
	std::map<int, Front>::iterator it_fronts;
	it_fronts = _paretoFrontiers.begin();
	while( (it_fronts != _paretoFrontiers.end())  )
	{
		_paretoFrontiers.erase(it_fronts);
		++it_fronts;
	}
	
	Front::iterator it;
	it = _serviceBids.begin();
	while( (it != _serviceBids.end())  )
	{
		status = (*(*it)).getParetoStatus();
		std::string statusStr = Poco::NumberFormatter::format(status);	
		app.logger().debug("Bid:" + (*it)->getId() + ":Pareto_Status:" + statusStr); 

		// The pareto status is a number that indicates as greater the value
		// teh point is less dominated. We mutiply by minus one so the will be ordered
		// in the map in reverse which means greater values first. 
		status = status * -1; 
		// Verifies if the status is in the container
		std::map< int, Front >::iterator it_result;
		it_result = _paretoFrontiers.find(status);
		if (it_result != _paretoFrontiers.end())
		{
			(it_result->second).push_back(*it);		
		}
		else
		{	// If it does not exists, then we create a new status
			Front new_front;
			new_front.push_back(*it);	
			_paretoFrontiers.insert( std::pair<int,Front>(status, new_front) );
		}
		++it;
	}
		
	app.logger().debug("Ended the pareto frontier organization");
	std::string NbrFrontsStr = Poco::NumberFormatter::format(_paretoFrontiers.size());	
	app.logger().debug("Fronts Nbr: " + NbrFrontsStr);
}

void BidServiceInformation::printParetoFrontier()
{
	Poco::Util::Application& app = Poco::Util::Application::instance();	
	app.logger().debug("Entering printParetoFrontier");

	std::vector<Datapoint *>::iterator it;
	it = _serviceBids.begin();
	while( (it != _serviceBids.end())  )
	{
		(*(*it)).print();
		++it;
	}
	app.logger().debug("Leaving printParetoFrontier");
}

void BidServiceInformation::addProviderBid(Bid * bidPtr)
{
	// Verifies if the provider is in the container
	if (_provider_information.count((*bidPtr).getProvider()) == 0){
		// If not it creates a new node for the provider
		addProvider((*bidPtr).getProvider());
	}
	
	// In any case the procedure insert the bid in the provider node
	std::map<std::string ,BidProviderInformation*>::iterator it;
	it = _provider_information.find((*bidPtr).getProvider());
	BidProviderInformation *ptr = it->second;
	(*ptr).addBid(bidPtr);
	
	// std::cout << "the entering bid is:" << std::endl;
	Message message;
	(*bidPtr).toMessage(message);
	// std::cout << message.to_string() << "Pareto Number:" << (*bidPtr).getParetoStatus() << "Dominated Count:" << (*bidPtr).numDominated() << std::endl;
	
	// In any case insert also the pointer to the bid in the list of bids
	 _serviceBids.push_back(bidPtr);
	ParetoAlgo* algo = new NondominatedsortAlgo();
    int numPareto = algo->computeFrontier(_serviceBids);
    
	// Organize the bids by pareto frontiers.
	organizeByParetoFrontiers();
}

void BidServiceInformation::deleteProviderBid(Bid * bidPtr)
{
	// Search the provider of the Bid.
	std::map<std::string ,BidProviderInformation*>::iterator it;
	it = _provider_information.find((*bidPtr).getProvider());
	if (it != _provider_information.end())
	{
		(*(it->second)).deleteBid(bidPtr);
	
	    // Delete the bid from the pareto list.
	    Front::size_type sz = _serviceBids.size();
	    for (unsigned i=0; i<sz; i++)
	    {
			if ((_serviceBids[i])->getId() == bidPtr->getId() )
				_serviceBids.erase (_serviceBids.begin()+i);
		}
	
		// Compute the ne pareto frontier.
		ParetoAlgo* algo = new NondominatedsortAlgo();
		int numPareto = algo->computeFrontier(_serviceBids);
    
		// Organize the bids by pareto frontiers.
		organizeByParetoFrontiers();
	}
}

void BidServiceInformation::calculateNeighbors(Bid * bidPtr)
{
	// std::cout << "Initiating calculateNeighbors" << std::endl;
	// For now this function put every Bid from other provider as a neighbor.
	std::map<std::string, BidProviderInformation *>::iterator it;
	it = _provider_information.begin();
	while (it != _provider_information.end() )
	{
		if (((*bidPtr).getProvider()).compare(it->first) != 0 ){
			(*(it->second)).calculateNeighbors(bidPtr);
		}
		++it; 
	}
	// std::cout << "Ending calculateNeighbors" << std::endl;
}


std::string BidServiceInformation::getBestBids( int fronts_to_include )
{
	// Local variables
	int status = 1;
	
	// Creates an xml message with element "bestBids" as root
	Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;
	Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement("bestBids");
	pDoc->appendChild(pRoot);
	
	int size = _paretoFrontiers.size();
	// std::cout << "Number of fronts:";
	// std::cout << size;
	// std::cout << std::endl;
	if ( size > 0 )
	{
		int fronts_included = 1;
		std::map<int,Front>::iterator it;
		it = _paretoFrontiers.begin();
		// std::cout << "Beginning to pass over the pareto frontier" << std::endl;
		while ( ( fronts_included <= fronts_to_include) 
			   and (it != _paretoFrontiers.end())  )
		{
			status = it->first;
			status = status * -1;
			std::string elementStr = "";
			Poco::NumberFormatter::append(elementStr, status);
			Poco::XML::AutoPtr<Poco::XML::Element> pFront = 
										pDoc->createElement("Front");
			Poco::XML::AutoPtr<Poco::XML::Element> pParNum = 
										pDoc->createElement("Pareto_Number");
			Poco::XML::AutoPtr<Poco::XML::Text> pText1 = 
										pDoc->createTextNode(elementStr);
			pParNum->appendChild(pText1);
			pFront->appendChild(pParNum);
			// loop throughout the vector and create and element on the 
			// xml by each element in the vector
			
			Front::iterator it_vector;
			it_vector = (it->second).begin();
			// std::cout << "putting the information of the bids on the front" << std::endl;
			while( (it_vector != (it->second).end())  )
			{
				Bid * bidPtr = (Bid*) (*it_vector);
				bidPtr->to_XML(pDoc, pFront);
				++it_vector;
			}
			pRoot->appendChild(pFront);
			++it;
			++fronts_included;
		}
		// std::cout << "It went thought all the pareto frontiers" << std::endl;
	}
	else
	{
		// Sends an empty front.
		std::string elementStr = "";
		Poco::NumberFormatter::append(elementStr, 0);
		Poco::XML::AutoPtr<Poco::XML::Element> pFront = 
										pDoc->createElement("Front");
		Poco::XML::AutoPtr<Poco::XML::Element> pParNum = 
										pDoc->createElement("Pareto_Number");
		Poco::XML::AutoPtr<Poco::XML::Text> pText1 = 
										pDoc->createTextNode(elementStr);
		pParNum->appendChild(pText1);
		pFront->appendChild(pParNum);		
		pRoot->appendChild(pFront);
	}
	
	// std::cout << "putting the xml into an string" << std::endl;
	Poco::XML::DOMWriter writer;
	writer.setNewLine("\n");
	writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
	std::stringstream  output; 
	writer.writeNode(output, pDoc);	
	// std::cout << "Ending BidServiceInformation - getBestBids" << std::endl;
	//std::cout << output.str() << std::endl;
	return output.str();
}

void BidServiceInformation::getProviderBids(std::string providerId, std::map<std::string, std::vector<std::string> > &bids)
{
	std::map<std::string, BidProviderInformation *>::iterator it;
	it = _provider_information.find(providerId);
	if (it != _provider_information.end())
	{
		(*(it->second)).getBids(bids);
	}
}

bool BidServiceInformation::isBidActive(std::string providerId, std::string bidId)
{
	std::map<std::string, BidProviderInformation *>::iterator it;
	it = _provider_information.find(providerId);
	if (it != _provider_information.end())
	{
		return (*(it->second)).isBidActive(bidId);
	}
	else
	{
		return false;
	}
	
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace



