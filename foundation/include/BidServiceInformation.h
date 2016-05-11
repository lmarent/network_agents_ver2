#ifndef BidServiceInformation_INCLUDED
#define BidServiceInformation_INCLUDED

#include <map>
#include <vector>
#include <Poco/RefCountedObject.h>
#include <Poco/AutoPtr.h>
#include "BidProviderInformation.h"
#include "Datapoint.h"
#include "FoundationException.h"

namespace ChoiceNet
{
namespace Eco
{
	
class BidServiceInformation: public Poco::RefCountedObject
{

public:

    BidServiceInformation();
    ~BidServiceInformation();

	void addProvider(std::string providerId);
	void addProviderBid(Bid * bidPtr);
	void deleteProviderBid(Bid * bidPtr);
	std::string getBestBids(int fronts_to_include);
	void organizeByParetoFrontiers(void);
	void printParetoFrontier();
	void calculateNeighbors(Bid * bidPtr);
	void getProviderBids(std::string providerId, std::map<std::string, std::vector<std::string> > &bids);
	bool isBidActive(std::string providerId, std::string bidId);
	
private:
    typedef std::vector<Datapoint *> Front;
    
    std::map<std::string, BidProviderInformation *> _provider_information;
    Front _serviceBids;
    std::map<int,Front> _paretoFrontiers;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // BidServiceInformation_INCLUDED

