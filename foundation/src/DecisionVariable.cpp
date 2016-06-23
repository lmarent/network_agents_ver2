#include <string>
#include "DecisionVariable.h"
#include "FoundationException.h"
#include "ProbabilityDistribution.h"
#include <Poco/NumberFormatter.h>

namespace ChoiceNet
{
namespace Eco
{
	
DecisionVariable::DecisionVariable(std::string id):
_id(id),
_opt_objetive(OO_UNDEFINED),
_is_modeling(MODEL_UNDEFINED),
_cost_function(NULL)
{
}
    
DecisionVariable::~DecisionVariable()
{
		
}

std::string DecisionVariable::getId()
{
	return _id;
}

void DecisionVariable::setName(std::string name)
{
	_name = name; 
}
    
std::string DecisionVariable::getName(void)
{
	return _name;
}
    
void DecisionVariable::setProbabilityDistribution( Purpose purpose, 
								ProbabilityDistribution * probability_distribution)
{
	std::map<std::string, ProbabilityDistribution *>::iterator it;
	it = _probability_distributions.find(getPurposeStr(purpose));
	if(it == _probability_distributions.end()) {
		_probability_distributions.insert ( std::pair<std::string, ProbabilityDistribution *>
									   (getPurposeStr(purpose),probability_distribution) );
	}
	else
	{
		throw FoundationException("The probability distribution for the given purpose has been already included");
	}
	
}	

void DecisionVariable::setCostFunction(CostFunction * cost_function)
{
	
	if(_cost_function == NULL) {
		_cost_function = cost_function;
	}
	else {
		throw FoundationException("The Cost Function has been already assigned");
	}
	
}	

    
void DecisionVariable::setObjetive(OptimizationObjective objetive)
{
	_opt_objetive = objetive;
}

ProbabilityDistribution * DecisionVariable::getProbabilityDistribution(Purpose purpose)
{
	std::map<std::string, ProbabilityDistribution *>::iterator it;
	it = _probability_distributions.find(getPurposeStr(purpose));
	if(it != _probability_distributions.end()) {
		return it->second;
	}
	else
	{
		throw FoundationException("The probability distribution for the given purpose is not part of the Quality Parameter");
	}
	
}

CostFunction * DecisionVariable::getCostFunction()
{
	return _cost_function;
}

void DecisionVariable::setModelling(Modeling modeling)
{
	_is_modeling = modeling;
}

Modeling DecisionVariable::getModeling(void)
{
	return _is_modeling;
}
    
std::string DecisionVariable::getModelingStr(void)
{
	std::string result;
    switch (_is_modeling)
    {
       case MODEL_PRICE:
          result = "price";
          break;
	   case MODEL_QUALITY:
          result = "quality";
          break;
	   case MODEL_UNDEFINED:
          result = "undefined";
          break;
    }
    return result;
}
       
OptimizationObjective DecisionVariable::getObjetive(void)
{
	return _opt_objetive;
}

std::string DecisionVariable::getObjectiveStr()
{
	std::string result;
    switch (_opt_objetive)
    {
       case MAXIME:
          result = "maximize";
          break;
	   case MINIMIZE:
          result = "minimize";
          break;
	   case OO_UNDEFINED:
          result = "undefined";
          break;
    }
    return result;
}

std::string DecisionVariable::getPurposeStr(Purpose purpose)
{
	std::string result;
    switch (purpose)
    {
       case VALUE:
          result = "value";
          break;
	   case SENSITIVITY:
          result = "sensitivity";
          break;
	   case P_UNDEFINED:
          result = "undefined";
          break;
    }
    return result;

}

void DecisionVariable::setRange(double min_value, double max_value)
{
	_min_value = min_value;
	_max_value = max_value;
}
    
double DecisionVariable::getMinValue(void)
{
	return _min_value;
}

std::string DecisionVariable::getMinValueStr(void)
{
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, _min_value);
	return valueStr;
}
    
double DecisionVariable::getMaxValue(void)
{
	return _max_value;
}

std::string DecisionVariable::getMaxValueStr(void)
{
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, _max_value);
	return valueStr;
}

void DecisionVariable::setResource(std::string resourceId)
{
	_resourceId = resourceId;
}

std::string DecisionVariable::getResource()
{
	return _resourceId;
}

void DecisionVariable::to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
				   Poco::XML::AutoPtr<Poco::XML::Element> pParent)
{
	Poco::XML::AutoPtr<Poco::XML::Element> proot = 
											pDoc->createElement("Decision_Variable");
	Poco::XML::AutoPtr<Poco::XML::Element> pId = pDoc->createElement("Id");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(getId());
	pId->appendChild(pText1);

	Poco::XML::AutoPtr<Poco::XML::Element> pName = pDoc->createElement("Name");
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = pDoc->createTextNode(getName());
	pName->appendChild(pText2);


	Poco::XML::AutoPtr<Poco::XML::Element> pObjective = pDoc->createElement("Objective");
	Poco::XML::AutoPtr<Poco::XML::Text> pText3 = pDoc->createTextNode(getObjectiveStr());
	pObjective->appendChild(pText3);
	
	Poco::XML::AutoPtr<Poco::XML::Element> pModeling = pDoc->createElement("Modeling");
	Poco::XML::AutoPtr<Poco::XML::Text> pText6 = pDoc->createTextNode(getModelingStr());
	pModeling->appendChild(pText6);

	Poco::XML::AutoPtr<Poco::XML::Element> pMinValue = pDoc->createElement("Min_Value");
	Poco::XML::AutoPtr<Poco::XML::Text> pText7 = pDoc->createTextNode(getMinValueStr());
	pMinValue->appendChild(pText7);

	Poco::XML::AutoPtr<Poco::XML::Element> pMaxValue = pDoc->createElement("Max_Value");
	Poco::XML::AutoPtr<Poco::XML::Text> pText8 = pDoc->createTextNode(getMaxValueStr());
	pMaxValue->appendChild(pText8);

	Poco::XML::AutoPtr<Poco::XML::Element> pResource = pDoc->createElement("Resource");
	Poco::XML::AutoPtr<Poco::XML::Text> pText9 = pDoc->createTextNode(getResource());
	pResource->appendChild(pText9);
	

    proot->appendChild(pId);
    proot->appendChild(pName);
	proot->appendChild(pObjective);
	proot->appendChild(pModeling);
	proot->appendChild(pMinValue);
	proot->appendChild(pMaxValue);
	proot->appendChild(pResource);
	
	std::map<std::string, ProbabilityDistribution *>::iterator it;
	for (it = _probability_distributions.begin(); it != _probability_distributions.end(); ++it)
	{
		Poco::XML::AutoPtr<Poco::XML::Element> pProbDist = pDoc->createElement("Pr_Distributions");
		
		// Define The purpose
		Poco::XML::AutoPtr<Poco::XML::Element> pPurpose = pDoc->createElement("Purpose");		
		Poco::XML::AutoPtr<Poco::XML::Text> pText4 = pDoc->createTextNode(it->first);
		pPurpose->appendChild(pText4);
		pProbDist->appendChild(pPurpose);
		
		// Define the class Name
		Poco::XML::AutoPtr<Poco::XML::Element> pClassName = 
							pDoc->createElement("Class_Name");
		Poco::XML::AutoPtr<Poco::XML::Text> pText5 = pDoc->createTextNode((it->second)->getClassName());
		pClassName->appendChild(pText5);		
		pProbDist->appendChild(pClassName);
		
		
		(it->second)->to_XML(pDoc,pProbDist);
	    proot->appendChild(pProbDist);
	}  
	
	// Add the cost function.
	if (_cost_function != NULL){
		Poco::XML::AutoPtr<Poco::XML::Element> cFunc = pDoc->createElement("Cst_Function");

		// Define the class Name
		Poco::XML::AutoPtr<Poco::XML::Element> cClassName = 
							pDoc->createElement("Class_Name");
		Poco::XML::AutoPtr<Poco::XML::Text> pText7 = pDoc->createTextNode((_cost_function)->getClassName());
		cClassName->appendChild(pText7);		
		cFunc->appendChild(cClassName);
		(_cost_function)->to_XML(pDoc, cFunc);
		proot->appendChild(cFunc);
	}
	else {
		Poco::XML::AutoPtr<Poco::XML::Element> cFunc = pDoc->createElement("Cst_Function");
		proot->appendChild(cFunc);	
	}
	
	pParent->appendChild(proot);
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
