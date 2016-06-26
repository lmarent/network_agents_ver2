/*
 * Test the Natural Logarithm Cost Test module.
 *
 * $Id: NaturalLogarithmCost_test.cpp 2015-12-30 11:27:00 amarentes $
 * $HeadURL: https://./test/NaturalLogarithmCost_test.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "ModuleLoader.h"
#include "Module.h"
#include "CostModule.h" 
#include "config.h"
#include "ProcError.h"
#include <cmath>


using namespace ChoiceNet::Eco;

class NaturalLogarithmCost_Test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( NaturalLogarithmCost_Test );

    CPPUNIT_TEST( general_test );
	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void general_test();
    
    //! module pointer
    ModuleLoader *loader;
    CostModule *costmod = NULL;
    Module *mod = NULL; 
    
    string moduleName;
    
};

CPPUNIT_TEST_SUITE_REGISTRATION( NaturalLogarithmCost_Test );

void NaturalLogarithmCost_Test::setUp() 
{

	try
	{
						
		// Load the module
		string libname, path, ext;

		string moduleDir = DEF_LIBDIR;
		loader = new ModuleLoader( moduleDir, "FOUNDATION_COST");

	}
	catch(ProcError &e){
		std::cout << "Error:" << e.getError() << std::endl << std::flush;

        if (loader) {
            saveDelete(loader);
        }
        throw e;
	}
}

void NaturalLogarithmCost_Test::tearDown() 
{
	if (loader != NULL){
		saveDelete(loader);
	}	
}


void NaturalLogarithmCost_Test::general_test() 
{
	string moduleName = "libNaturalLogarithmCost";
	Module *module = loader->loadModule( moduleName, 0, NULL );
	CostModule *costmod = dynamic_cast<CostModule*>(module);

	if (costmod != NULL){
		costmod->getAPI()->setParameter("a",2.0);
		costmod->getAPI()->setParameter("b",3.0);
		double val = costmod->getAPI()->getEvaluation( 3.0 );
		val = round(val * 10000) / 10000; 
		CPPUNIT_ASSERT(val == 5.7726);
		
		loader->releaseModule(costmod);
	}
	else {
		// Unable to load the cost module
		double val = 0;
		CPPUNIT_ASSERT(val == 1);
	}
	
}
