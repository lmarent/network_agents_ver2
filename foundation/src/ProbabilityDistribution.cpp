#include <map>
#include <iostream>
#include <fstream>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>

#include "ProbabilityDistribution.h"
#include "FoundationException.h"

namespace ChoiceNet
{
namespace Eco
{

ProbabilityDistribution::ProbabilityDistribution(std::string id, std::string class_name, Domain domain):
_id(id),
_class_name(class_name),
_domain(domain)
{

}
	
ProbabilityDistribution::~ProbabilityDistribution()
{
	
}

std::string ProbabilityDistribution::getId()
{
	return _id;
}

std::string ProbabilityDistribution::getClassName()
{
	return _class_name;
}
	
Domain ProbabilityDistribution::getDomain()
{
	return _domain;
}

std::string ProbabilityDistribution::getDomainStr()
{
	std::string result;
	switch (_domain)
	{
		case DOM_CONTINUOUS:
			result = "continuous";
			break;
		
		case DOM_DISCRETE:
			result = "discrete";
			break;
		
		case DOM_UNDEFINED:
			result = "undefined";
			break;
	}
	return result;
}

void ProbabilityDistribution::addParameter(std::string name, double value)
{
	std::map<std::string, double>::iterator it;
	it = _parameters.find(name);
	if(it == _parameters.end()) {
		_parameters.insert ( std::pair<std::string, double>
									   (name,value) );
	}
	else
	{
		throw FoundationException("ProbabilityDistribution Parameter is already included");
	}
}
	
void ProbabilityDistribution::addPoint(double value, double probability)
{
	if (_domain == DOM_DISCRETE){
		std::map<double, double>::iterator it;
		it = _points.find(value);
		if(it == _points.end()) {
			_points.insert ( std::pair<double, double>
										   (value,probability) );
		}
		else
		{
			throw FoundationException("ProbabilityDistribution point is already included");
		}
	} 
	else{
		throw FoundationException("ProbabilityDistribution a point cannot be inserted in a continous distribution");
	}
	
}

void ProbabilityDistribution::readPointsFromFile(std::string location, std::string file_name)
{

    std::string separator = ":";
    std::string absFileName = location;
    absFileName.append("/");
    absFileName.append(file_name);

    // Open the file for reading, checking to make sure it was successfully opened
    std::ifstream inFile;
    // std::cout << "file_name:" << absFileName << std::endl;
    inFile.open(absFileName.c_str());

    if(inFile.is_open())
    {
        // std::cout << "We could open the file" << std::endl;
        for ( std::string line; getline( inFile, line );)
        {
			Poco::StringTokenizer dividedLine(line, separator, Poco::StringTokenizer::TOK_TRIM);
			if (dividedLine.count() == 2)
			{
				try{
					double point = Poco::NumberParser::parseFloat(dividedLine[0]);
					double probability = Poco::NumberParser::parseFloat(dividedLine[1]);
					
					// Validates typical poblems in data
					if ((probability < 0) || (probability > 1)) {
						std::string message = "Invalid probability for point ";
						Poco::NumberFormatter::append(message, probability);
						throw FoundationException(message);
					}
					else if (_points.count(point) > 0){
						std::string message = "Point ";
						Poco::NumberFormatter::append(message, point);
						message.append(" is already given as parameter");
						throw FoundationException(message);
					}
					else
					{
						// std::cout << "inserting line" << line << "Point:" << point << "Probability:" << probability << std::endl;
						_points.insert(std::pair<double,double>(point,probability));
					}
				}
				catch (Poco::SyntaxException e){
					throw FoundationException("Invalid value in probability distribution file");
				}
			}
			else
			{
				// we have to raise an exception because the file is not well formed.
				throw FoundationException("Probability point line is not in the correct format( point : probability )");
			}
		}
		inFile.close(); 
		
		// Verifies that the sum of probabilities is equal to 1.0
		double acum = 0;
		std::map<double,double>::iterator it;
		for(it = _points.begin(); it != _points.end(); it++) {
			acum = acum + it->second;
		}
		
		if (acum != 1.0)
		{
			_points.clear();
			throw FoundationException("Probabilities in the file don't sum up 1.0");
		}
		
    }
    else{
		throw FoundationException("Probability file was not found");
    }
	
}
	
void ProbabilityDistribution::to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
					Poco::XML::AutoPtr<Poco::XML::Element> pParent)
{
	// std::cout << "putting the information of the probability Distribution:" << getClassName() <<  std::endl;

	Poco::XML::AutoPtr<Poco::XML::Element> proot = 
											pDoc->createElement("Probability_Distribution");

	Poco::XML::AutoPtr<Poco::XML::Element> pId = 
							pDoc->createElement("Name");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(getId());
	pId->appendChild(pText1);		
	proot->appendChild(pId);

	Poco::XML::AutoPtr<Poco::XML::Element> pName = 
							pDoc->createElement("Domain");
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = pDoc->createTextNode(getDomainStr());
	pName->appendChild(pText2);		
	proot->appendChild(pName);
	
	// Sets the parameters.
	std::map<std::string, double >::iterator it;
	for ( it= _parameters.begin(); it != _parameters.end(); ++it)
	{
		Poco::XML::AutoPtr<Poco::XML::Element> pParameter = 
							pDoc->createElement("Parameter");
							
		Poco::XML::AutoPtr<Poco::XML::Element> pNamePar = 
							pDoc->createElement("Name");
	    Poco::XML::AutoPtr<Poco::XML::Text> pText3 = pDoc->createTextNode(it->first);							
	    pNamePar->appendChild(pText3);	

		Poco::XML::AutoPtr<Poco::XML::Element> pValuePar = 
							pDoc->createElement("Value");
		double value = it->second;
		std::string valueStr;
	    Poco::NumberFormatter::append(valueStr, value);
 	    Poco::XML::AutoPtr<Poco::XML::Text> pText4 = pDoc->createTextNode(valueStr);
 	    pValuePar->appendChild(pText4);	
 	    
 	    pParameter->appendChild(pNamePar);
 	    pParameter->appendChild(pValuePar);
 	    proot->appendChild(pParameter);
	}
	
	// Sets the points.
	std::map<double, double >::iterator it_points;
	for ( it_points = _points.begin(); it_points != _points.end(); ++it_points)
	{
		Poco::XML::AutoPtr<Poco::XML::Element> pPoint = 
							pDoc->createElement("Point");
							
		Poco::XML::AutoPtr<Poco::XML::Element> pValue = 
							pDoc->createElement("Value");
	    double valuePoint = it_points->first;
	    std::string valueStr;
	    Poco::NumberFormatter::append(valueStr, valuePoint);
	    Poco::XML::AutoPtr<Poco::XML::Text> pText3 = pDoc->createTextNode(valueStr);							
	    pValue->appendChild(pText3);	

		Poco::XML::AutoPtr<Poco::XML::Element> pProbability = 
							pDoc->createElement("Probability");
		double probability = it_points->second;
		std::string probabilityStr;
	    Poco::NumberFormatter::append(probabilityStr, probability);
 	    Poco::XML::AutoPtr<Poco::XML::Text> pText4 = pDoc->createTextNode(probabilityStr);
 	    pProbability->appendChild(pText4);	
 	    
 	    pPoint->appendChild(pValue);
 	    pPoint->appendChild(pProbability);
 	    proot->appendChild(pPoint);
	
	}

	pParent->appendChild(proot);

}
	
}  /// End Eco namespace

}  /// End ChoiceNet namespace
