#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/NumberFormatter.h>
#include <Poco/StringTokenizer.h>

#include "Service.h"
#include "DecisionVariable.h"
#include "FoundationException.h"

namespace ChoiceNet
{
namespace Eco
{

Service::Service(): _last_dimension(-1)
{
}
	
Service::Service(std::string id, std::string name): _last_dimension(0)
{
	_id = id;
	_name = name;
}
	
Service::~Service()
{
	// Release the memory assigned to decision variables
	DecisionVariablesMap::iterator it_variables;
	it_variables = _decision_variables.begin();
	while( (it_variables!=_decision_variables.end())  )
	{
		_decision_variables.erase(it_variables);
		++it_variables;
	}

    if (_demand_forecaster != NULL)
    {
		delete _demand_forecaster;
	}
	
	if (_traffic_converter != NULL)
	{
		delete _traffic_converter;
	}

	// The vector with dimension references is not necessary to iterate and delete.

}

void Service::setId(std::string id)
{
	_id = id;
}

std::string Service::getId()
{
	return _id;
}

void Service::addDecisionVariable(DecisionVariable * quality_param)
{
	std::map<std::string, DecisionVariable *>::iterator it;
	it = _decision_variables.find(quality_param->getId());
	if(it == _decision_variables.end()) {
		_decision_variables.insert ( std::pair<std::string, DecisionVariable *>
									   (quality_param->getId(),quality_param) );
		_last_dimension = _last_dimension + 1;							   
		// std::cout << "Inserting dec variable:" <<  quality_param->getId() << "in dimension:" << _last_dimension << std::endl;
		_dimensions.insert ( std::pair<std::string, size_t> 
							   ( quality_param->getId(), _last_dimension) );
	}
	else
	{
		throw FoundationException("Decision variable is already included");
	}
}
	
DecisionVariable * Service::getDecisionVariable(std::string id)
{
	std::map<std::string, DecisionVariable *>::iterator it;
	it = _decision_variables.find(id);
	if(it != _decision_variables.end()) {
		return it->second;
	}
	else
	{
		throw FoundationException("The decision variable is not part of the service");
	}
}

size_t Service::getDecisionVariableDimension(std::string id)
{
	std::map<std::string, size_t>::iterator it;
	it = _dimensions.find(id);
	if(it != _dimensions.end()) {
		return it->second;
	}
	else
	{
		throw FoundationException("The decision variable is not part of the service");
	}

}

void Service::setName(std::string name)
{
	_name = name;
}

std::string Service::getName()
{
	return _name;
}

std::string Service::to_string()
{
	std::string val_return;
	val_return = "Id:";
	val_return.append(_id);
	val_return.append("\n Name:");
	val_return.append(_name);
	std::map<std::string, DecisionVariable *>::iterator it;
	for (it=_decision_variables.begin(); it!=_decision_variables.end(); ++it)
	{
 	    val_return.append(it->first);
 	    val_return.append(" :");
 	    val_return.append((*it->second).getId());
	}

	std::map<std::string, size_t>::iterator it_dimension;
	for (it_dimension=_dimensions.begin(); it_dimension !=_dimensions.end(); ++ it_dimension)
	{
 	    val_return.append(it_dimension->first);
 	    val_return.append(" :");
 	    Poco::NumberFormatter::append(val_return, it_dimension->second);
	}

	return val_return;
}

void Service::to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
				   Poco::XML::AutoPtr<Poco::XML::Element> pParent)
{
	// std::cout << "putting the information of the service:" << getId() <<  std::endl;

	Poco::XML::AutoPtr<Poco::XML::Element> proot = 
											pDoc->createElement("Service");

	Poco::XML::AutoPtr<Poco::XML::Element> pId = 
							pDoc->createElement("Id");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(getId());
	pId->appendChild(pText1);		
	proot->appendChild(pId);

	Poco::XML::AutoPtr<Poco::XML::Element> pName = 
							pDoc->createElement("Name");
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = pDoc->createTextNode(getName());
	pName->appendChild(pText2);		
	proot->appendChild(pName);

	
	std::map<std::string, DecisionVariable *>::iterator it;
	for (it=_decision_variables.begin(); it!=_decision_variables.end(); ++it)
	{
 	    (*(it->second)).to_XML(pDoc, proot);
	}
	pParent->appendChild(proot);
}

std::string Service::findExecutionPath(void)
{
	// Finds the execution path, Only works for unix
	char result[ FILENAME_MAX  ];
	ssize_t count = readlink( "/proc/self/exe", result, FILENAME_MAX  );
	std::string path = std::string( result, (count > 0) ? count : 0 );
	Poco::StringTokenizer elements(path, "/", Poco::StringTokenizer::TOK_TRIM);
	int i = 0;
	std::string adj_path;
	while (i < elements.count() - 3)
	{
		adj_path.append(elements[i]);
		adj_path.append("/");
		i++;
	}
	std::cout << "path:" << adj_path << std::endl;
	return adj_path;
}


void Service::loadDemand(std::string file_name)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
    std::string demand_subdirect = app.config().getString("demand_directory", "demand/");
	
	std::string adj_path = findExecutionPath();
	std::cout << "The path is:" << adj_path;
	adj_path.append(demand_subdirect);
	
	// Loads the demand forecast.
	std::map<std::string, std::string> parameters;
	parameters.insert(std::pair<std::string, std::string> ("location", adj_path));
	parameters.insert(std::pair<std::string, std::string> ("file_name", file_name));
	_demand_forecaster = new PointSetDemandForecaster( parameters );
	
}


void Service::loadTrafficConverter(std::string file_name)
{
	_traffic_converter = new SimplestTrafficConverter();
	Poco::Util::Application& app = Poco::Util::Application::instance();
    std::string demand_subdirect = app.config().getString("demand_directory", "demand/");

	// Loads the traffic converter from file.
	std::string adj_path = findExecutionPath();
	adj_path.append(demand_subdirect);
	_traffic_converter->readFromFile(adj_path, file_name, _id);

}

void Service::setTrafficConverter(SimplestTrafficConverter * traffic_converter)
{
	_traffic_converter = traffic_converter;
}

double Service::getForecast(int period)
{
	if (_demand_forecaster != NULL)
	{
		return _demand_forecaster->getForecast(period);
	}
	else
	{
		return 0;
	}
}

unsigned Service::getRequiredAgents(double demand)
{
	if (_traffic_converter != NULL)
	{
		return _traffic_converter->getRequiredAgents(demand);
	}
	else
	{
		return 0;
	}
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace
