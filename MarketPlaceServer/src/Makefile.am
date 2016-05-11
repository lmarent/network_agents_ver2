ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = MarketPlaceServer

MarketPlaceServer_SOURCES = Datapoint.cpp \
							MarketPlaceException.cpp \
							NondominatedsortAlgo.cpp \
							ConnectionHandler.cpp 	\
							MarketPlaceSys.cpp \
							MarketPlaceServer.cpp \
							main.cpp

if ENABLE_DEBUG
  AM_CXXFLAGS = -g  -I@top_srcdir@/include @LIBNETAGENTSFDTION_CFLAGS@ \
				 @poco_CFLAGS@ -fno-inline -DDEBUG -ggdb
else
  AM_CXXFLAGS = -O2 -I@top_srcdir@/include @LIBNETAGENTSFDTION_CFLAGS@ \
				@poco_CFLAGS@
endif

# what flags you want to pass to the C compiler & linker
AM_LDFLAGS = @poco_LDFLAGS@ @LIBNETAGENTSFDTION_LIBS@

LDADD = @poco_LIBS@ 
