
/*! \file MiscCostModule.cpp

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

    $Id: MiscCostModule.cpp 748 2016-06-25 8:35:00 amarentes $

*/

#include "MiscCostModule.h"


/*! \short   embed magic cookie number in every packet processing module
    _N_et_M_ate _P_rocessing Module */
int magic = PROC_MAGIC;

/*! \short   declaration of struct containing all function pointers of a module */
ChoiceNet::Eco::CostModuleInterface_t func = 
{ 
    3, 
    ChoiceNet::Eco::initModule, 
    ChoiceNet::Eco::destroyModule, 
    ChoiceNet::Eco::getEvaluation, 
    ChoiceNet::Eco::getParameters, 
    ChoiceNet::Eco::getName,
    ChoiceNet::Eco::setParameter, 
    ChoiceNet::Eco::destroy
};


