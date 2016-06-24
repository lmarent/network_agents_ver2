
/*! \file   ModuleLoader.h

    Copyright 2014-2015 Universidad de los Andes, Bogotá, Colombia
                        University of Massachusetts, Amherst  			

    This file is part of the ChoiceNet Network Simulation System.

    ChoiceNet Network Simulation is free software; you can redistribute 
    it and/or modify it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    NETAUM is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Description:
    load/unload algorithm proc modules

    $Id: ModuleLoader.h 748 2016-06-23 16:07:00 amarentes $

*/

#ifndef _MODULELOADER_H_
#define _MODULELOADER_H_


#include "stdincpp.h"
#include "CostModuleInterface.h"
#include "Module.h"

namespace ChoiceNet
{
namespace Eco
{

/*! \short   struct that stores data for a loaded dynamic library

  this struct is used by a ModuleLoader object to store information
  about a dynamically loaded library
 */
struct ActionModule_t {
    //!< library handle from dlopen call
    void *libhandle; 
    
    //!< struct of functions pointers for library
    CostModuleInterface_t *funcList; 

    string name; //!< name supplied by module
    int    uid;  //!< unique id number supplied by module
    int    ref;  //!< reference (link) counter
};

//! module list
typedef map<string, Module*>            moduleList_t;
typedef map<string, Module*>::iterator  moduleListIter_t;


/*! \short   load dynamic libraries and extract function pointers
  
  the ModuleLoader class allows requesting of a dynamic library that is to
  be used. That Library will be loaded and it's functions made available
  to the caller using the ModuleLoader object.
 */

class ModuleLoader
{
  private:

	//! Group configuration for these modules
	string group;
	
    //! name of dir that contains the librari(es)
    string basepath;

     //! list storing data about loaded modules
    moduleList_t modules;
    
    //! number of ModuleLoader objects 'ever' created
    static int s_loaders;
    
  public:

    /*! \short   construct and initialize a ModuleLoader object

        \arg \c basedir - directory that contains libraries that can by
                          loaded with subsequent calls to getModule()
    */
    ModuleLoader( string basedir = "./", string group = "" );

    /*! \short   destroy a ModuleLoader object

        this will close all dynamic libraries used by this ModuleLoader (or at
        least decrement their reference counter if they are used otherwise)
    */
    ~ModuleLoader();


    /*! \short   load a metric module and make its functions available

        tries to open a module and return a list of functions from it. The
        library has to include 'ActionModule.h' and needs to implement all
        functions given in 'ProcModuleInterface.h'

        \arg \c modname - name of the module
        \arg \c preload - load module instantly (not only on demand) if ==1
        \returns a reference to a struct with pointers to all the functions of the module
        \throws Error in case the module is not available
    */
    Module *loadModule( string libname, int preload, configParam_t *params );


    /*! \short get the info block for a loaded metric module

        looks for a metric module with the specified name and returns
        an info block for this module 

        \arg \c libname - name of the library
        \returns a reference to a struct with pointers to all the functions of the Action Module
        \throws MyErr in case the module is not available
    */
    Module *getModule( string libname );

    /*! \short   decrement reference count to a loaded dynamic lobrary

        reduce the link count of the given library by one. If the link 
        count reaches zero by doing so, the library is unloaded.

        \arg \c minfo - pointer to module
    */
    int releaseModule( Module *minfo );

    /*! \short   get version number from module

      \arg \c modname - name of the evaluation module
      \returns the version number stored inside the module
    */
    int getVersion( string modname );

    /*! \short   get the module name
     * 
      \returns the module name stored inside the module
    */
    string getName( string modname );



    /*! \short fetch magic number from module lib file
     */
    int fetchMagic( libHandle_t libHandle );

    /*! \short   dump a ModuleLoader object

        display names of loaded libraries and the number of times 
        they have been requested
    */
    void dump( ostream &os );

    //! return number of currently loaded modules
    int numModules() 
    { 
        return modules.size(); 
    }
};


//! overload for <<, so that a ModuleLoader object can be thrown into an iostream
ostream& operator<< ( ostream &os, ModuleLoader &ml );

}  // namespace Eco 

}  // namespace ChoiceNet


#endif // _MODULELOADER_H_
