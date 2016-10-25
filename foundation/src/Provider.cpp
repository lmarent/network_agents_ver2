
#include <Poco/Util/ServerApplication.h>
#include "FoundationException.h"
#include "Provider.h"
#include <limits>

namespace ChoiceNet
{
namespace Eco
{

Provider::Provider(std::string id, ProviderCapacityType capacity_type):
_id(id), _type(capacity_type)
{
}

Provider::~Provider()
{
	std::map<std::string, ResourceAvailability *>::iterator it;
	it = _resources.begin();
	while (it != _resources.end())
	{
		delete it->second;
		_resources.erase(it);
		++it;
	}
}

void Provider::addResourceAvailability(std::string resourceId)
{
    std::map<std::string, ResourceAvailability *>::iterator it;
	it = _resources.find(resourceId);
	if (it == _resources.end())
	{
		ResourceAvailability * resourcePtr = new ResourceAvailability(resourceId);
		_resources.insert(std::pair<std::string, ResourceAvailability *>(resourceId,resourcePtr));
	}
	else
	{
		throw FoundationException("Resource already registered in the provider", 313);
	}
}

void Provider::setInitialAvailability(Resource *resource, double quantity)
{

#ifndef TEST_ENABLED
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering provider setInitialAvailability");
#endif


    std::map<std::string, ResourceAvailability *>::iterator it;
	it = _resources.find(resource->getId());
	if (it != _resources.end())
	{
		(*(it->second)).setInitialAvailability(quantity);
	}
	else
	{
		addResourceAvailability(resource->getId());
		it = _resources.find(resource->getId());
		(*(it->second)).setResource(resource);
		(*(it->second)).setInitialAvailability(quantity);
	}

#ifndef TEST_ENABLED
	app.logger().debug("Ending provider setInitialAvailability");
#endif

}

void Provider::deductResourceAvailability(unsigned period,
										 std::string resourceId,
										 DecisionVariable *variable,
										 double level,
										 double quantity )
{
	ResourceMap::iterator it;
	it = _resources.find(resourceId);
	// when the resourceId is not found the system, it assumes unlimited capacity
	if (it != _resources.end())
		(*(it->second)).deductAvailability(period, variable, level,quantity);
}

void Provider::deductAvailability(unsigned period,
									Service * service,
									Purchase * purchase,
									Bid * bid)
{
	std::map<std::string, DecisionVariable *>::iterator it;
	for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
	{
		DecisionVariable *variable = it->second;
		if (variable->getModeling() == MODEL_QUALITY)
		{

			deductResourceAvailability(period,
									   variable->getResource(),
									   variable,
									   bid->getDecisionVariable(variable->getId()),
									   purchase->getQuantity() );
		}
	}
}

double Provider::getResourceAvailability(unsigned period,
										 std::string resourceId)
{

	ResourceMap::iterator it;
	it = _resources.find(resourceId);

	// When the resourceId is not found the system, it assumes not capacity
	if (it != _resources.end()){
		return (it->second)->getAvailability(period);
	}
	else{
	    return false;
	}
}

double Provider::getUnitaryRequirement(unsigned period,
									   std::string resourceId,
									   DecisionVariable *variable,
									   double level)
{

	ResourceMap::iterator it;
	it = _resources.find(resourceId);

	// When the resourceId is not found the system, it assumes not capacity
	if (it != _resources.end()){
		return (it->second)->getUnitaryRequirement(period, variable, level );
	}
	else{
	    return -1; // An the resource was not found.
	}
}


bool Provider::checkResourceAvailability(unsigned period,
										 std::string resourceId,
										 DecisionVariable *variable,
										 double level,
										 double quantity )
{

	ResourceMap::iterator it;
	it = _resources.find(resourceId);

	// When the resourceId is not found the system, it assumes not capacity
	if (it != _resources.end()){
		return (it->second)->checkAvailability(period, variable, level,quantity);
	}
	else{
	    return false;
	}
}

bool Provider::isBulkAvailable(unsigned period, Service * service,  Purchase * purchase, Bid * bid)
{
#ifndef TEST_ENABLED
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering provider isAvailable");
#endif

	bool available = true;

	if (service->hasQualityVariables() == true)
	{
		std::map<std::string, DecisionVariable *>::iterator it;
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			DecisionVariable *variable = it->second;
			if (variable->getModeling() == MODEL_QUALITY)
			{


#ifndef TEST_ENABLED
				app.logger().debug("Checking resource availability for resource" + variable->getResource());
#endif

				available = available && checkResourceAvailability(period,
																   variable->getResource(),
																   variable,
																   bid->getDecisionVariable(variable->getId()),
																   purchase->getQuantity() );
			}
		}
	}
	else {

		std::map<std::string, DecisionVariable *>::iterator it;
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			DecisionVariable *variable = it->second;
			if (variable->getModeling() == MODEL_PRICE )
			{

#ifndef TEST_ENABLED
				app.logger().debug("Checking resource availability for resource" + variable->getResource());
#endif

				available = available && checkResourceAvailability(period,
																   variable->getResource(),
																   variable,
																   bid->getDecisionVariable(variable->getId()),
																   purchase->getQuantity() );
			}
		}

	}

#ifndef TEST_ENABLED
	app.logger().debug(Poco::format("Ending provider isAvailable %b", available ));
#endif

	return available;
}


double Provider::getBulkAvailability(unsigned period, Service * service, Bid * bid)
{
#ifndef TEST_ENABLED
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering provider getBulkAvailability");
#endif

	double availabilityResource = 0;
	bool enter = false;
	double availabilityTmp;
	double availability;
	double requirement;

	if (service->hasQualityVariables() == true)
	{
		std::map<std::string, DecisionVariable *>::iterator it;
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			DecisionVariable *variable = it->second;
			if (variable->getModeling() == MODEL_QUALITY)
			{

				app.logger().debug("Getting resource availability - Resource:" + variable->getResource());
				requirement = getUnitaryRequirement(period,variable->getResource(), variable, bid->getDecisionVariable(variable->getId()) );
				if (requirement < 0){
					app.logger().error("The resource was not found, which means it has no capacity");
					return 0; // The resource was not found, which means it has no capacity.
				}
				else{
					if (enter == false)
					{
						availabilityResource = getResourceAvailability(period,variable->getResource() );
						if (requirement > 0) {
							availability = availabilityResource / requirement;
							enter = true;
						}
						else{
							// It puts the maximum availability
							availability = std::numeric_limits<double>::max();
							enter = true;
						}
					}
					else
					{
						// The assumption here is that every resource can be compared in terms of capacity.
						availabilityResource = getResourceAvailability(period,variable->getResource() );
						if (requirement > 0) {
							availabilityTmp = availabilityResource / requirement;
							if (availabilityTmp <availability){
								availability = availabilityTmp;
							}
						}
						else {
							availabilityTmp = std::numeric_limits<double>::max();
							if (availabilityTmp <availability){
								availability = availabilityTmp;
							}
						}
					}
				}

				app.logger().information(Poco::format("resource: %s - Requirement:%f - Availability:%f", variable->getResource(), requirement, availability) );

			}
		}
	}
	else {

		std::map<std::string, DecisionVariable *>::iterator it;
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			DecisionVariable *variable = it->second;
			if (variable->getModeling() == MODEL_PRICE )
			{

#ifndef TEST_ENABLED
				app.logger().debug("Getting resource availability - Resource:" + variable->getResource());
#endif
				requirement = getUnitaryRequirement(period,variable->getResource(), variable, bid->getDecisionVariable(variable->getId()) );
				if (enter == false)
				{
					availabilityResource = getResourceAvailability(period,variable->getResource() );
					availability = availabilityResource / requirement;
					enter = true;
				}
				else
				{
					// The assumption here is that every resource can be compared in terms of capacity.
					availabilityResource = getResourceAvailability(period,variable->getResource() );
					availabilityTmp = availabilityResource / requirement;
					if (availabilityTmp < availability){
						availability = availabilityTmp;
					}
				}

				app.logger().information(Poco::format("resource: %s - Requirement:%f - Availability:%f", variable->getResource(), requirement, availability) );
			}
		}

	}

#ifndef TEST_ENABLED
	app.logger().debug(Poco::format("Ending provider getBulkAvailability %d", availability ));
#endif

	return availability;
}


ProviderCapacityType Provider::getCapacityType(void)
{
	return _type;
}

std::string Provider::getId()
{
	return _id;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
