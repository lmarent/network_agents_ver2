/*! \file   LinealCost.cpp

    Copyright 2014-2016 Universidad de los Andes, Bogota, Colombia
                        University of Massachusetts, Amherst  			

    This file is part of the ChoiceNet Network Simulation System.

    NETQos is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    NETQoS is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Description:
    Delay function.

	#-----------------------------------------------------------------------
	# Name:        	Lineal Function
	#-----------------------------------------------------------------------

	This class implements the Lineal Cost Function defined as:
		slope*x + intercept

    $Id: LinealCost.cpp 748 2016-06-23 15:29:00 amarentes $
*/

#include "stdincpp.h"
#include "ProcError.h"
#include "CostModuleInterface.h"
#include "MiscCostModule.h"
#include <cmath>


ChoiceNet::Eco::configParam_t parameters;
ChoiceNet::Eco::defParams_t defParameters;

void ChoiceNet::Eco::initModule( ChoiceNet::Eco::configParam_t *params )
{

	

}

void ChoiceNet::Eco::destroyModule( ChoiceNet::Eco::configParam_t *params )
{



}

double ChoiceNet::Eco::getEvaluation( double variable )
{
	 // Variable initialization
	std::map<string, string>::iterator iter;
	configParamIter_t iter2;
	 
    // Verifies existance of all parameters.
    getParameters();
    for (iter = defParameters.begin(); iter != defParameters.end(); ++iter){
		std::string name = iter->second;
		iter2 = parameters.find(name);
		if (iter2 == parameters.end())
			throw ProcError("Parameter for cost evaluation not found - name%s", name.c_str());
	}
 
    double slope = parameters[std::string("slope")];
    double intercept = parameters[std::string("intercept")];
    return (slope * variable) + intercept;
	
}

ChoiceNet::Eco::defParams_t * ChoiceNet::Eco::getParameters( void )
{
	if (defParameters.size() == 0){
		defParameters.insert( std::pair<std::string, std::string>(std::string("1"), std::string("slope")));
		defParameters.insert( std::pair<std::string, std::string>(std::string("2"), std::string("intercept")));
	}
	
	return &defParameters;
}

std::string ChoiceNet::Eco::getName( void )
{
	return std::string("LinealCost");
}

void ChoiceNet::Eco::setParameter( std::string name, double value )
{
	std::map<string, string>::iterator iter;
    
    // Fill the param list in case it was not previously done
    getParameters( );
    
    for (iter = defParameters.begin(); iter != defParameters.end(); ++iter){
		if (iter->second == name){
			parameters[name] = value; // update or insert
		}	
	}
}

void ChoiceNet::Eco::destroy( ChoiceNet::Eco::configParam_t *params )
{


}


    
