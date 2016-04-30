#ifndef SimplestTrafficConverter_INCLUDED
#define SimplestTrafficConverter_INCLUDED

#include <map>
#include <string>
#include "TrafficConverter.h"

namespace ChoiceNet
{
namespace Eco
{

class SimplestTrafficConverter: public TrafficConverter
{


public:
	SimplestTrafficConverter();
	
	~SimplestTrafficConverter();
	
	void readFromFile(std::string absolute_path, 
					  std::string file_name,
					  std::string serviceId);
	
	void setTrafficSampleConfiguration(double average,
									   double variance);
	
	unsigned getRequiredAgents(double total_demand);

private:


	double _average;
	double _variance;
	
};
	

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // Simplest_Traffic_Converter
