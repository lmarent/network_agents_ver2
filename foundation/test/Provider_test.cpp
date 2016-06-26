/*
 * Test the Lineal Cost Test module.
 *
 * $Id: LinealCost_test.cpp 2015-12-30 11:27:00 amarentes $
 * $HeadURL: https://./test/LinealCost_test.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "ModuleLoader.h"
#include "Module.h"
#include "CostModule.h" 
#include "config.h"
#include "ProcError.h"
#include "Bid.h"
#include "Purchase.h"
#include "Service.h"
#include "DecisionVariable.h"
#include "Resource.h"
#include "Provider.h"
#include "CostFunction.h"
#include "FoundationException.h"


using namespace ChoiceNet::Eco;

class Provider_Test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( Provider_Test );

    CPPUNIT_TEST( general_test );
	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void general_test();
    
    //! module pointer
    ModuleLoader *loader = NULL;
    CostModule *costmod = NULL;
    Module *mod = NULL;
    Service *service = NULL;
    
    string moduleName;
    
};

CPPUNIT_TEST_SUITE_REGISTRATION( Provider_Test );

void Provider_Test::setUp() 
{

	try
	{
						
		// Load the module
		string libname, path, ext;

		string moduleDir = DEF_LIBDIR;
		string moduleName = "LinealCost";

		loader = new ModuleLoader( moduleDir, "FOUNDATION_COST");

		service = new Service(std::string("1"), std::string("Service1"));
		
		DecisionVariable *qual = new DecisionVariable(std::string("1"));
		qual->setName(std::string("quality"));
		qual->setObjetive(MINIMIZE);
		qual->setModelling(MODEL_QUALITY);
		qual->setRange(0.0, 1.0);
		qual->setResource(std::string("resource1"));
		
		Module *module = loader->loadModule( "libLinealCost", 0, NULL );
		CostModule *linealCostmod = dynamic_cast<CostModule*>(module);
		
		CostFunction *linealCost = new CostFunction(std::string("1"), std::string("LinealCost"), RANGE_CONTINUOUS, loader);
		linealCost->addParameter("slope", 2.0);
		linealCost->addParameter("intercept", 3.0);
		
		linealCost->setModule(linealCostmod);
		qual->setCostFunction(linealCost);

		DecisionVariable *price = new DecisionVariable(std::string("2"));
		price->setName(std::string("price"));
		price->setObjetive(MINIMIZE);
		price->setModelling(MODEL_PRICE);
		price->setRange(0.0, 1.0);

		module = loader->loadModule( "libNonApplicableCost", 0, NULL );
		CostModule *nonApplicableCostmod = dynamic_cast<CostModule*>(module);
		
		CostFunction *nonCost = new CostFunction(std::string("2"), std::string("NonApplicableCost"), RANGE_CONTINUOUS, loader);
		nonCost->setModule(nonApplicableCostmod);
		price->setCostFunction(nonCost);
		
		service->addDecisionVariable(qual);
		service->addDecisionVariable(price);

	}
	catch(ProcError &e){
		std::cout << "Error:" << e.getError() << std::endl << std::flush;

        if (loader) {
            saveDelete(loader);
        }
        throw e;
	}
}

void Provider_Test::tearDown() 
{
	if (loader != NULL){
		saveDelete(loader);
	}	
}


void Provider_Test::general_test() 
{
	try
	{
		Module *module = loader->loadModule( moduleName, 0, NULL );
		
		CostModule *costmod = dynamic_cast<CostModule*>(module);
				
		Resource *resource1 = new Resource(std::string("resource1"));
		Resource *resource2 = new Resource(std::string("resource2"));
		Resource *resource3 = new Resource(std::string("resource3"));
		
		Provider provider(std::string("Provider1"), BULK_CAPACITY);
						
		provider.setInitialAvailability(resource1, 100);
		provider.setInitialAvailability(resource2, 100);
		provider.setInitialAvailability(resource3, 100);
				
		Purchase *pur1 = new Purchase(std::string("1010"), std::string("10"), service->getId(), 10, 2);
		Purchase *pur2 = new Purchase(std::string("1011"), std::string("10"), service->getId(), 10, 2);
		Purchase *pur3 = new Purchase(std::string("1012"), std::string("10"), service->getId(), 100, 2);
				
		Bid *bid1 = new Bid(std::string("10"), provider.getId(), service->getId(), 2);
		bid1->setDecisionVariable(std::string("1"), (size_t) 0, 0.14, MINIMIZE);
		bid1->setDecisionVariable(std::string("2"), (size_t) 1, 10.0, MINIMIZE);
		
		Bid *bid2 = new Bid(std::string("11"), provider.getId(), service->getId(), 2);
		Bid *bid3 = new Bid(std::string("12"), provider.getId(), service->getId(), 2);

		bool avail = provider.isBulkAvailable(1, service, pur1, bid1);
		CPPUNIT_ASSERT(avail == true);
				
		provider.deductAvailability(1,service, pur1, bid1);
		
		double availQty = 0;
		availQty = provider.getResourceAvailability(1, std::string("resource1"));
		CPPUNIT_ASSERT((availQty >= 52.7) && (availQty <= 52.9) );
		
		provider.deductAvailability(1, service, pur2, bid1);
		availQty = provider.getResourceAvailability(1, std::string("resource1"));
		CPPUNIT_ASSERT((availQty >= 5.5) && (availQty <= 5.7) );
		
		avail = provider.isBulkAvailable(1, service, pur3, bid1);
		CPPUNIT_ASSERT(avail == false);
		
		
	} catch (FoundationException &e){
		std::cout << e.displayText() << std::endl;
		CPPUNIT_ASSERT(1 == 0);
	}
}
