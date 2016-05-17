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
#include "Resource.h"


namespace ChoiceNet
{
namespace Eco
{

class FoundationSys
{

public:

    FoundationSys(void);
    ~FoundationSys(void);
    
    void initialize(Poco::Util::Application &app, int bid_periods, int pareto_fronts);
    void readGeneralParametersFromDataBase(void);
    void readProbabilityDistributionsFromDataBase(void);
    void readContinuousProbabilityDistributionsFromDataBase(int probabilityId, 
					ProbabilityDistribution * probDistribution);
	void readDiscreteProbabilityDistributionsFromDataBase( int probabilityId, 
					ProbabilityDistribution * probDistribution);
    void readResourcesFromDataBase(void);
    void readDecisionVariablesFromDataBase(void);
    void readServiceDecisionVariablesFromDataBase(int serviceId, Service * service);
    void readServicesFromDataBase(void);
    void readServiceToExecute(void);
    
    void insertProbabilityDistribution(ProbabilityDistribution * probability_distribution);
    void insertDecisionVariable(DecisionVariable *quality_parameter);
    void insertService(Service *service_parameter);
    
    int getBidPeriods();
    int getParetoFrontsToExchange();
    ProbabilityDistribution * getProbabilityDistribution(std::string id);
    Service * getService(std::string serviceId);
    DecisionVariable * getDecisionVariable(std::string decisionVariableId);
    Resource * getResource(std::string resourceId);
    SimplestTrafficConverter * loadTrafficConverter(std::string serviceId);
    
protected:
	
    int _bid_periods;
    int _pareto_fronts_to_exchange;
	
    typedef std::map<std::string, Resource *> ResourceContainer;
	typedef std::map<std::string, Service *> ServiceContainer;
	typedef std::map<std::string, DecisionVariable *> DecisionVariableContainer;
    typedef std::map<std::string, ProbabilityDistribution *> ProbabilityDistributionContainer;

	ServiceContainer _services;
    DecisionVariableContainer _decision_variables;
    ProbabilityDistributionContainer _probability_distributions;
    ResourceContainer _resources;
    
    std::vector<std::string> _services_to_execute;

	Poco::Data::SessionPool * _pool;
	
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace


#endif // FoundationSys_INCLUDED