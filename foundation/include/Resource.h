#ifndef Resource_INCLUDED
#define Resource_INCLUDED

#include "DecisionVariable.h"
#include <Poco/RefCountedObject.h>

namespace ChoiceNet
{
namespace Eco
{

class Resource : public Poco::RefCountedObject
{
	
	friend class ResourceAvailability;
	
public:
	
	Resource(std::string id);
	
	~Resource();
	
	std::string getId();
	
	void setName(std::string name);
	
	std::string getName();
	
	double getConsumption(DecisionVariable *qualityParameter, double value);

private:
	std::string _id;
	std::string _name;
	
};


}  /// End Eco namespace

}  /// End ChoiceNet namespace


#endif // Resource_INCLUDED
