#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libnetagentsfdtion], [0.1], [tteixeira@umass.edu])
AM_INIT_AUTOMAKE
LT_INIT
AC_CONFIG_SRCDIR([src/Bid.cpp])

## ------------------------Begin instantiation part 
## Meaning:
## An instantiating macro is one which defines one or more tags, 
## usually referring to files that are to be translated by the generated 
## configure scripts, from a template containing Autoconf substitution 
## variables.

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL

AC_LANG_CPLUSPLUS

# Checks for libraries.


# Checks for header files.


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL


# check for Poco Foundation library
# Checks for libraries.
AC_LANG(C++)
SAVED_LDFLAGS=$LDFLAGS
SAVED_CPPFLAGS=$CPPFLAGS
SAVED_LIBS=$LIBS
CPPFLAGS="$CPPFLAGS -I/usr/local/include/Poco"
LDFLAGS="$LDFLAGS -L/usr/local/lib/"
LIBS="$LIBS -lPocoFoundation -lPocoUtil -lPocoNet -lPocoXML -lPocoData -lPocoDataMySQL"
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <DateTime.h>],
    [Poco::DateTime now])],
    [poco_LIBS="-lPocoFoundation -lPocoUtil -lPocoNet -lPocoXML -lPocoData -lPocoDataMySQL"] 
    [poco_CFLAGS="-DHAVE_PCAP -I/usr/local/include/Poco"] 
    [poco_LDFLAGS]="-L/usr/local/lib/"]
    [HAVE_POCO=1],
    [AC_MSG_WARN([libPoco is not installed.])])
CPPFLAGS=$SAVED_CPPFLAGS 
LDFLAGS=$SAVED_LDFLAGS
LIBS= $SAVED_LIBS

AC_SUBST(poco_CFLAGS)
AC_SUBST(poco_LIBS)
AC_SUBST(poco_LDFLAGS)

AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
