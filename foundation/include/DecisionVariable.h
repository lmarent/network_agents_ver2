#ifndef Decision_Variable_INCLUDED
#define Decision_Variable_INCLUDED

#include <string>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>

#include "ProbabilityDistribution.h"


namespace ChoiceNet
{
namespace Eco
{

struct DecisionVariableStruct
{
	std::string _parentId;
	std::string _name;
	double _value;
	int _execution_count;
};

enum OptimizationObjective
{
	MAXIME = 1,
	MINIMIZE = 2,
	OO_UNDEFINED = 3
};

enum Purpose
{
	VALUE = 1,
	SENSITIVITY = 2,
	P_UNDEFINED = 3
};

enum Modeling
{
	MODEL_PRICE = 1,
	MODEL_QUALITY = 2,
	MODEL_UNDEFINED = 3
};

class DecisionVariable
{

public:

    DecisionVariable(std::string id);
    
    ~DecisionVariable();
    
    std::string getId();
    
    void setName(std::string name);
    
    std::string getName(void);
        
    void setObjetive(OptimizationObjective objetive);
    
    OptimizationObjective getObjetive(void);
    
    void setModelling(Modeling modeling);
    
    Modeling getModeling(void);
        
    std::string getModelingStr(void);
    
    std::string getObjectiveStr(void);

    void setRange(double min_value, double max_value);
    
    double getMinValue(void);
    
    std::string getMinValueStr(void);
    
    double getMaxValue(void);

	std::string getMaxValueStr(void);
    
    void setResource(std::string resourceId);
    
    std::string getResource();
        
    void setProbabilityDistribution(Purpose  purpose, ProbabilityDistribution * probability_distribution);
    
    ProbabilityDistribution * getProbabilityDistribution(Purpose purpose);
    
    void to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
					Poco::XML::AutoPtr<Poco::XML::Element> pParent);

protected:

	std::string getPurposeStr(Purpose purpose);


private:
	
	std::string _id;
	std::string _name;
	OptimizationObjective _opt_objetive;
	double _min_value;
	double _max_value;
	std::string _resourceId;  
	/// Corresponds to the resource that the decision variable consumes
	/// only it is associated when the objetive is a quality parameter.
	
	Modeling _is_modeling;
	std::map< std::string, ProbabilityDistribution *> _probability_distributions;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // Decision_Variable_INCLUDED
