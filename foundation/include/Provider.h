#ifndef Provider_INCLUDED
#define Provider_INCLUDED

#include <string>
#include "Resource.h"
#include "ResourceAvailability.h"
#include "Purchase.h"
#include "Service.h"
#include "Bid.h"



namespace ChoiceNet
{
namespace Eco
{

enum ProviderCapacityType
{
	BID_BY_BID = 0,
	BULK_CAPACITY =  1,
	UNDEFINED_CAPACITY_TYPE = 2,
	MAX_PROVIDER_CAPACITY_TYPE = 3,
};

class Provider
{
public:

    Provider(std::string id, ProviderCapacityType capacity_type);
    /// Constructor provider

    ~Provider();
    /// Destructor for provider's class

    void addResourceAvailability(std::string resourceId);
    /// Adds a new resource for provider's control.

	void setInitialAvailability(Resource *resource, double quantity);
	/// Set the initial availability for the rest of periods.

	void deductAvailability(unsigned period, Service * service, Purchase * purchase, Bid * bid);
	/// Deducts the units of every resource required to do the purchase.
	/// It is assumed that always the user of the function checks the availability
	/// and based on its value calls this function.

	bool isBulkAvailable(unsigned period, Service * service, Purchase * purchase, Bid * bid);
	/// Determines whether a provider is availability for a purchase.

	double getBulkAvailability(unsigned period, Service * service, Bid * bid);
	/// Gets the availability of a provider for a specific service.

	double getUnitaryRequirement(unsigned period, std::string resourceId, DecisionVariable *variable, double level);

	double getResourceAvailability(unsigned period, std::string resourceId);

	ProviderCapacityType getCapacityType(void);

	std::string getId();

protected:

	bool checkResourceAvailability(unsigned period,
								   std::string resourceId,
		   						   DecisionVariable *variable,
								   double level,
								   double quantity );

	void deductResourceAvailability(unsigned period,
		   						    std::string resourceId,
									DecisionVariable *variable,
									double level,
									double quantity );

private:
	std::string _id;
	ProviderCapacityType _type;
	typedef std::map<std::string, ResourceAvailability *>  ResourceMap;

	ResourceMap _resources;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace


#endif // Provider_INCLUDED
