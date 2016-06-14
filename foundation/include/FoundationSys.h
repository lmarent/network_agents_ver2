#ifndef FoundationSys_INCLUDED
#define FoundationSys_INCLUDED

#include <Poco/Util/Application.h>
#include <Poco/Util/Subsystem.h>
#include <Poco/Data/SessionPool.h>
#include <vector>
#include <map>

#include "Service.h"
#include "DecisionVariable.h"
#include "ProbabilityDistribution.h"
#include "CostFunction.h"
#include "Resource.h"


namespace ChoiceNet
{
namespace Eco
{

enum AgentType{
	CLOCK_SERVER = 0,
	MARKET_SERVER = 1
};


class FoundationSys
{

public:

    FoundationSys(AgentType type);
    ~FoundationSys(void);
    
    void initialize(Poco::Util::Application &app, int bid_periods, int pareto_fronts);
    void readGeneralParametersFromDataBase(void);
    
    /// Read the probability distribution associated data.
    void readProbabilityDistributionsFromDataBase(void);
    void readContinuousProbabilityDistributionsFromDataBase(int probabilityId, 
					ProbabilityDistribution * probDistribution);
	void readDiscreteProbabilityDistributionsFromDataBase( int probabilityId, 
					ProbabilityDistribution * probDistribution);
					
	/// Read the CostFunctions associated data from
	void readCostFunctionsFromDataBase(void);
    void readContinuousCostFunctionsFromDataBase(int costFunctionId, CostFunction * costFunction);
	
		
    void readResourcesFromDataBase(void);
    void readDecisionVariablesFromDataBase(void);
    void readServiceDecisionVariablesFromDataBase(int serviceId, Service * service);
    void readServicesFromDataBase(void);
    void readServiceToExecute(void);
    
    void insertProbabilityDistribution(ProbabilityDistribution * probability_distribution);
    void insertCostFunction(CostFunction * cost_function);
    void insertDecisionVariable(DecisionVariable *quality_parameter);
    void insertService(Service *service_parameter);
    
    int getBidPeriods();
    int getParetoFrontsToExchange();
    ProbabilityDistribution * getProbabilityDistribution(std::string id);
    CostFunction * getCostFunction(std::string id);
    Service * getService(std::string serviceId);
    DecisionVariable * getDecisionVariable(std::string decisionVariableId);
    Resource * getResource(std::string resourceId);
    SimplestTrafficConverter * loadTrafficConverter(std::string serviceId);
    
    AgentType getType();

    int getExecutionCount();

protected:
	
    int _bid_periods;
    int _pareto_fronts_to_exchange;
    int _execution_count;
    AgentType _type;
	
    typedef std::map<std::string, Resource *> ResourceContainer;
	typedef std::map<std::string, Service *> ServiceContainer;
	typedef std::map<std::string, DecisionVariable *> DecisionVariableContainer;
    typedef std::map<std::string, ProbabilityDistribution *> ProbabilityDistributionContainer;
    typedef std::map<std::string, CostFunction *> CostFunctionContainer;

	ServiceContainer _services;
    DecisionVariableContainer _decision_variables;
    ProbabilityDistributionContainer _probability_distributions;
    CostFunctionContainer _cost_functions;
    ResourceContainer _resources;
    
    std::vector<std::string> _services_to_execute;

	Poco::Data::SessionPool * _pool;
	
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace


#endif // FoundationSys_INCLUDED
