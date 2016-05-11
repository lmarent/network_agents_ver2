#include <iostream>
#include "Datapoint.h"
#include "FoundationException.h"
#include <string>
#include <Poco/Util/ServerApplication.h>
#include <Poco/NumberFormatter.h>


namespace ChoiceNet
{
namespace Eco
{

Datapoint::Datapoint(std::string id): 
_id(id), 
paretoStatus(-1), 
dominatedCount(0) 
{

}

void Datapoint::addNumber(double number)
{
  vec.push_back(number);
}

void Datapoint::setNumberAtDim(size_t d, double number)
{
	if (d >= vec.size()){
		throw FoundationException("Invalid dimension for Bid",309);
	}
	else
	{
		vec[d] = number;
	}
}

void Datapoint::print() const 
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	std::string paretoStr = Poco::NumberFormatter::format(paretoStatus);	
	app.logger().debug("Entering printParetoFrontier" + paretoStr);
	
	for (size_t k=0; k<vec.size();++k){
		std::cout << vec[k] << " ";
	}
	std::cout << std::endl;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
