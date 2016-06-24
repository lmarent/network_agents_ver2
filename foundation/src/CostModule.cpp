/*! \file   CostModule.cpp

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
    container class for cost function module

    $Id: CostModule.cpp 748 2016-06-23 15:29:00 amarentes $
*/

#include "ProcError.h"
#include "CostModule.h"
#include "CostModuleInterface.h"


using namespace ChoiceNet::Eco;



/* ------------------------- CostModule ------------------------- */

CostModule::CostModule( string libname, string libfile, 
                        libHandle_t libhandle, string confgroup, 
						configParam_t *params ) :
    Module( libname, libfile, libhandle ), confgroup(confgroup)
{    

    checkMagic(PROC_MAGIC);

    funcList = (CostModuleInterface_t *) loadAPI( "func" );
 
	setOwnName(libname); // TODO (change): read ownName from module properties XML file

	try
	{
				
		funcList->initModule(params);
	
	}
	catch(ProcError &e)
	{
		throw e;
	}
	
}


/* ------------------------- parseAttribList ------------------------- */


static string checkNullStr( const char* c ) {
    return (c == NULL) ? "" : c;
}



CostModule::~CostModule()
{

    funcList->destroyModule(NULL);

}

/* ------------------------- dump ------------------------- */

void CostModule::dump( ostream &os )
{
    Module::dump(os);
}


/* ------------------------- operator<< ------------------------- */

ostream& ChoiceNet::Eco::operator<< ( ostream &os, CostModule &obj )
{
    obj.dump(os);
    return os;
}
