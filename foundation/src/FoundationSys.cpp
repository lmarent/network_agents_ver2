#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/MySQL/Connector.h>
#include <iostream>

#include "FoundationSys.h"
#include "FoundationException.h"


namespace ChoiceNet
{
namespace Eco
{

FoundationSys::FoundationSys(void):
_pool(NULL),
_bid_periods(0),
_pareto_fronts_to_exchange(0)
{
	Poco::Data::MySQL::Connector::registerConnector();
}

FoundationSys::~FoundationSys(void)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().information("Eliminating services registered");
	// Release the memory assigned to service objects
	ServiceContainer::iterator it_service;
	it_service = _services.begin();
	while( (it_service !=_services.end())  )
	{
		delete it_service->second;
		_services.erase(it_service);
		++it_service;
	}
	
	app.logger().debug("Eliminating Decision Variables registered");
	// Release the memory assigned to Decision Variables objects
	DecisionVariableContainer::iterator it_parameter;
	it_parameter = _decision_variables.begin();
	while( (it_parameter !=_decision_variables.end())  )
	{
		delete it_parameter->second;
		_decision_variables.erase(it_parameter);
		++it_parameter;
	}
	
	app.logger().debug("Eliminating Resources registered");
	ResourceContainer::iterator it_resources;
	it_resources = _resources.begin();
	while( (it_resources != _resources.end())  )
	{
		delete it_resources->second;
		_resources.erase(it_resources);
		++it_resources;
	}

	app.logger().debug("Eliminating Probabilities registered");
	ProbabilityDistributionContainer::iterator it_probabilities;
	it_probabilities = _probability_distributions.begin();
	while( (it_probabilities != _probability_distributions.end())  )
	{
		delete it_probabilities->second;
		_probability_distributions.erase(it_probabilities);
		++it_probabilities;
	}
		
	app.logger().debug("Disconnecting from the database");
	if (_pool != NULL)
		delete _pool;
		
	Poco::Data::MySQL::Connector::unregisterConnector();
}

void FoundationSys::initialize(Poco::Util::Application &app, int bid_periods, int pareto_fronts)
{
	//
	// Connection string to POCO
	std::string connectionStr = "host=localhost;port=3306;user=root;password=password;db=Network_Simulation";
	
	app.logger().information("Connecting with the database");
	_pool = new Poco::Data::SessionPool("MySQL", connectionStr);
	
	readGeneralParametersFromDataBase();
	if (_bid_periods == 0 ){
		_bid_periods = bid_periods;
	}
	
	if (_pareto_fronts_to_exchange == 0)
	{
		_pareto_fronts_to_exchange = pareto_fronts;
	}
	
	std::cout << "Read the general parameters" << std::endl;
	readResourcesFromDataBase();
	std::cout << "Read the resources" << std::endl;
	readProbabilityDistributionsFromDataBase();
	std::cout << "Read the probability distributions" << std::endl;
	readDecisionVariablesFromDataBase();
	std::cout << "Read the decision variables" << std::endl;
	readServicesFromDataBase();
	std::cout << "Read the services" << std::endl;
	readServiceToExecute();
	std::cout << "Read the service to execute" << std::endl;
	app.logger().debug("Data has been read from the database");
}

void FoundationSys::readGeneralParametersFromDataBase(void)
{
	// Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
	int bid_periods = 0;
	int pareto_fronts_to_exchange = 0;
	
	Poco::Data::Statement select(session);
	select << "select bid_periods, pareto_fronts_to_exchange from simulation_generalparameters limit 1",
	          Poco::Data::into(bid_periods),
	          Poco::Data::into(pareto_fronts_to_exchange),
	          Poco::Data::range(0, 1); //  iterate over result set one row at a time
	 while (!select.done())
	 {
		 select.execute();
		 _bid_periods = bid_periods;
		 _pareto_fronts_to_exchange = pareto_fronts_to_exchange;
		 
		 std::cout << "In readGeneralParametersFromDataBase " << _bid_periods << std::endl;
	 }

}

void FoundationSys::readDiscreteProbabilityDistributionsFromDataBase( int probabilityId, 
					ProbabilityDistribution * probDistribution)
{
	// Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
		
	double value;
	double probability;
	Poco::Data::Statement select(session);
	select << "SELECT value, probability FROM simulation_discreteprobabilitydistribution where probability_id_id = ?",
			Poco::Data::into(value),
			Poco::Data::into(probability),
			Poco::Data::use(probabilityId),
			Poco::Data::range(0, 1); //  iterate over result set one row at a time
	
	while (!select.done())
    {
        select.execute();
        probDistribution->addPoint(value, probability);
    }	

}

void FoundationSys::readContinuousProbabilityDistributionsFromDataBase(int probabilityId, 
					ProbabilityDistribution * probDistribution)
{
	// Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
		
	std::string parameter;
	double value;
	Poco::Data::Statement select(session);
	select << "SELECT parameter, value FROM simulation_continuousprobabilitydistribution where probability_id_id = ?",
			Poco::Data::into(parameter),
			Poco::Data::into(value),
			Poco::Data::use(probabilityId),
			Poco::Data::range(0, 1); //  iterate over result set one row at a time
	
	while (!select.done())
    {
        select.execute();
        probDistribution->addParameter(parameter, value);
    }	

}


void FoundationSys::readProbabilityDistributionsFromDataBase(void)
{
	 // Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
	
	int id;
	std::string name;
	std::string domainStr;
	std::string class_name;
	
	Poco::Data::Statement select(session);
	select << "SELECT id, name, domain, class_name FROM simulation_probabilitydistribution",
			Poco::Data::into(id),
			Poco::Data::into(name),
			Poco::Data::into(domainStr),
			Poco::Data::into(class_name),
			Poco::Data::range(0, 1); //  iterate over result set one row at a time
	
	while (!select.done())
    {
        select.execute();
        Domain domain;
        if (domainStr.compare("C") == 0)
			domain = DOM_CONTINUOUS;
		else
			domain = DOM_DISCRETE;
		
		std::string probId;
		Poco::NumberFormatter::append(probId, id);
		ProbabilityDistribution * probDistribution = new ProbabilityDistribution(probId, class_name, domain);
		if (domain == DOM_DISCRETE)
		{
			readDiscreteProbabilityDistributionsFromDataBase( id, probDistribution);
		}
		else
		{
			readContinuousProbabilityDistributionsFromDataBase(id, probDistribution);
		}
		insertProbabilityDistribution(probDistribution);
    }
}

void FoundationSys::readResourcesFromDataBase(void)
{

	// Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
		
	int id;
	std::string name;
	Poco::Data::Statement select(session);
	select << "SELECT id, name FROM simulation_resource",
			Poco::Data::into(id),
			Poco::Data::into(name),
			Poco::Data::range(0, 1); //  iterate over result set one row at a time
	
	while (!select.done())
    {
        select.execute();
        std::string resourceId;
        Poco::NumberFormatter::append(resourceId, id);	
        Resource *resource = new Resource(resourceId); 
        resource->setName(name);
		_resources.insert(std::pair<std::string, Resource *> (resource->getId(), resource));
    }	
}


void FoundationSys::readDecisionVariablesFromDataBase(void)
{
	 // Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
	
	int id;
	std::string name;
	std::string optimizationStr;
	double minValue;
	double maxValue;
	int resId;
	std::string modelingStr;
	int sensDistribId;
	int valDistribId;
	
	Poco::Data::Statement select(session);
	select << "SELECT id, name, optimization, min_value, max_value, resource_id, modeling, sensitivity_distribution_id, value_distribution_id FROM simulation_decisionvariable",
			Poco::Data::into(id),
			Poco::Data::into(name),
			Poco::Data::into(optimizationStr),
			Poco::Data::into(minValue),
			Poco::Data::into(maxValue),
			Poco::Data::into(resId),
			Poco::Data::into(modelingStr),
			Poco::Data::into(sensDistribId),
			Poco::Data::into(valDistribId),
			Poco::Data::range(0, 1); //  iterate over result set one row at a time
	
	while (!select.done())
    {
        select.execute();
        OptimizationObjective objetive;
        if (optimizationStr.compare("C") == 0)
			objetive = MAXIME;
		else
			objetive = MINIMIZE;

        Modeling modeling;
        if (modelingStr.compare("Q") == 0)
			modeling = MODEL_QUALITY;
		else
			modeling = MODEL_PRICE;
		
		std::string decId, resourceId, sensitivityDistrId,valueDistrId;
		Poco::NumberFormatter::append(decId, id);
		Poco::NumberFormatter::append(resourceId, resId);
		Poco::NumberFormatter::append(sensitivityDistrId, sensDistribId);
		Poco::NumberFormatter::append(valueDistrId, valDistribId);	
		DecisionVariable * decisionVar = new DecisionVariable(decId);
		decisionVar->setName(name);
		decisionVar->setModelling(modeling);
		decisionVar->setRange(minValue,maxValue);
		decisionVar->setObjetive(objetive);
		decisionVar->setResource(resourceId);
		decisionVar->setProbabilityDistribution( SENSITIVITY, getProbabilityDistribution(sensitivityDistrId));
		decisionVar->setProbabilityDistribution( VALUE, getProbabilityDistribution(valueDistrId));
		insertDecisionVariable(decisionVar);
    }
}

void FoundationSys::readServiceDecisionVariablesFromDataBase(int serviceId,  Service * service)
{
	 // Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
	Poco::Data::Statement select(session);
	
	int id;
	select << "SELECT id_decision_variable_id FROM simulation_service_decisionvariable where id_service_id = ?",
			Poco::Data::into(id),
			Poco::Data::use(serviceId),
			Poco::Data::range(0, 1);
			
	while (!select.done())
	{
		select.execute();
		std::string variableId;
		Poco::NumberFormatter::append(variableId, id);	
		DecisionVariable * variable = getDecisionVariable(variableId);
		service->addDecisionVariable(variable);
	}

}

void FoundationSys::readServicesFromDataBase(void)
{
	 // Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
	int id;
	std::string name, file_name_demand, converter_origin, file_name_converter;
	Poco::Data::Statement select(session);
	select << "SELECT id, name, file_name_demand, converter_origin, file_name_converter FROM simulation_service",
			Poco::Data::into(id),
			Poco::Data::into(name),
			Poco::Data::into(file_name_demand),
			Poco::Data::into(converter_origin),
			Poco::Data::into(file_name_converter),
			Poco::Data::range(0, 1);
	while (!select.done())
    {
		select.execute();
		std::string serviceId;
		Poco::NumberFormatter::append(serviceId, id);
		Service *service = new Service();
		service->setId(serviceId);
		service->setName(name);
		service->loadDemand(file_name_demand);
		if (converter_origin.compare("D") == 0)
		{
			service->setTrafficConverter(loadTrafficConverter(serviceId));
		}
		else
		{
			service->loadTrafficConverter(file_name_converter);
		}
		readServiceDecisionVariablesFromDataBase(id, service);
		insertService(service);	
	}
}

void FoundationSys::readServiceToExecute(void)
{
	 // Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
	int id;
	std::string name;
	Poco::Data::Statement select(session);
	select << "select distinct b.service_id from simulation_consumer a, simulation_consumerservice b where a.id = b.consumer_id and b.execute = 1",
			Poco::Data::into(id),
			Poco::Data::range(0, 1);
	while (!select.done())
    {
		select.execute();
		std::string serviceId;
		Poco::NumberFormatter::append(serviceId, id);
		ServiceContainer::iterator it;
		it = _services.find(serviceId);
		if (it != _services.end())
		{
			_services_to_execute.push_back(serviceId);
		}
	}
}

void FoundationSys::insertProbabilityDistribution(ProbabilityDistribution * probability_distribution)
{
	ProbabilityDistributionContainer::iterator it;
	it = _probability_distributions.find(probability_distribution->getId());
	if(it == _probability_distributions.end()) {
		_probability_distributions.insert ( std::pair<std::string, ProbabilityDistribution *>
									   (probability_distribution->getId(),probability_distribution) );
	}
	else
	{
		throw FoundationException("Probability Distribution is already included in the container", 321);
	}
}

void FoundationSys::insertDecisionVariable(DecisionVariable *quality_parameter)
{
	DecisionVariableContainer::iterator it;
	it = _decision_variables.find(quality_parameter->getId());
	if(it == _decision_variables.end()) {
		_decision_variables.insert ( std::pair<std::string, DecisionVariable *>
									   (quality_parameter->getId(),quality_parameter) );
	}
	else
	{
		throw FoundationException("Decision Variable is already included in the container",322);
	}
}

void FoundationSys::insertService(Service *service_parameter)
{

	ServiceContainer::iterator it;
	it = _services.find(service_parameter->getId());
	if(it == _services.end()) {
		_services.insert ( std::pair<std::string, Service *>
									   (service_parameter->getId(),service_parameter) );
	}
	else
	{
		throw FoundationException("Service is already included in the container",323);
	}
}

ProbabilityDistribution * FoundationSys::getProbabilityDistribution(std::string id)
{
	ProbabilityDistributionContainer::iterator it;
	it = _probability_distributions.find(id);
	if(it != _probability_distributions.end()) 
	{
		return it->second;
	}
	else
	{
		throw FoundationException("The probability distribution is not part of the container",324);
	}
}

int FoundationSys::getBidPeriods()
{
	return _bid_periods;
}

int FoundationSys::getParetoFrontsToExchange()
{
	return _pareto_fronts_to_exchange;
}

Service * FoundationSys::getService(std::string serviceId)
{
	ServiceContainer::iterator it;
	it = _services.find(serviceId);
	if(it != _services.end()) {
		return it->second;
	}
	else
	{
		throw FoundationException("Service is not included in the container",325);
	}
}

DecisionVariable * FoundationSys::getDecisionVariable(std::string decisionVariableId)
{
    // Verify the resource register on the system.
    DecisionVariableContainer::iterator it_decision_variable;
    it_decision_variable = _decision_variables.find(decisionVariableId);
    if (it_decision_variable != _decision_variables.end())
    {
		return it_decision_variable->second;
	}
    else
    {
		Poco::Util::Application& app = Poco::Util::Application::instance();
		app.logger().debug("Decision Variable is not included in the container");
		throw FoundationException("Decision Variable is not included in the container", 320);
	}

}

Resource * FoundationSys::getResource(std::string resourceId)
{
    // Verify the resource register on the system.
    std::map<std::string, Resource *>::iterator it_resource;
    it_resource = _resources.find(resourceId);
    if (it_resource != _resources.end())
    {
		return it_resource->second;
	}
    else
    {
		Poco::Util::Application& app = Poco::Util::Application::instance();
		app.logger().debug("Resource is not included in the container");
		throw FoundationException("Resource is not included in the container", 313);
	}

}

SimplestTrafficConverter * FoundationSys::loadTrafficConverter(std::string serviceId)
{

	std::cout << "Starting loadTrafficConverter Parameter:" <<  serviceId << std::endl;
	
	// Obtain a session from the pool
	Poco::Data::Session session(_pool->get());
	double average, variance, market_potential;
	Poco::Data::Statement select(session);
	select << "SELECT average, variance, market_potential FROM simulation_consumerservice where service_id = ?",
				Poco::Data::into(average),
				Poco::Data::into(variance),
				Poco::Data::into(market_potential),
				Poco::Data::use(serviceId),
				Poco::Data::range(0, 1); //  iterate over result set one row at a time

	while (!select.done())
	{
		select.execute();
		SimplestTrafficConverter * traffic_converter = new SimplestTrafficConverter();
		traffic_converter->setTrafficSampleConfiguration(average, variance);
		return traffic_converter;
	}
	// if it is not configured, then it returns NULL.
	return NULL;		
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
