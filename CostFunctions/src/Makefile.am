EXTRA_DIST = MiscCostModule.h MiscCostModule.cpp

pkgincludedir = $(includedir)/$(PACKAGE)

API_INC		= $(top_srcdir)/include
INC_DIR 	= $(top_srcdir)/include

lib_LTLIBRARIES = libLinealCost.la libNonApplicableCost.la \
				  libNaturalLogarithmCost.la libQuadraticCost.la

if ENABLE_DEBUG
  AM_CFLAGS = -I$(top_srcdir)/include \
			  -I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			  -g -DDEBUG -DPROFILING
			  
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			    -g -DDEBUG -DPROFILING
else
  AM_CFLAGS = -I$(top_srcdir)/include \
			  -I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			  -O2
			  
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-I$(top_srcdir) @LIBNETAGENTSFDTION_CFLAGS@ \
			    -O2 
endif

# --------------------- libDelayLinealCostFunction  --------------------
libLinealCost_la_CPPFLAGS = -I$(top_srcdir)/include
		   
libLinealCost_la_LDFLAGS = -export-dynamic
					
libLinealCost_la_SOURCES = LinealCost.cpp
libLinealCost_la_LIBADD = MiscCostModule.lo @LIBNETAGENTSFDTION_LIBS@

# --------------------- libNonApplicableCost  --------------------
libNonApplicableCost_la_CPPFLAGS = -I$(top_srcdir)/include 
		   
libNonApplicableCost_la_LDFLAGS = -export-dynamic
					
libNonApplicableCost_la_SOURCES = NonApplicableCost.cpp
libNonApplicableCost_la_LIBADD = MiscCostModule.lo @LIBNETAGENTSFDTION_LIBS@


# --------------------- libDelayNaturalLogarithm  --------------------
libNaturalLogarithmCost_la_CPPFLAGS = -I$(top_srcdir)/include 
		   
libNaturalLogarithmCost_la_LDFLAGS = -export-dynamic
					
libNaturalLogarithmCost_la_SOURCES = NaturalLogarithmCost.cpp
libNaturalLogarithmCost_la_LIBADD = MiscCostModule.lo @LIBNETAGENTSFDTION_LIBS@

# --------------------- libDelayQudraticCostFunction  --------------------
libQuadraticCost_la_CPPFLAGS = -I$(top_srcdir)/include 
		   
libQuadraticCost_la_LDFLAGS = -export-dynamic
					
libQuadraticCost_la_SOURCES = QuadraticCost.cpp
libQuadraticCost_la_LIBADD = MiscCostModule.lo @LIBNETAGENTSFDTION_LIBS@
