#include <vector>
#include <Poco/AutoPtr.h>
#include <iostream>

#include "FoundationException.h"
#include "Bid.h"
#include "BidProviderInformation.h"

namespace ChoiceNet
{
namespace Eco
{


BidProviderInformation::BidProviderInformation()
{
	
}
	
BidProviderInformation::~BidProviderInformation()
{
	// std::cout << "Deleting provider information" << std::endl;
	
	// Release the memory assigned to objects
	for(std::map<std::string, Bid *>::iterator it = _bids.begin(); it != _bids.end(); ++it) 
	{
		_bids.erase(it);
	}

	// std::cout << "Delete provider information" << std::endl;
		
}

void BidProviderInformation::calculateNeighbors(Bid *bidPtr)
{
    // std::cout << "Initiate calculate neighbors for bid:" << (*bidPtr).getId() << std::endl;
    
    // Iterate over the providers and send the information of purchases for the period.
    for(std::map<std::string, Bid *>::iterator it = _bids.begin(); it != _bids.end(); ++it) 
    {
		// Neighbor is a reflexive relationship.
		std::string neigborId = it->first;
		std::string bidId = (*bidPtr).getId();
		(*bidPtr).addNeighbor(neigborId);
		(*(it->second)).addNeighbor(bidId);
	}
	// std::cout << "Ending calculate neighbors for bid:" << (*bidPtr).getId() << "Bids included:" << (*bidPtr).getNumberOfNeighbors() << std::endl;
}
	
void BidProviderInformation::addBid(Bid * bidPtr)
{
	std::map<std::string, Bid *>::iterator it;
	it = _bids.find((*bidPtr).getId());
	if (it == _bids.end())
	{
		_bids.insert(std::pair<std::string, Bid *>((*bidPtr).getId(), bidPtr));
	}
	else
	{
		FoundationException("Bid is already inserted in the provider information", 311);
	}
}

void BidProviderInformation::deleteBid(Bid * bidPtr)
{
	std::map<std::string, Bid *>::iterator it;
	it = _bids.find((*bidPtr).getId());
	if (it != _bids.end())
	{
		_bids.erase(it);
	}
	else
	{
		FoundationException("Bid was not found in the provider information", 312);
	}
	
}
	
void BidProviderInformation::getBids(std::map<std::string, std::vector<std::string> > &bids_parameter)
{
    std::cout << "Number of bids for the provider:" << _bids.size() << std::endl;
    
    // Iterate over the provider bids
    for(std::map<std::string, Bid *>::iterator it = _bids.begin(); it != _bids.end(); it++) 
    {
		std::vector<std::string> neighbors;
		(*(it->second)).getNeighbors(neighbors);
		bids_parameter.insert( std::pair<std::string, std::vector<std::string> >((*(it->second)).getId(), neighbors) );
		
		/* std::cout << "Printing the neighbors of the bid:" << (*(*it)).getId() << std::endl;
		//for(std::vector<std::string>::iterator it_nei = neighbors.begin(); it_nei != neighbors.end(); ++it_nei) 
		//{
		//	std::cout << "bid:" << (*it_nei) << std::endl;
		} */
	}
	
}

bool BidProviderInformation::isBidActive(std::string bidId)
{
	std::map<std::string, Bid *>::iterator it;
	it = _bids.find(bidId);
	if (it != _bids.end())
	{
		return true;
	}
	else
	{
		return false;
	}

}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
