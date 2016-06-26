
check_PROGRAMS = test_runner

API_INC			= $(top_srcdir)/include
INC_DIR 		= $(top_srcdir)/include/


test_runner_SOURCES =  @top_srcdir@/src/Module.cpp \
					   @top_srcdir@/src/CostModule.cpp \
					   @top_srcdir@/src/ModuleLoader.cpp \
					   @top_srcdir@/src/ProcError.cpp \
					   @top_srcdir@/src/Bid.cpp \
					   @top_srcdir@/src/BidInformation.cpp \
					   @top_srcdir@/src/BidProviderInformation.cpp \
					   @top_srcdir@/src/BidServiceInformation.cpp \
					   @top_srcdir@/src/DecisionVariable.cpp \
					   @top_srcdir@/src/DemandForecaster.cpp \
					   @top_srcdir@/src/FoundationException.cpp \
					   @top_srcdir@/src/FoundationSys.cpp \
					   @top_srcdir@/src/Datapoint.cpp \
					   @top_srcdir@/src/Listener.cpp \
					   @top_srcdir@/src/Message.cpp \
					   @top_srcdir@/src/NondominatedsortAlgo.cpp \
					   @top_srcdir@/src/PointSetDemandForecaster.cpp \
					   @top_srcdir@/src/ProbabilityDistribution.cpp \
					   @top_srcdir@/src/CostFunction.cpp \
					   @top_srcdir@/src/Provider.cpp \
					   @top_srcdir@/src/Purchase.cpp \
					   @top_srcdir@/src/PurchaseInformation.cpp \
					   @top_srcdir@/src/PurchaseServiceInformation.cpp \
					   @top_srcdir@/src/ResourceAvailability.cpp \
					   @top_srcdir@/src/Resource.cpp \
					   @top_srcdir@/src/Service.cpp \
					   @top_srcdir@/src/SimplestTrafficConverter.cpp \
					   @top_srcdir@/src/WaitingSocketReactor.cpp \
					   @top_srcdir@/test/Provider_test.cpp \
					   @top_srcdir@/test/test_runner.cpp

test_runner_CPPFLAGS  = -I$(API_INC) $(CPPUNIT_CFLAGS) @poco_CFLAGS@ -DTEST_ENABLED

test_runner_LDFLAGS = @poco_LDFLAGS@ $(CPPUNIT_LIBS)

test_runner_LDADD  = -lm -ldl @poco_LDFLAGS@ @poco_LIBS@
					 
TESTS = $(check_PROGRAMS)


if ENABLE_DEBUG
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-g  -fno-inline -DDEBUG $(CPPUNIT_CFLAGS)
else
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-O2 $(CPPUNIT_CFLAGS)
endif

AM_CXXFLAGS += -Wall -ggdb


#  TEST_CXX_FLAGS=`cppunit-config --cflags`
