
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
		app.logger().debug("get Consumption - Objetive Maximize");

		if ((maxValue - minValue) == 0)
			percentage = percentage + (value - minValue) / maxValue;
		else
			percentage = percentage + (value - minValue) / (maxValue - minValue);

	}

	if (objetive == MINIMIZE)
	{
		app.logger().debug("get Consumption - Objetive Minimize");

		if ((maxValue - minValue) == 0)
			percentage = percentage + (maxValue - value) / minValue;
		else
            percentage = percentage + (maxValue - value) / (maxValue - minValue);
	}

	app.logger().information(Poco::format("Ending Resource - get Consumption percentage:%f", percentage) );

	return percentage;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
