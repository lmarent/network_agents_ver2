#ifndef BidInformation_INCLUDED
#define BidInformation_INCLUDED

#include <map>
#include <Poco/RefCountedObject.h>
#include <Poco/AutoPtr.h>
#include "BidServiceInformation.h"

namespace ChoiceNet
{
namespace Eco
{

class BidInformation
{

public:
	BidInformation();
	~BidInformation();
	
	void addBidToService(Bid * bidPtr);
	void deleteBidToService(Bid * bidPtr);
	void addService(std::string serviceId);
	bool existService(std::string serviceId);
	std::string getBestBids(std::string serviceIdParam, int fronts);
	void getProviderBids(std::string providerId, std::map<std::string, std::vector<std::string> > &bids);
	bool isBidActive(std::string serviceId, std::string providerId, std::string bidId);

private:

	typedef std::map<std::string, BidServiceInformation *> BidServiceInformationContainer;
	BidServiceInformationContainer _service_information;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // BidInformation_INCLUDED
