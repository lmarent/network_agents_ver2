#ifndef Service_INCLUDED
#define Service_INCLUDED

#include <string>
#include <map>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/Subsystem.h>
#include "DecisionVariable.h"
#include "DemandForecaster.h"
#include "PointSetDemandForecaster.h"
#include "TrafficConverter.h"
#include "SimplestTrafficConverter.h"


namespace ChoiceNet
{
namespace Eco
{

class Service
{
	
	// Both classes are declared as friends becuase they will use
	// the decision variables attribute.
	friend class Bid; 
	friend class Purchase; 
	friend class Provider;

public:
	Service();
	
	Service(std::string id, std::string name);
	
	~Service();
	
	void setId(std::string id);
	
	std::string getId();
	
	void addDecisionVariable(DecisionVariable *);
	
	DecisionVariable * getDecisionVariable(std::string id);
	
	size_t getDecisionVariableDimension(std::string id);
	
	void setName(std::string name);
	
	std::string getName();
	
	std::string to_string();
	
	void loadDemand(std::string file_name);
	
	void loadTrafficConverter(std::string converter_origin, 
							  std::string file_name);
	
	void to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
					Poco::XML::AutoPtr<Poco::XML::Element> pParent);
					
	double getForecast(int period);
	
	unsigned getRequiredAgents(double demand);
	
	void setTrafficConverter(SimplestTrafficConverter * traffic_converter);
	
	void loadTrafficConverter(std::string file_name);
	

protected:

    // Attributes.
	std::string _id;
	std::string _name;
	typedef std::map<std::string, DecisionVariable *> DecisionVariablesMap;
	std::map<std::string, size_t> _dimensions;
	DecisionVariablesMap _decision_variables;
	PointSetDemandForecaster * _demand_forecaster;  /// TODO: review the inhiritance that must be executed here.
	SimplestTrafficConverter * _traffic_converter;  /// TODO: review the inhiritance that must be executed here.
	size_t _last_dimension;
	
	
	// Methods.
	std::string findExecutionPath(void);
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // Service_INCLUDED
