#ifndef ProbabilityDistribution_INCLUDED
#define ProbabilityDistribution_INCLUDED

#include <map>
#include <string>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>

namespace ChoiceNet
{
namespace Eco
{

enum Domain
{
	DOM_CONTINUOUS = 1,
	DOM_DISCRETE = 2,
	DOM_UNDEFINED = 3
};

class ProbabilityDistribution
{

public:
	
	ProbabilityDistribution(std::string id, std::string class_name, Domain domain);
	
	~ProbabilityDistribution();
	
	std::string getId();
	
	std::string getClassName();
	
	Domain getDomain();
	
	std::string getDomainStr();
	
	void addParameter(std::string name, double value);
	
	void addPoint(double value, double probability);
	
	void readPointsFromFile(std::string location, std::string file_name);
	
	void to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
					Poco::XML::AutoPtr<Poco::XML::Element> pParent);
	
private: 
	std::string _id;
	std::string _class_name;
	Domain _domain;
	std::map<std::string, double> _parameters;
	std::map<double, double> _points;
	
};	

}  /// End Eco namespace

}  /// End ChoiceNet namespace


#endif // ProbabilityDistribution_INCLUDED
