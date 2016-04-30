#include <math.h>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>


#include "SimplestTrafficConverter.h"
#include "FoundationException.h"

namespace ChoiceNet
{
namespace Eco
{

SimplestTrafficConverter::SimplestTrafficConverter()
{

}
	
SimplestTrafficConverter::~SimplestTrafficConverter()
{
	
}

void SimplestTrafficConverter::readFromFile(std::string absolute_path, 
											std::string file_name,
											std::string serviceId )
{
	std::string separator = ":";
    std::string absFileName = absolute_path;
    absFileName.append(file_name);
    // Open the file for reading, checking to make sure it was successfully opened
    std::ifstream inFile;
    std::cout << "file_name:" << absFileName << std::endl;
    inFile.open(absFileName.c_str());
    unsigned last_period = 0;
    if(inFile.is_open())
    {
        std::cout << "We could open the file" << std::endl;
        for ( std::string line; getline( inFile, line );)
        {
			Poco::StringTokenizer dividedLine(line, separator, Poco::StringTokenizer::TOK_TRIM);
			if (dividedLine.count() == 3)
			{
				try
				{
					std::string serviceIdParam = dividedLine[0];
					if (serviceId.compare(serviceIdParam) == 0)
					{
						double average = Poco::NumberParser::parseUnsigned(dividedLine[1]);
						double variance = Poco::NumberParser::parseFloat(dividedLine[2]);
						setTrafficSampleConfiguration(average, variance);
						break;
					}
				} 
				catch (Poco::SyntaxException e)
				{
					throw FoundationException("Invalid value in demand file", 334);
				}
			}
			else
			{
				throw FoundationException("Invalid traffic line converter, the correct format is ( service_id:average:variance )", 335);
			}
		}
	}
	else
	{
		throw FoundationException("Simple traffic converter file was not found",336);
	}	
				
}


void SimplestTrafficConverter::setTrafficSampleConfiguration(double average, double variance)
{

	_average = average;
	_variance = variance;
            
}

unsigned SimplestTrafficConverter::getRequiredAgents(double total_demand)
{
	
	unsigned num_agents = ceil(total_demand / _average);	
	return num_agents;
} 	

}  /// End Eco namespace

}  /// End ChoiceNet namespace
