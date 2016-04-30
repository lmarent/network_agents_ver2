#include <map>
#include <string>

#include "DemandForecaster.h"
#include "FoundationException.h"

namespace ChoiceNet
{

namespace Eco
{

DemandForecaster::DemandForecaster()
{

}
	
DemandForecaster::~DemandForecaster()
{
	_actual_demand.clear();
}

void DemandForecaster::setActualDemand(unsigned period, double value)
{

	std::map<unsigned, double>::iterator it;
	it = _actual_demand.find(period);
	if(it == _actual_demand.end()) {
		_actual_demand.insert ( std::pair<unsigned, double>
									   (period,value) );
	}
	else
	{
		throw FoundationException("Period is already included", 328);
	}

}
	
double DemandForecaster::getActualDemand(unsigned period)
{

	std::map<unsigned, double>::iterator it;
	it = _actual_demand.find(period);
	if(it != _actual_demand.end()) {
		return it->second;
	}
	else
	{
		throw FoundationException("The period given is not registered" , 329);
	}

}


}   /// End Eco namespace

}  /// End ChoiceNet namespace
