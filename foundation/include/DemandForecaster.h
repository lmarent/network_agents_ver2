#ifndef Demand_Forecaster_INCLUDED
#define Demand_Forecaster_INCLUDED

#include <map>

namespace ChoiceNet
{

namespace Eco
{

class DemandForecaster
{

public:	
	
	DemandForecaster();
	
	~DemandForecaster();
	
	void setActualDemand(unsigned period, double value);
	
	double getActualDemand(unsigned period);
	
	double getForecast(unsigned period){return 0;};

private:
	
	std::map<unsigned, double> _actual_demand;
};

}   /// End Eco namespace

}  /// End ChoiceNet namespace

#endif   // Demand_Forecaster_INCLUDED
