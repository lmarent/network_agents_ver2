#ifndef ResourceAvailability_INCLUDED
#define ResourceAvailability_INCLUDED

#include <map>
#include <iostream>
#include "Resource.h"
#include "DecisionVariable.h"

namespace ChoiceNet
{
namespace Eco
{

class ResourceAvailability
{

public:

	ResourceAvailability(std::string id);

	~ResourceAvailability();

	std::string getId();

	void setResource(Resource * resourcePtr);
	/// Sets the resource behind this availability resource object.

	void setInitialAvailability(double quantity);
	/// Sets the initial availability for the rest of periods.

	double getAvailability(unsigned period);
	/// Gets the availability for a specified period.

	double deductAvailability(unsigned period, DecisionVariable *variable,
							  double level, double quantity);
	/// returns the number of units deducted from the availability.

	bool checkAvailability(unsigned period, DecisionVariable *variable,
						   double level, double quantity);
	/// Verifies whether the resource has enough capacity to operate or not.

	double getUnitaryRequirement(unsigned period, DecisionVariable *variable, double level);
	/// Gets the unitary resource requirement in order to be able to fulfil a service with the level specified.

private:

	std::string _id;
	Resource * _resource;
	double _init_availability;
	std::map<unsigned, double> _time_availability;
};


}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // ResourceAvailability_INCLUDED
