
check_PROGRAMS = test_runner

API_INC			= $(top_srcdir)/include
INC_DIR 		= $(top_srcdir)/include/


test_runner_SOURCES =  @top_srcdir@/test/LinealCost_test.cpp \
					   @top_srcdir@/test/NaturalLogarithmCost_test.cpp \
					   @top_srcdir@/test/QuadraticCost_test.cpp \
					   @top_srcdir@/test/NonApplicableCost_test.cpp \
					   @top_srcdir@/test/test_runner.cpp

test_runner_CPPFLAGS  = -I$(API_INC) $(CPPUNIT_CFLAGS) \
						 @LIBNETAGENTSFDTION_CFLAGS@ @poco_CFLAGS@

test_runner_LDFLAGS = @poco_LDFLAGS@ @LIBNETAGENTSFDTION_LIBS@ $(CPPUNIT_LIBS)

test_runner_LDADD  = @LIBNETAGENTSFDTION_LIBS@  @poco_LDFLAGS@ @poco_LIBS@
					 

TESTS = $(check_PROGRAMS)


if ENABLE_DEBUG
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-g  -fno-inline -DDEBUG $(CPPUNIT_CFLAGS)
else
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-O2 $(CPPUNIT_CFLAGS)
endif

AM_CXXFLAGS += -Wall -ggdb -pedantic -Wno-long-long


#  TEST_CXX_FLAGS=`cppunit-config --cflags`
