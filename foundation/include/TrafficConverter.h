#ifndef TrafficConverter_INCLUDED
#define TrafficConverter_INCLUDED

#include <map>
#include <string>

namespace ChoiceNet
{

namespace Eco
{

class TrafficConverter
{
public:

	TrafficConverter(){};
	
	~TrafficConverter(){};
	
	void setTrafficSampleConfiguration(std::map<std::string, double> &){};
	
	unsigned getRequiredAgents(double totalDemand){};

};

}   /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // Traffic_Converter_INCLUDED
