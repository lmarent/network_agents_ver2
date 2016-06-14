#include <map>
#include <iostream>
#include <fstream>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>

#include "CostFunction.h"
#include "FoundationException.h"

namespace ChoiceNet
{
namespace Eco
{

CostFunction::CostFunction(std::string id, std::string class_name, Range range):
_id(id),
_class_name(class_name),
_range(range)
{

}
	
CostFunction::~CostFunction()
{
	
}

std::string CostFunction::getId()
{
	return _id;
}

std::string CostFunction::getClassName()
{
	return _class_name;
}
	
Range CostFunction::getRange()
{
	return _range;
}

std::string CostFunction::getRangeStr()
{
	std::string result;
	switch (_range)
	{
		case RANGE_CONTINUOUS:
			result = "continuous";
			break;
		
		case RANGE_DISCRETE:
			result = "discrete";
			break;
		
		case RANGE_UNDEFINED:
			result = "undefined";
			break;
	}
	return result;
}

void CostFunction::addParameter(std::string name, double value)
{
	std::map<std::string, double>::iterator it;
	it = _parameters.find(name);
	if(it == _parameters.end()) {
		_parameters.insert ( std::pair<std::string, double>  (name,value) );
	}
	else
	{
		throw FoundationException("Cost Function Parameter is already included");
	}
}
		
void CostFunction::to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
					Poco::XML::AutoPtr<Poco::XML::Element> pParent)
{
	// std::cout << "putting the information of the Cost Function:" << getClassName() <<  std::endl;

	Poco::XML::AutoPtr<Poco::XML::Element> proot = 
											pDoc->createElement("Cost_Function");

	Poco::XML::AutoPtr<Poco::XML::Element> pId = 
							pDoc->createElement("Name");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(getId());
	pId->appendChild(pText1);		
	proot->appendChild(pId);

	Poco::XML::AutoPtr<Poco::XML::Element> pName = 
							pDoc->createElement("Range");
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = pDoc->createTextNode(getRangeStr());
	pName->appendChild(pText2);		
	proot->appendChild(pName);
	
	// Sets the parameters.
	std::map<std::string, double >::iterator it;
	for ( it= _parameters.begin(); it != _parameters.end(); ++it)
	{
		Poco::XML::AutoPtr<Poco::XML::Element> pParameter = 
							pDoc->createElement("Parameter");
							
		Poco::XML::AutoPtr<Poco::XML::Element> pNamePar = 
							pDoc->createElement("Name");
	    Poco::XML::AutoPtr<Poco::XML::Text> pText3 = pDoc->createTextNode(it->first);							
	    pNamePar->appendChild(pText3);	

		Poco::XML::AutoPtr<Poco::XML::Element> pValuePar = 
							pDoc->createElement("Value");
		double value = it->second;
		std::string valueStr;
	    Poco::NumberFormatter::append(valueStr, value);
 	    Poco::XML::AutoPtr<Poco::XML::Text> pText4 = pDoc->createTextNode(valueStr);
 	    pValuePar->appendChild(pText4);	
 	    
 	    pParameter->appendChild(pNamePar);
 	    pParameter->appendChild(pValuePar);
 	    proot->appendChild(pParameter);
	}
	

	pParent->appendChild(proot);

}
	
}  /// End Eco namespace

}  /// End ChoiceNet namespace
