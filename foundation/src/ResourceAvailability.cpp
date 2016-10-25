
#include <string>
#include <Poco/Util/ServerApplication.h>
#include "ResourceAvailability.h"

namespace ChoiceNet
{
namespace Eco
{

ResourceAvailability::ResourceAvailability(std::string id):
_id(id),
_init_availability(0),
_resource(NULL)
{
}

ResourceAvailability::~ResourceAvailability()
{
	_resource = NULL;
}

std::string ResourceAvailability::getId()
{
	return _id;
}

void ResourceAvailability::setResource(Resource *resourcePtr)
{
	_resource = resourcePtr;
}

void ResourceAvailability::setInitialAvailability(double quantity)
{
	_init_availability = quantity;
}

double ResourceAvailability::getAvailability(unsigned period)
{
    std::map<unsigned, double>::iterator it;
	it = _time_availability.find(period);
	if (it != _time_availability.end())
	{
		return it->second;
	}
	else
	{
		return _init_availability;
	}
}

double ResourceAvailability::getUnitaryRequirement(unsigned period,
												   DecisionVariable *variable,
										    	   double level)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering getUnitaryRequirement");

	double quantityRequired = 0;

	if ( _resource != NULL )
	{
		double rateAdj = 0;
		double rate =_resource->getConsumption(variable,level);
		CostFunction *cost_function = variable->getCostFunction();
		if (cost_function != NULL)
			rateAdj = cost_function->getEvaluation(rate);
		else
			rateAdj = rate;

		quantityRequired = rateAdj;

	}

	app.logger().debug(Poco::format("Ending getUnitaryRequirement %f", quantityRequired) );

	return quantityRequired;
}

double ResourceAvailability::deductAvailability(unsigned period,
												DecisionVariable *variable,
											    double level,
											    double quantity)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information(Poco::format("Entering deductAvailability level:%f quantity:%f", level, quantity) );

	double quantityRequired = 0;
	double endAvailability = 0;

	if ( _resource != NULL )
	{
		double rateAdj = 0;
		double rate =_resource->getConsumption(variable,level);
		CostFunction *cost_function = variable->getCostFunction();
		if (cost_function != NULL)
			rateAdj = cost_function->getEvaluation(rate);
		else
			rateAdj = rate;

		quantityRequired = quantity * rateAdj;
		std::map<unsigned, double>::iterator it;
		it = _time_availability.find(period);
		if (it == _time_availability.end()){
			_time_availability.insert(std::pair<unsigned, double>(period,_init_availability));
		}
		it = _time_availability.find(period);
		it->second -= quantityRequired;
		endAvailability = it->second;

		app.logger().information(Poco::format("Ending deductAvailability - final availability: %f", it->second) );

	}

	return endAvailability;
}

bool ResourceAvailability::checkAvailability(unsigned period,
											 DecisionVariable *variable,
											 double level,
											 double quantity)
{
	bool val_return;

#ifndef TEST_ENABLED
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering ResourceAvailability - checkAvailability");
#endif

	if ( _resource != NULL )
	{

		double rateAdj = 0;
		double rate =_resource->getConsumption(variable,level);
		CostFunction *cost_function = variable->getCostFunction();
		if (cost_function != NULL){
#ifndef TEST_ENABLED
			app.logger().debug(Poco::format("cost_function called %s", cost_function->getName() ));
#endif

			rateAdj = cost_function->getEvaluation(rate);
		}
		else{
			rateAdj = rate;
		}

		double quantityRequired = quantity * rateAdj;
		double available = getAvailability(period);

#ifndef TEST_ENABLED
		app.logger().debug(Poco::format("checkAvailability rate:%f rateAdj:%f initQty:%f period:%d qtyAvailable:%f qtyRequired:%f",rate, rateAdj, quantity, (int) period, available, quantityRequired));
#endif

		if (available >= quantityRequired)
		{
			val_return = true;
		}
		else
		{
			val_return= false;
		}
	}
	else
	{
#ifndef TEST_ENABLED
		app.logger().debug("checkAvailability - undefined resource");
#endif

		val_return = false;
	}

#ifndef TEST_ENABLED
	app.logger().debug("Ending ResourceAvailability - checkAvailability" + val_return);
#endif

	return val_return;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace

