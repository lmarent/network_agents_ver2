
/*! \file   CostModule.h

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
    container class for costFunctions modules (resource consumption) proc module

    $Id: CostModule.h 748 2016-06-23 15:29:00 amarentes $
*/


#ifndef _COSTMODULE_H_
#define _COSTMODULE_H_


#include "stdincpp.h"
#include "CostModuleInterface.h"
#include "Module.h"

namespace ChoiceNet
{
namespace Eco
{


/*! \short   container class that stores information about an evaluation module
  
    container class - stores information about an evaluation module such as 
    name, uid, function list, libhandle and reference counter
*/

class CostModule : public Module
{
  private:


    static int    s_ch;   //!< logging channel number for ExportModule class
    string confgroup;      //!< configuartion group for the module

    //! struct of functions pointers for library
    CostModuleInterface_t *funcList;

    
  public:


    CostModuleInterface_t *getAPI()  
    { 
        return funcList; 
    }

    virtual int getVersion()     
    { 
        return funcList->version; 
    }

    /*! \short   construct and initialize a CostModule object

        take the library handle of an evaluation module and retrieve all the
        other information (name, uid, function list) via this handle

        \arg \c _cnf - link to configuration manager (for cfg file query)
        \arg \c libname - name of the evaluation module 
        \arg \c filename - name of module including path and extension
        \arg \c libhandle - system handle for loaded library (see dlopen)
        \arg \c confgroup - Configuration group for the module.
        \arg \c params - 	Configuration params for the module.
    */
    CostModule( string libname, string libfile, libHandle_t libhandle, string confgroup, configParam_t *params );

    //! destroy a CostModule object
    ~CostModule();


    //! dump a CostModule object
    void dump( ostream &os );

};

//! overload for <<, so that a CostModule object can be thrown into an ostream
ostream& operator<< ( ostream &os, CostModule &obj );

}  // namespace Eco 

}  // namespace ChoiceNet

#endif // _COSTMODULE_H_
