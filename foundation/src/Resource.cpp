
#include <Poco/Util/ServerApplication.h>
#include <Poco/NumberFormatter.h>
#include "DecisionVariable.h"
#include "Resource.h"

namespace ChoiceNet
{
namespace Eco
{


Resource::Resource(std::string id):
_id(id)
{

}
	
Resource::~Resource()
{

}

std::string Resource::getId()
{
	return _id;
}

std::string Resource::getName()
{
	return _name;
}

void Resource::setName(std::string name)
{
	_name = name;
}
	
double Resource::getConsumption(DecisionVariable *qualityParameter, double value)
{
	
#ifndef TEST_ENABLED	
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug("Entering Resource - get Consumption");
#endif
	
	double percentage = 0;
	double minValue = qualityParameter->getMinValue();
	double maxValue = qualityParameter->getMaxValue();
	
	OptimizationObjective objetive = qualityParameter->getObjetive();
	if (objetive == MAXIME)
	{
		if (minValue == 0)
			percentage = percentage + (value - minValue) / maxValue;
		else
			percentage = percentage + (value - minValue) / minValue;
			
	}
	
	if (objetive == MINIMIZE)
	{
		if (maxValue == 0)
			percentage = percentage + (maxValue - value) / minValue;
		else
            percentage = percentage + (maxValue - value) / maxValue;
	}
	
#ifndef TEST_ENABLED
	std::string percentageStr;
	Poco::NumberFormatter::append(percentageStr, percentage);
	app.logger().debug("Ending Resource - get Consumption" + percentageStr);
#endif
	
	return percentage;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
