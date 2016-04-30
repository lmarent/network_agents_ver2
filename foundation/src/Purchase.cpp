#include <string>
#include <Poco/NumberParser.h>
#include <iostream>
#include <Poco/NumberFormatter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>

#include "Purchase.h"
#include "Message.h"
#include "Datapoint.h"
#include "FoundationException.h"
#include "MarketPlaceException.h"


namespace ChoiceNet
{
namespace Eco
{

Purchase::Purchase(std::string idParam,
		 std::string bidParam,
		 std::string serviceParam,
		 double quantityParam,
		 size_t numberDecisionVariables):
Datapoint(idParam),
_bid(bidParam),
_service(serviceParam),
_quantity(quantityParam)
{
	// Initializes all dimensions for decision variables in 0.
	for (size_t i=0; i< numberDecisionVariables; ++i)
	{
		Datapoint::addNumber(0);
	}
}

Purchase::Purchase(Service * service, Message & message):
Datapoint() 
{
	// Obtains the values required to create a Purchase from the message 
	try
	{
		// Initializes all dimensions for decision variables in 0.
		for (size_t i=0; i< (service->_decision_variables).size(); ++i)
		{
			Datapoint::addNumber(0);
		}		
		
		double quantity = 0;
		_id = message.getParameter("Id");
		_bid = message.getParameter("Bid");
		_service = message.getParameter("Service");
		std::string quantityStr = message.getParameter("Quantity");
		quantity = Poco::NumberParser::parseFloat(quantityStr);
		_quantity = quantity;

		std::map<std::string, DecisionVariable *>::iterator it; 
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			std::string decision_variable = it->first;
			size_t dimension = service->getDecisionVariableDimension(decision_variable);
			std::string valueStr =  message.getParameter(decision_variable);
			double value = Poco::NumberParser::parseFloat(valueStr);
			// std::cout << "reading decision variable: " << decision_variable << "value:" << value << std::endl;
			Datapoint::setNumberAtDim(dimension, value);
			// Insert in the mapping between the names of decision variables and their dimensions.
			_decision_variables.insert( std::pair<std::string, size_t>(decision_variable,dimension));
		}
	}
	catch (FoundationException &e){
		// Some of the parameters where not found, so the Purchase is incomplete
		throw MarketPlaceException("Incomplete Purchase - " + e.message(), 306);
	}
}
		
Purchase::~Purchase()
{
	std::cout << "Deleting the Purchase" << std::endl;
}

std::string Purchase::getId()
{
	return _id;
}
	
std::string Purchase::getService()
{
	return _service;
}

std::string Purchase::getBid()
{
	return _bid;
}

void Purchase::setDecisionVariable(std::string decisionVariableId, size_t dimension, double value)
{
	std::map<std::string, size_t>::iterator it;
	it = _decision_variables.find(decisionVariableId);
	if(it == _decision_variables.end()) {
		// Insert in the mapping between the names of decision variables and their dimensions.
		_decision_variables.insert( std::pair<std::string, size_t>(decisionVariableId,dimension));
		Datapoint::setNumberAtDim(dimension, value);
	}
	else{
		throw MarketPlaceException("The decision variable is already in the Purchase");
	}
}


double Purchase::getDecisionVariable(std::string decisionVariableId)
{
	// std::cout << "Entering getDecisionVariable -" << decisionVariableId << std::endl;
	std::map<std::string, size_t>::iterator it;
	it = _decision_variables.find(decisionVariableId);
	if(it != _decision_variables.end()) {
		size_t dimension = it->second;
		double val = Datapoint::getNumberAtDim(dimension);
		// std::cout << "Ending getDecisionVariable -" << val << std::endl;
		return val;
	}
	else
	{
		throw MarketPlaceException("The decision variable is not part of the bid");
	}
}

std::string Purchase::getDecisionVariableStr(std::string decisionVariableId)
{
	double value =  getDecisionVariable(decisionVariableId);
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, value);
	return valueStr;
}

double Purchase::getQuantity(void)
{
	return _quantity;
}

std::string Purchase::getQuantityStr(void)
{
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, _quantity);
	return valueStr;
}


void Purchase::to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
				 Poco::XML::AutoPtr<Poco::XML::Element> pParent)
{
	// std::cout << "putting the information of the Purchase:" << getId() <<  std::endl;

	Poco::XML::AutoPtr<Poco::XML::Element> proot = 
											pDoc->createElement("Purchase");

	Poco::XML::AutoPtr<Poco::XML::Element> pId = 
							pDoc->createElement("Id");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(getId());
	pId->appendChild(pText1);
	
	Poco::XML::AutoPtr<Poco::XML::Element> pBid = 
							pDoc->createElement("Bid");
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = 
							pDoc->createTextNode(getBid());
	pBid->appendChild(pText2);							

	Poco::XML::AutoPtr<Poco::XML::Element> pService = 
											pDoc->createElement("Service");
	Poco::XML::AutoPtr<Poco::XML::Text> pText3 = 
							pDoc->createTextNode(getService());
	pService->appendChild(pText3);									

	Poco::XML::AutoPtr<Poco::XML::Element> pQuantity = 
											pDoc->createElement("Quantity");
	Poco::XML::AutoPtr<Poco::XML::Text> pText6 = 
							pDoc->createTextNode(getQuantityStr());
	pQuantity->appendChild(pText6);	

	proot->appendChild(pId);
	proot->appendChild(pBid);
	proot->appendChild(pService);
	proot->appendChild(pQuantity);

	std::map<std::string, size_t>::iterator it;
	for (it= _decision_variables.begin(); it != _decision_variables.end(); ++it)
	{
		Poco::XML::AutoPtr<Poco::XML::Element> pDecisionVariable = 
											pDoc->createElement("Decision_Variable");
											
		// Name of the decision variable
		Poco::XML::AutoPtr<Poco::XML::Element> pDecisionVariableName = 
											pDoc->createElement("Name");
	    Poco::XML::AutoPtr<Poco::XML::Text> pText4 = 
							pDoc->createTextNode(it->first);
	    pDecisionVariableName->appendChild(pText4);	

		// Value of the decision variable
		Poco::XML::AutoPtr<Poco::XML::Element> pDecisionVariableValue = 
											pDoc->createElement("Value");
	    Poco::XML::AutoPtr<Poco::XML::Text> pText5 = 
							pDoc->createTextNode(getDecisionVariableStr(it->first));
	    pDecisionVariableValue->appendChild(pText5);	
	    
	    pDecisionVariable->appendChild(pDecisionVariableName);
	    pDecisionVariable->appendChild(pDecisionVariableValue);
		proot->appendChild(pDecisionVariable);
	}		
		
	pParent->appendChild(proot);

}

}  /// End Eco namespace

}  /// End ChoiceNet namespace

