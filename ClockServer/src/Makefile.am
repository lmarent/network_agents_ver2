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
				@poco_CFLAGS@ -fno-inline -DDEBUG -ggdb
else
  AM_CXXFLAGS = -O2 -I@top_srcdir@/include @LIBNETAGENTSFDTION_CFLAGS@ \
				@poco_CFLAGS@
endif


# what flags you want to pass to the C compiler & linker
AM_LDFLAGS = @poco_LDFLAGS@ @LIBNETAGENTSFDTION_LIBS@

LDADD = @poco_LIBS@ 
