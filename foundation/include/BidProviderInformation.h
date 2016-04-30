#ifndef BidProviderInformation_INCLUDED
#define BidProviderInformation_INCLUDED

#include <map>
#include <Poco/RefCountedObject.h>
#include <Poco/AutoPtr.h>
#include "Bid.h"

namespace ChoiceNet
{
namespace Eco
{

class BidProviderInformation: public Poco::RefCountedObject
{

public:

	BidProviderInformation();
	
	~BidProviderInformation();
	
	void addBid(Bid * bidPtr);
	
	void deleteBid(Bid * bidPtr);
	
	void calculateNeighbors(Bid *bidPtr);
	
	void getBids(std::map<std::string, std::vector<std::string> > &bids_parameter);	
	
	bool isBidActive(std::string bidId);
	
private:
    std::map<std::string, Bid *> _bids;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // BidProviderInformation_INCLUDED
