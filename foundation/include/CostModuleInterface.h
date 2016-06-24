/*! \file   CostModuleInterface.h

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
    Interface definition for Cost Function modules

    $Id: CostModuleInterface.h 748 2016-06-23 15:29:00 amarentes $
*/


#ifndef __COSTMODULEINTERFACE_H
#define __COSTMODULEINTERFACE_H

#include "stdinc.h"

namespace ChoiceNet
{
namespace Eco
{

typedef std::map<std::string, double> configParam_t;
typedef std::map<std::string, double>::iterator configParamIter_t;


typedef int (*timeout_func_t)( int timerID );

//! short   the magic number that will be embedded into every action module
#define PROC_MAGIC   ('N'<<24 | 'M'<<16 | '_'<<8 | 'P')


static const int offset_refer[] = {
    ( -14 ),
    ( 0 ),
    ( 0 ),
    ( 0 )
};


/*! \short   initialize the action module upon loading 
   \returns 0 - on success, <0 - else 
*/
void initModule( configParam_t *params );


/*! \short   cleanup action module structures before it is unloaded 
   \returns 0 - on success, <0 - else 
*/
void destroyModule( configParam_t *params );


/*! \short   get an evaluation from the cost function.

    \arg \c  variable 			- variable for evaluation.
    
*/
double getEvaluation( double variable );

/*! \short   get all parameters needed to evaluate the cost function.

*/
configParam_t * getParameters( void );


/*! \short   get the name of the cost function.

*/
std::string getName( void );

/*! \short   set a parameter for the cost function.

*/
std::string setParameter( std::string name, double value );


/*! \short   dismantle the module

    attention: do NOT free this slice of memory itself
    \arg \c  Configured parameters given to the module.
    \returns 0 - on success, <0 - else
*/
void destroy( configParam_t *params );


/*! \short   return error message for last failed function

    \arg \c    - error number (return value from failed function)
    \returns 0 - textual description of error for logging purposes
*/
std::string getErrorMsg( int code );



/*! \short   definition of interface struct for Action Modules 

  this structure contains pointers to all functions of this module
  which are part of the Action Module API. It will be automatically 
  set for an Action Module upon compilation (don't forget to include
  ActionModule.h into every module!)
*/

typedef struct {

    int version;

    void (*initModule)( configParam_t *params );
    
    void (*destroyModule)( configParam_t *params );

    double (*getEvaluation)( double variable );
    
    configParam_t * (*getParameters)(void);

	std::string (*getName)(void);
	
	void (*setParameter)( std::string name, double value );
					 
    void (*destroy)( configParam_t *params );
    
    std::string (*getErrorMsg)( int code );

} CostModuleInterface_t;

}  // namespace Eco 

}  // namespace ChoiceNet


#endif /* __COSTMODULEINTERFACE_H */
