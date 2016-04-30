
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>

#include "PointSetDemandForecaster.h"
#include "FoundationException.h"

namespace ChoiceNet
{

namespace Eco
{

PointSetDemandForecaster::PointSetDemandForecaster(std::map<std::string, std::string> parameters )
{
	std::string separator = ":";
	std::string location = (parameters.find("location"))->second;
	std::string file_name = (parameters.find("file_name"))->second;

    std::string absFileName = location;
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
			if (dividedLine.count() == 2)
			{
				try{
					unsigned period = Poco::NumberParser::parseUnsigned(dividedLine[0]);
					double value = Poco::NumberParser::parseFloat(dividedLine[1]);
					
					// Validates typical poblems in data
					if (value < 0){
						std::string message = "Invalid value in demand file for period ";
						Poco::NumberFormatter::append(message, period);
						throw FoundationException(message);
					}
					
					if (_forecast.count(period) > 0){
						std::string message = "Period ";
						Poco::NumberFormatter::append(message, period);
						message.append(" is already given as parameter");
						throw FoundationException(message);
					}
					else{
						if ((last_period + 1) != period){
							std::string message = "Period ";
							Poco::NumberFormatter::append(message, period);
							message.append(" is not consecutive and create a partitioned interval");
							throw FoundationException(message, 330);
						}
						else{
							std::cout << "inserting line" << line << "Period:" << period << "Value:" << value << std::endl;
							_forecast.insert(std::pair<unsigned,double>(period,value));
							last_period = period;
						}
					}
				}
				catch (Poco::SyntaxException e){
					throw FoundationException("Invalid value in demand file", 331);
				}
			}
			else
			{
				// we have to raise an exception because the file is not well formed.
				throw FoundationException("Demand line not in the correct format( period : value )" , 332);
			}
		}
		inFile.close(); 
    }
    else
    {
		throw FoundationException("Demand file was not found", 333);
    }

}
    
PointSetDemandForecaster::~PointSetDemandForecaster()
{
	_forecast.clear();
}

double PointSetDemandForecaster::getForecast(unsigned period)
{
	// std::cout << "Start PointSetDemandForecaster -- getForecast" << _forecast.size() << std::endl;
	
	if (period > _forecast.size()){
		unsigned mod = period % _forecast.size();
		if (mod >= 1)
		{
			return _forecast[mod];
		}
		else
		{
			// This is assuming that periods are ordered as we check during creation.
			return _forecast[_forecast.size()];
		}
	}
	else{
		return _forecast[period];
	}
	// std::cout << "End PointSetDemandForecaster -- getForecast" << std::endl;
}
	
}   /// End Eco namespace

}  /// End ChoiceNet namespace    

