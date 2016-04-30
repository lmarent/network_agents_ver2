
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
	std::cout << "deting reference to resource"<< std::endl;
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
	
double ResourceAvailability::deductAvailability(unsigned period, 
												DecisionVariable *variable, 
											    double level,
											    double quantity)
{
	if ( _resource != NULL )
	{
		double rate =_resource->getConsumption(variable,level);
		double quantityRequired = quantity * rate;
		std::map<unsigned, double>::iterator it;
		it = _time_availability.find(period);
		if (it == _time_availability.end()){
			_time_availability.insert(std::pair<unsigned, double>(period,_init_availability));
		}
		it = _time_availability.find(period);			
		it->second -= quantityRequired;
		std::cout << "Availability:" << it->second << std::endl;
	}
}

bool ResourceAvailability::checkAvailability(unsigned period, 
											 DecisionVariable *variable, 
											 double level,
											 double quantity)
{
	bool val_return;
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering ResourceAvailability - checkAvailability");
	
	if ( _resource != NULL )
	{
		double rate =_resource->getConsumption(variable,level);
		double quantityRequired = quantity * rate;
		double available = getAvailability(period);
		if (available >= quantityRequired) 
		{   
			val_return = true;
			std::cout << "in Check availability 1" << std::endl;
		}
		else
		{
			std::cout << "in Check availability 2" << "available:" << available << "quantityRequired" << quantityRequired << std::endl;
			val_return= false;
		}
	}
	else 
	{   
		std::cout << "in Check availability 3" << std::endl;
		val_return = false;
	}
	app.logger().debug("Ending ResourceAvailability - checkAvailability" + val_return);
	return val_return;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace

