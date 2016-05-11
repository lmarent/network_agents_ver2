#include <string>
#include <Poco/NumberParser.h>
#include <iostream>
#include <Poco/NumberFormatter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>

#include "Bid.h"
#include "Message.h"
#include "FoundationException.h"
#include "Datapoint.h"
#include "Service.h"
#include "DecisionVariable.h"



namespace ChoiceNet
{
namespace Eco
{

Bid::Bid(std::string idParam,
		 std::string providerParam, 
		 std::string serviceParam,
		 size_t numberDecisionVariables):
Datapoint(idParam),
_provider(providerParam),
_service(serviceParam),
_status(active)
{
	// Initializes all dimensions for decision variables in 0.
	for (size_t i=0; i< numberDecisionVariables; ++i)
	{
		Datapoint::addNumber(0);
	}
}

Bid::Bid(Service *service, Message & message): 
Datapoint()
{
	try
	{
		// Initializes all dimensions for decision variables in 0.
		for (size_t i=0; i< (service->_decision_variables).size(); ++i)
		{
			Datapoint::addNumber(0);
		}

   	    // Obtains the values required to create a bid from the message 
	    _id = message.getParameter("Id");
	    _provider = message.getParameter("Provider");
	    _service = message.getParameter("Service");
	    setStatus(message.getParameter("Status"));
	    
		std::map<std::string, DecisionVariable *>::iterator it; 
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			std::string decision_variable = it->first;
			size_t dimension = service->getDecisionVariableDimension(decision_variable);
			std::string valueStr =  message.getParameter(decision_variable);
			double value = Poco::NumberParser::parseFloat(valueStr);
			// std::cout << "reading decision variable: " << decision_variable << "value:" << value << std::endl;
		
			// Insert in the mapping between the names of decision variables and their dimensions.
			_decision_variables.insert( std::pair<std::string, size_t>(decision_variable,dimension));
			
			// Insert the decision variable objetive
			std::string objetive = (*(it->second)).getObjectiveStr();
			// std::cout << "Objetive:" << objetive << std::endl;

			int objetive_int;
			if (objetive.compare("maximize") == 0){
				objetive_int = 1;
				Datapoint::setNumberAtDim(dimension, value);
			}
			else{
				Datapoint::setNumberAtDim(dimension, value * -1);
				if (objetive.compare("minimize") == 0)
					objetive_int = 2;
				else
					objetive_int = 3;
			}
			_decision_variables_objectives.insert( std::pair<std::string, int> (decision_variable, objetive_int) );
			// std::cout << "Ending decision variable: " << decision_variable << std::endl;
		}
	}
	catch (FoundationException &ex){
		throw FoundationException(ex.message(), ex.code());
	}
}
		
Bid::~Bid()
{
	// std::cout << "Number of neigborhood bids" << _neighbors.size() << std::endl;
	std::cout << "Deleting the bid" << std::endl;
}

std::string Bid::getId()
{
	return _id;
}
	
std::string Bid::getProvider()
{
	return _provider;
}

std::string Bid::getService()
{
	return _service;
}

std::string Bid::getStatus(void)
{
	
	if (_status == inactive)
		return "inactive";
	else
		return "active";
}

bool Bid::isActive(void)
{
	if (_status == active)
		return true;
	else
		return false;
}

void Bid::setStatus(std::string status)
{
	// Verifies the status given.
	if ((status).compare("inactive") == 0)
	{
		_status = inactive;
	}
	else 
	{
		_status = active;
	}
}

void Bid::setDecisionVariable(std::string decisionVariableId, size_t dimension, double value, OptimizationObjective &objetive)
{
	std::map<std::string, size_t>::iterator it;
	it = _decision_variables.find(decisionVariableId);
	if(it == _decision_variables.end()) {
		// Insert in the mapping between the names of decision variables and their dimensions.
		_decision_variables.insert( std::pair<std::string, size_t>(decisionVariableId,dimension));
		
		// Insert the decision variable objetive
		int objetive_int;
		if (objetive == MAXIME){
		    objetive_int = 1;
			Datapoint::setNumberAtDim(dimension, value);
		}
		else{
			Datapoint::setNumberAtDim(dimension, value * -1);
			if (objetive == MINIMIZE)
				objetive_int = 2;
			else
				objetive_int = 3;
		}
		_decision_variables_objectives.insert( std::pair<std::string, int> (decisionVariableId, objetive_int));
		
	}
	else{
		throw FoundationException("The decision variable is already in the Bid");
	}
}

double Bid::getDecisionVariable(std::string decisionVariableId)
{
	// std::cout << "Entering getDecisionVariable -" << decisionVariableId << std::endl;
	std::map<std::string, size_t>::iterator it;
	it = _decision_variables.find(decisionVariableId);
	std::map<std::string, int>::iterator it_objetive;
	it_objetive = _decision_variables_objectives.find(decisionVariableId);
	if(it != _decision_variables.end()) {
		size_t dimension = it->second;
		int objetive_int = it_objetive->second;
		
		double val = Datapoint::getNumberAtDim(dimension);
		if (objetive_int != 1)
			val = val * -1;
		// std::cout << "Ending getDecisionVariable -" << val << std::endl;
		return val;
	}
	else
	{
		throw FoundationException("The decision variable is not part of the bid");
	}
}

std::string Bid::getDecisionVariableStr(std::string decisionVariableId)
{
	double value =  getDecisionVariable(decisionVariableId);
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, value);
	return valueStr;
}

void Bid::to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
				 Poco::XML::AutoPtr<Poco::XML::Element> pParent)
{
	// std::cout << "putting the information of the bid:" << getId() <<  std::endl;

	Poco::XML::AutoPtr<Poco::XML::Element> proot = 
											pDoc->createElement("Bid");

	Poco::XML::AutoPtr<Poco::XML::Element> pId = 
							pDoc->createElement("Id");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(getId());
	pId->appendChild(pText1);
	
	Poco::XML::AutoPtr<Poco::XML::Element> pProvider = 
							pDoc->createElement("Provider");
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = 
							pDoc->createTextNode(getProvider());
	pProvider->appendChild(pText2);							

	Poco::XML::AutoPtr<Poco::XML::Element> pService = 
											pDoc->createElement("Service");
	Poco::XML::AutoPtr<Poco::XML::Text> pText3 = 
							pDoc->createTextNode(getService());
	pService->appendChild(pText3);	

	Poco::XML::AutoPtr<Poco::XML::Element> pStatus = 
											pDoc->createElement("Status");
	Poco::XML::AutoPtr<Poco::XML::Text> pText6 = 
							pDoc->createTextNode(getStatus());
	pStatus->appendChild(pText6);	
	
									
	proot->appendChild(pId);
	proot->appendChild(pProvider);
	proot->appendChild(pService);
	proot->appendChild(pStatus);
	

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

void Bid::addNeighbor(std::string bidId)
{
	_neighbors.insert(std::pair<std::string, std::string>(bidId, bidId));
}

void Bid::deleteNeighbor(std::string bidId)
{
	std::map<std::string,std::string>::iterator it;
	it = _neighbors.find(bidId);
	if (it != _neighbors.end())
	{
		_neighbors.erase(it); 
	}
}

int Bid::getNumberOfNeighbors(void)
{
	return (int) _neighbors.size();
}

void Bid::getNeighbors(std::vector<std::string>& listResult)
{
	    // Iterate over the provider bids
    for(std::map<std::string, std::string>::iterator it = _neighbors.begin(); it != _neighbors.end(); it++) 
    {
		listResult.push_back(it->first);
	}
}

void Bid::toMessage(Message & message)
{
	message.setParameter("Provider", getProvider());
	message.setParameter("Service", getService());
	message.setParameter("Id", getId());
	message.setParameter("Status", getStatus());
	std::map<std::string, size_t>::iterator it;
	for (it= _decision_variables.begin(); it != _decision_variables.end(); ++it)
	{
		message.setParameter(it->first, getDecisionVariableStr(it->first));
	}
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace

