#ifndef Cost_Function_INCLUDED
#define Cost_Function_INCLUDED

#include <string>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>

#include "CostFunction.h"


namespace ChoiceNet
{
namespace Eco
{

enum Range
{
	RANGE_CONTINUOUS = 1,
	RANGE_DISCRETE = 2,
	RANGE_UNDEFINED = 3
};

class CostFunction
{

public:
	
	CostFunction(std::string id, std::string class_name, Range range);
	
	~CostFunction();
	
	std::string getId();
	
	std::string getClassName();
	
	Range getRange();
	
	std::string getRangeStr();
	
	void addParameter(std::string name, double value);
		
	void to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
					Poco::XML::AutoPtr<Poco::XML::Element> pParent);
	
private: 
	std::string _id;
	std::string _class_name;
	Range _range;
	std::map<std::string, double> _parameters;
	
};	

}  /// End Eco namespace

}  /// End ChoiceNet namespace


#endif // Cost_Function_INCLUDED
