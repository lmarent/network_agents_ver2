/*! \file   NonApplicableCost.cpp

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
	# Name:        	Non Applicable Cost Function
	#-----------------------------------------------------------------------

	This class implements the Non Applicable Cost Function defined as:
		y = 0

    $Id: NonApplicableCost.cpp 748 2016-06-23 15:29:00 amarentes $
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
 
    return 0;
	
}

ChoiceNet::Eco::defParams_t * ChoiceNet::Eco::getParameters( void )
{
	
	return &defParameters;
}

std::string ChoiceNet::Eco::getName( void )
{
	return std::string("NonApplicableCost");
}

void ChoiceNet::Eco::setParameter( std::string name, double value )
{
	// Nothing has to happen.
}

void ChoiceNet::Eco::destroy( ChoiceNet::Eco::configParam_t *params )
{


}

    
