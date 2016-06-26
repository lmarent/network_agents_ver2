
/*! \file   ModuleLoader.cpp

    Copyright 2014-2015 Universidad de los Andes, Bogotá, Colombia
                        University of Massachusetts, Amherst  			

    This file is part of the ChoiceNet Network Simulation System.

    ChoiceNet Network Simulation is free software; you can redistribute 
    it and/or modify it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ChoiceNet Network Simulation is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Description:
    load/unload algorithm proc modules

    $Id: ModuleLoader.cpp 748 2016-06-23 16:07:00 amarentes $

*/

#include "ProcError.h"
#include "ModuleLoader.h"
#include "Module.h"
#include "CostModule.h"
#include "CostModuleInterface.h"


using namespace ChoiceNet::Eco;

int     ModuleLoader::s_loaders = 0;

/* ------------------------- ModuleLoader ------------------------- */

ModuleLoader::ModuleLoader( string basedir, string group )
    : basepath(basedir), group(group)

{
    struct stat statbuf;

    	
	if (basepath.empty()){
		throw ProcError("ModuleLoader: invalid empty basepath ");
	}
	
    // add '/' if dir does not end in one already
    if (basepath[basepath.size()-1] != '/') {
        basepath += "/";
    }

    // test if specified dir does exist
    if (stat(basepath.c_str(), &statbuf) < 0) {
        throw ProcError("ModuleLoader: invalid basepath '%s': %s", 
                    basepath.c_str(), strerror(errno));
    }

}


/* ------------------------- ~ModuleLoader ------------------------- */

ModuleLoader::~ModuleLoader()
{
    moduleListIter_t iter;

    // close each dymamic library we have opened so far
    for (iter = modules.begin(); iter != modules.end(); iter++) {	
        saveDelete(iter->second); // deleting a Module
    }

}


/* ------------------------- getVersion ------------------------- */

int ModuleLoader::getVersion( string modname )
{
    moduleListIter_t iter;
    
    // search for module in list of already loaded modules
    if ((iter = modules.find(modname)) != modules.end()) {
        return (iter->second)->getVersion();
    } else {
        return -1;
    }
}

/* ------------------------- getVersion ------------------------- */

string ModuleLoader::getName( string modname )
{
    moduleListIter_t iter;
    
    // search for module in list of already loaded modules
    if ((iter = modules.find(modname)) != modules.end()) {
        return (iter->second)->getName();
    } else {
        return string();
    }
}

/* ------------------------- getModule ------------------------- */

Module *ModuleLoader::getModule( string modname )
{
    moduleListIter_t iter;

    if (modname.empty()) {
        throw ProcError("no module name specified");
    }
    
    // search for library in list of already used libraries
    if ((iter = modules.find(modname)) != modules.end()) {
        (iter->second)->link(); // increment reference count
        return iter->second; // return Module object
    }

    // no such module in the list of currently available modules ->try to load it
    return loadModule(modname, 0, NULL);
}


/* ------------------------- loadModule ------------------------- */

Module *ModuleLoader::loadModule( string libname, int preload, configParam_t *params )
{
    Module *module = NULL;
    string filename, path, ext, pre;
    libHandle_t libhandle = NULL;
    
    if (libname.empty()) {
        return NULL;
    }
    
    try {
        // if libname has relative path (or none) then use basepath
        if (libname[0] != '/') {
            path = basepath;
        }

        // use '.so' as postfix if it is not yet there
        if (libname.substr(0,3) != "lib") {
            pre = "lib";
        }

        // use '.so' as postfix if it is not yet there
        if (libname.substr(libname.size()-3,3) != ".so") {
            ext = ".so";
        }
	
        // construct filename of module including path and extension
        filename = path + pre + libname + ext;
        
        std::cout << filename << std::endl;
	
        // try to load the library module
        libhandle = dlopen(filename.c_str(), RTLD_LAZY);
        if (!libhandle) {

			fprintf(stderr, "%s\n", dlerror());
		}
        
        if (libhandle == NULL) {
            // try to load without .so extension (cater for libtool bug)
            filename = path + libname;
            
            libhandle = dlopen(filename.c_str(), RTLD_LAZY);
            if (libhandle == NULL) {
                throw ProcError("cannot load module '%s': %s", 
                            libname.c_str(), (char *) dlerror());
            }
        }
        
        // dlopen succeeded, now check what module we have there 
        // everything went fine up to now -> create new module info entry
        {
            int magic = fetchMagic(libhandle);
            
            if (magic == PROC_MAGIC) {
                module = new CostModule(libname, filename, libhandle, group, params);
            } else {
                throw ProcError("unsupported module type (unknown magic number)");
            }
        }
	
        module->link();             // increase sue counter in this module
        
        modules[libname] = module;  // and finally store the module
			                
        return module;
        
    } catch (ProcError &e) {

        if (module) {
            saveDelete(module);
        }
        if (libhandle) {
            dlclose(libhandle);
        }
        throw e;
    }
}


/* ------------------------- releaseModule ------------------------- */

int ModuleLoader::releaseModule( Module *minfo )
{
    moduleListIter_t iter;
    
    iter = modules.find(minfo->getModName());

    if (iter != modules.end()) {
        if (iter->second->unlink() == 0) {
            saveDelete(iter->second);
            modules.erase(iter);
        }
    }
 
    return 0;
}


/* ------------------------- dump ------------------------- */

void ModuleLoader::dump( ostream &os )
{
    moduleListIter_t iter;

    os << "ModuleLoader dump : " << endl;
    // close each dymamic library we have opened so far
    for (iter = modules.begin(); iter != modules.end(); iter++) {	
        os << "ModuleName:" << (iter->second)->getName() << endl;
    }


}


/* ------------------------- fetchMagic ------------------------- */

int ModuleLoader::fetchMagic( libHandle_t libHandle )
{
    // test for magic number in loaded module
    int *magic = (int *)dlsym(libHandle, "magic");

    if (magic == NULL) {
        throw ProcError("invalid module - no magic number present");
    } else {
        return *magic;
    }
}


/* ------------------------- operator<< ------------------------- */

ostream& ChoiceNet::Eco::operator<< ( ostream &os, ModuleLoader &ml )
{
    ml.dump(os);
    return os;
}
