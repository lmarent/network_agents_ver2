
#include <Poco/Util/ServerApplication.h>
#include "FoundationException.h"
#include "Provider.h"

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

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering provider setInitialAvailability");


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

	app.logger().debug("Ending provider setInitialAvailability");

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

bool Provider::checkResourceAvailability(unsigned period, 
										 std::string resourceId, 
										 DecisionVariable *variable, 
										 double level,
										 double quantity )
{
	ResourceMap::iterator it;
	it = _resources.find(resourceId);
	// When the resourceId is not found the system, it assumes not capacity
	if (it != _resources.end())
		return (it->second)->checkAvailability(period, variable, level,quantity);
	else 
	    return false;
}

bool Provider::isBulkAvailable(unsigned period, Service * service,  Purchase * purchase, Bid * bid)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering provider isAvailable");
	
	bool available = true;
	
	if (service->hasQualityVariables() == true)
	{	
		std::map<std::string, DecisionVariable *>::iterator it; 
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			DecisionVariable *variable = it->second;
			if (variable->getModeling() == MODEL_QUALITY)
			{
				
				app.logger().debug("Checking resource availability for resource" + variable->getResource());
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
				
				app.logger().debug("Checking resource availability for resource" + variable->getResource());
				available = available && checkResourceAvailability(period, 
																   variable->getResource(), 
																   variable, 
																   bid->getDecisionVariable(variable->getId()),
																   purchase->getQuantity() );
			}
		}
		
	}

	app.logger().debug(Poco::format("Ending provider isAvailable %b", available ));
	return available;
}

ProviderCapacityType Provider::getCapacityType(void)
{
	return _type;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
