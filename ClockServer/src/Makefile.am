ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = ClockServer

ClockServer_SOURCES = main.cpp \
					  ClockServerException.cpp \
					  ConnectionHandler.cpp \
					  ClockServer.cpp \
					  ClockSys.cpp \
					  TimerNotification.cpp
						 
if ENABLE_DEBUG
  AM_CXXFLAGS = -g -I@top_srcdir@/include @LIBNETAGENTSFDTION_CFLAGS@ \
				@poco_CFLAGS@ -fno-inline -DDEBUG -ggdb -std=c++11
else
  AM_CXXFLAGS = -O2 -I@top_srcdir@/include @LIBNETAGENTSFDTION_CFLAGS@ \
				@poco_CFLAGS@ -std=c++11
endif


# what flags you want to pass to the C compiler & linker
AM_LDFLAGS = @LIBNETAGENTSFDTION_LIBS@ @poco_LDFLAGS@ 

LDADD = @LIBNETAGENTSFDTION_LIBS@ @poco_LIBS@ 
