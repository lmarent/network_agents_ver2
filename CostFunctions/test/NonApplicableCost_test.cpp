/*
 * Test the Quadratic Cost Test module.
 *
 * $Id: QuadraticCost_test.cpp 2015-12-30 11:27:00 amarentes $
 * $HeadURL: https://./test/QuadraticCost_test.cpp $
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

class NonApplicableCost_Test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( NonApplicableCost_Test );

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

CPPUNIT_TEST_SUITE_REGISTRATION( NonApplicableCost_Test );

void NonApplicableCost_Test::setUp() 
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

void NonApplicableCost_Test::tearDown() 
{
	if (loader != NULL){
		saveDelete(loader);
	}	
}


void NonApplicableCost_Test::general_test() 
{
	string moduleName = "NonApplicableCost";
	Module *module = loader->loadModule( moduleName, 0, NULL );
	CostModule *costmod = dynamic_cast<CostModule*>(module);

	if (costmod != NULL){
		costmod->getAPI()->setParameter("a",2.0);
		double val = costmod->getAPI()->getEvaluation( 3.0 );
		CPPUNIT_ASSERT(val == 0);
		
		loader->releaseModule(costmod);
	}
	else {
		// Unable to load the cost module
		double val = 0;
		CPPUNIT_ASSERT(val == 1);
	}
	
}
