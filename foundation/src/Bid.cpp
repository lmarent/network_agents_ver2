#include <string>
#include <Poco/NumberParser.h>
#include <iostream>
#include <Poco/NumberFormatter.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SessionFactory.h>



#include "Bid.h"
#include "Message.h"
#include "FoundationException.h"
#include "Datapoint.h"
#include "Service.h"
#include "DecisionVariable.h"


using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

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
_status(active),
_unitary_profit(0),
_unitary_cost(0),
_parent_bid_id(),
_capacity(0),
_init_capacity(0),
_creation_period(0)
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

	    std::string unitaryProfit = message.getParameter("UnitaryProfit");
	    std::string unitaryCost = message.getParameter("UnitaryCost");
	    std::string parentBidId = message.getParameter("ParentBid");
	    std::string capacity = message.getParameter("Capacity");
	    std::string period = message.getParameter("CreationPeriod");

	    setUnitaryProfit(unitaryProfit);
	    setUnitaryCost(unitaryCost);
	    setParentBidId(parentBidId);
	    setCapacity(capacity);
	    setInitCapacity(capacity);
	    setCreationPeriod(period);

		std::map<std::string, DecisionVariable *>::iterator it;
		for (it=(service->_decision_variables.begin()); it!=(service->_decision_variables.end()); ++it)
		{
			std::string decision_variable = it->first;
			size_t dimension = service->getDecisionVariableDimension(decision_variable);
			std::string valueStr =  message.getParameter(decision_variable);
			double value = Poco::NumberParser::parseFloat(valueStr);


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

double Bid::getUnitaryCost(void)
{
	return _unitary_cost;
}

double Bid::getUnitaryProfit(void)
{
	return _unitary_profit;
}

std::string Bid::getParentBidId(void)
{
	return _parent_bid_id;
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

void Bid::setParentBidId(std::string parentBidId)
{
	_parent_bid_id = parentBidId;
}

void Bid::setDecisionVariable(std::string decisionVariableId, size_t dimension, double value, OptimizationObjective objetive)
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

	Poco::XML::AutoPtr<Poco::XML::Element> pParentBid =
											pDoc->createElement("ParentBid");
	Poco::XML::AutoPtr<Poco::XML::Text> pText7 =
							pDoc->createTextNode(getParentBidId());
	pParentBid->appendChild(pText7);


	proot->appendChild(pId);
	proot->appendChild(pProvider);
	proot->appendChild(pService);
	proot->appendChild(pStatus);
	proot->appendChild(pParentBid);


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

BidStruct Bid::getDBBidStructure(int execute_count, int period)
{
	BidStruct BidS = { period,
					   getId(),
					   getProvider(),
					   getService(),
					   (int) _status,
					   getParetoStatus(),
					   numDominated(),
					   execute_count,
					   getUnitaryProfit(),
					   getUnitaryCost(),
					   getParentBidId(),
					   getCapacity(),
					   getInitCapacity(),
					   getCreationPeriod() };
	return BidS;

}

void Bid::getDBDecisionVariables(std::map<std::string, double>  *listDecVars )
{

	std::map<std::string, size_t>::iterator it;
	for (it= _decision_variables.begin(); it != _decision_variables.end(); ++it)
	{

		listDecVars->insert(std::pair<std::string,double>(it->first, getDecisionVariable(it->first)) );

	}

}


void Bid::toDatabase(Poco::Data::SessionPool * _pool, int execute_count, int period)
{
	std::cout << "putting the information of the bid:" << getId() <<  std::endl;

	Poco::Data::Session session(_pool->get());

	BidStruct BidS = { period,
					   getId(),
					   getProvider(),
					   getService(),
					   (int) _status,
					   getParetoStatus(),
					   numDominated(),
					   execute_count,
					   getUnitaryProfit(),
					   getUnitaryCost(),
					   getParentBidId(),
					   getCapacity(),
					   getInitCapacity(),
					   getCreationPeriod() };

	Poco::Data::Statement insert(session);
	insert << "insert into simulation_bid (period, bidId, providerId, status, paretoStatus, dominatedCount, execution_count, unitary_profit, unitary_cost, parentBidId, capacity, init_capacity, creation_period) values (?,?,?,?,?,?,?,?,?,?,?,?,?)",
				use(BidS._period),
				use(BidS._id),
				use(BidS._provider),
				use(BidS._status),
				use(BidS._paretoStatus),
				use(BidS._dominatedCount),
				use(BidS._execution_count),
				use(BidS._unitary_profit),
				use(BidS._unitary_cost),
				use(BidS._parent_bid_id),
				use(BidS._capacity),
				use(BidS._init_capacity),
				use(BidS._creation_period);

	insert.execute();

	std::map<std::string, size_t>::iterator it;
	for (it= _decision_variables.begin(); it != _decision_variables.end(); ++it)
	{

		DecisionVariableStruct DecisionVariableS =
		{
			getId(),
			it->first,
			getDecisionVariable(it->first),
			execute_count
		};

		Poco::Data::Statement insertDecisionVariable(session);
		insertDecisionVariable << "insert into simulation_bid_decision_variable (parentId, decisionVariableName, value, execution_count ) values(?,?,?,?)",
					use(DecisionVariableS._parentId),
					use(DecisionVariableS._name),
					use(DecisionVariableS._value),
					use(DecisionVariableS._execution_count);

		insertDecisionVariable.execute();

	}

	session.commit();
}

void Bid::setCreationPeriod(std::string period)
{
	_creation_period = Poco::NumberParser::parse(period);
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

void Bid::setUnitaryProfit(std::string unitaryProfit)
{
	_unitary_profit = Poco::NumberParser::parseFloat(unitaryProfit);
}

void Bid::setUnitaryCost(std::string unitaryCost)
{
	_unitary_cost = Poco::NumberParser::parseFloat(unitaryCost);
}

void Bid::setCapacity(std::string capacity)
{
	_capacity = Poco::NumberParser::parseFloat(capacity);
}

void Bid::setCapacity(double capacity)
{
	_capacity = capacity;
}

void Bid::setInitCapacity(std::string capacity)
{
	_init_capacity = Poco::NumberParser::parseFloat(capacity);
}


void Bid::setInitCapacity(double capacity)
{
	_init_capacity = capacity;
}

double Bid::getCapacity(void)
{
	return _capacity;
}

double Bid::getInitCapacity(void)
{
	return _init_capacity;
}


int Bid::getCreationPeriod(void)
{
	return _creation_period;
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace

