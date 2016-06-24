lib_LTLIBRARIES = libDelayLinealCostFunction.la libNonApplicableCost.la \
				  libDelayNaturalLogarithm.la libDelayQudraticCostFunction.la

if ENABLE_DEBUG
  AM_CFLAGS = -I$(top_srcdir)/foundation/include \
			  -I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			  -g -DDEBUG -DPROFILING
			  
  AM_CXXFLAGS = -I$(top_srcdir)/foundation/include \
				-I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			    -g -DDEBUG -DPROFILING
else
  AM_CFLAGS = -I$(top_srcdir)/foundation/include \
			  -I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			  -O2
			  
  AM_CXXFLAGS = -I$(top_srcdir)/foundation/include \
				-I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			    -O2 
endif

# --------------------- libDelayLinealCostFunction  --------------------
libDelayLinealCostFunction_la_CPPFLAGS = -I$(top_srcdir)/foundation/include
		   
libDelayLinealCostFunction_la_LDFLAGS = -export-dynamic
					
libDelayLinealCostFunction_la_SOURCES = DelayLinealCostFunction.cpp
libDelayLinealCostFunction_la_LIBADD = @LIBNETAGENTSFDTION_LIBS@

# --------------------- libNonApplicableCost  --------------------
libNonApplicableCost_la_CPPFLAGS = -I$(top_srcdir)/foundation/include 
		   
libNonApplicableCost_la_LDFLAGS = -export-dynamic
					
libNonApplicableCost_la_SOURCES = NonApplicableCost.cpp
libNonApplicableCost_la_LIBADD = @LIBNETAGENTSFDTION_LIBS@


# --------------------- libDelayNaturalLogarithm  --------------------
libDelayNaturalLogarithm_la_CPPFLAGS = -I$(top_srcdir)/foundation/include 
		   
libDelayNaturalLogarithm_la_LDFLAGS = -export-dynamic
					
libDelayNaturalLogarithm_la_SOURCES = DelayNaturalLogarithm.cpp
libDelayNaturalLogarithm_la_LIBADD = @LIBNETAGENTSFDTION_LIBS@

# --------------------- libDelayQudraticCostFunction  --------------------
libDelayQudraticCostFunction_la_CPPFLAGS = -I$(top_srcdir)/foundation/include 
		   
libDelayQudraticCostFunction_la_LDFLAGS = -export-dynamic
					
libDelayQudraticCostFunction_la_SOURCES = DelayQudraticCostFunction.cpp
libDelayQudraticCostFunction_la_LIBADD = @LIBNETAGENTSFDTION_LIBS@
