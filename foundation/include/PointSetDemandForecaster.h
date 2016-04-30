#ifndef PointSetDemandForecaster_INCLUDED
#define PointSetDemandForecaster_INCLUDED

#include <map>
#include <string>
#include "DemandForecaster.h"

namespace ChoiceNet
{

namespace Eco
{

class PointSetDemandForecaster: public DemandForecaster
{

public:

    PointSetDemandForecaster(std::map<std::string, std::string> parameters );
    
    ~PointSetDemandForecaster();

	double getForecast(unsigned period);
	
	
private:    
    std::map <unsigned, double> _forecast;
    
};

}   /// End Eco namespace

}  /// End ChoiceNet namespace    

#endif   // PointSetDemandForecaster_INCLUDED
