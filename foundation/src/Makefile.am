ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libnetagentsfdtion.la

INC_DIR 	= $(top_srcdir)/include

pkginclude_HEADERS = $(INC_DIR)/ProcError.h \
				     $(INC_DIR)/stdinc.h \
				     $(INC_DIR)/stdincpp.h \
				     $(INC_DIR)/Module.h \
				     $(INC_DIR)/CostModuleInterface.h \
				     $(INC_DIR)/CostModule.h \
				     $(INC_DIR)/ModuleLoader.h \
					 $(INC_DIR)/Bid.h \
					 $(INC_DIR)/BidInformation.h  \
					 $(INC_DIR)/BidProviderInformation.h \
					 $(INC_DIR)/BidServiceInformation.h \
					 $(INC_DIR)/Datapoint.h \
					 $(INC_DIR)/DecisionVariable.h \
					 $(INC_DIR)/DemandForecaster.h \
					 $(INC_DIR)/FoundationException.h \
					 $(INC_DIR)/FoundationSys.h \
					 $(INC_DIR)/Listener.h \
					 $(INC_DIR)/Message.h \
					 $(INC_DIR)/NondominatedsortAlgo.h \
					 $(INC_DIR)/ParetoAlgo.h \
					 $(INC_DIR)/PointSetDemandForecaster.h \
					 $(INC_DIR)/ProbabilityDistribution.h \
					 $(INC_DIR)/CostFunction.h \
					 $(INC_DIR)/Provider.h \
					 $(INC_DIR)/Purchase.h \
					 $(INC_DIR)/PurchaseInformation.h \
					 $(INC_DIR)/PurchaseServiceInformation.h \
					 $(INC_DIR)/ResourceAvailability.h \
					 $(INC_DIR)/Resource.h \
					 $(INC_DIR)/Service.h \
					 $(INC_DIR)/SimplestTrafficConverter.h \
					 $(INC_DIR)/TrafficConverter.h \
					 $(INC_DIR)/WaitingSocketReactor.h

if ENABLE_DEBUG
  AM_CXXFLAGS = -g  -fno-inline -DDEBUG -ggdb -std=c++11
else
  AM_CXXFLAGS = -O2 -std=c++11
endif

libnetagentsfdtion_la_CPPFLAGS = -I$(top_srcdir) \
							   -I$(top_srcdir)/include

libnetagentsfdtion_la_SOURCES =  Module.cpp \
								 CostModule.cpp \
								 ModuleLoader.cpp \
								 Bid.cpp \
								 BidInformation.cpp \
								 BidProviderInformation.cpp \
								 BidServiceInformation.cpp \
								 DecisionVariable.cpp \
								 DemandForecaster.cpp \
								 FoundationException.cpp \
								 FoundationSys.cpp \
								 Datapoint.cpp \
								 Listener.cpp \
								 Message.cpp \
								 NondominatedsortAlgo.cpp \
								 PointSetDemandForecaster.cpp \
								 ProbabilityDistribution.cpp \
								 CostFunction.cpp \
								 Provider.cpp \
								 Purchase.cpp \
								 PurchaseInformation.cpp \
								 PurchaseServiceInformation.cpp \
								 ResourceAvailability.cpp \
								 Resource.cpp \
							     Service.cpp \
							     SimplestTrafficConverter.cpp \
								 WaitingSocketReactor.cpp		  
						  

libnetagentsfdtion_la_LDFLAGS = -export-dynamic
