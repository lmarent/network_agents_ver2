#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libnetagentsfdtion], [0.1], [tteixeira@umass.edu])
AM_INIT_AUTOMAKE([subdir-objects])
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

AC_ARG_ENABLE(debug,
  [  --enable-debug            enable debug build ],
  [case "${enableval}" in
    yes) debug=true ;;
    no)  debug=false ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
  esac],[debug=false])
AM_CONDITIONAL(ENABLE_DEBUG, test x$debug = xtrue)
if test $debug = true ; then
  AC_DEFINE(ENABLE_DEBUG, 1, [enable debug build])
fi

AC_ARG_ENABLE(test,
  [  --enable-test            enable all test applications ],
  [case "${enableval}" in
    yes) tst=true ;;
    no)  tst=false ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-test) ;;
  esac],[tst=false])
AM_CONDITIONAL(ENABLE_TEST, test x$tst = xtrue)
if test $tst = true ; then
  AC_DEFINE(ENABLE_TEST, 1, [enable test applications])
fi

AC_CHECK_LIB([cppunit], [main])

# Checks for header files.


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL


# check for Poco Foundation library
# Checks for libraries.
AC_LANG(C++)
SAVED_LDFLAGS=$LDFLAGS
SAVED_CPPFLAGS=$CPPFLAGS
SAVED_LIBS=$LIBS
CPPFLAGS="$CPPFLAGS -I/usr/local/include/Poco -I/usr/include/Poco"
LDFLAGS="$LDFLAGS -L/usr/local/lib/ -L/usr/lib/"
LIBS="$LIBS -lPocoFoundation -lPocoUtil -lPocoNet -lPocoXML -lPocoData -lPocoDataMySQL"
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <DateTime.h>],
    [Poco::DateTime now])],
    [poco_LIBS="-lPocoFoundation -lPocoUtil -lPocoNet -lPocoXML -lPocoData -lPocoDataMySQL"] 
    [poco_CFLAGS="-DHAVE_PCAP -I/usr/local/include/Poco -I/usr/local/include/Poco"] 
    [poco_LDFLAGS]="-L/usr/local/lib/ -L/usr/lib/"]
    [HAVE_POCO=1],
    [AC_MSG_WARN([libPoco is not installed.])])
CPPFLAGS=$SAVED_CPPFLAGS 
LDFLAGS=$SAVED_LDFLAGS
LIBS= $SAVED_LIBS


# Additional arguments (and substitutions)
# Allow the pkg-config directory to be set
AC_ARG_WITH(pkgconfigdir,
	AC_HELP_STRING([--with-pkgconfigdir],
	[Use the specified pkgconfig dir (default is pkgconfig)]),
	[pkgconfigdir=${withval}],
	[pkgconfigdir='${exec_prefix}/lib/pkgconfig'])

AC_SUBST(poco_CFLAGS)
AC_SUBST(poco_LIBS)
AC_SUBST(poco_LDFLAGS)
AC_SUBST(pkgconfigdir)

dnl # set default directories

if test "${prefix}" = "NONE" ; then
  prefix=${ac_default_prefix}
fi

dnl # set default config directory for testing
sysconfdir="${sysconfdir}"

DEF_SYSCONFDIR=`eval echo ${sysconfdir}`
AC_DEFINE_UNQUOTED(DEF_SYSCONFDIR, "$DEF_SYSCONFDIR", "sysconfig dir")

DEF_STATEDIR=`eval echo ${localstatedir}`
AC_DEFINE_UNQUOTED(DEF_STATEDIR, "$DEF_STATEDIR", "state dir")

libdir="${libdir}/choicenet"
if test "${exec_prefix}" = "NONE" ; then
  exec_prefix=${prefix}
fi
DEF_LIBDIR=`eval echo ${libdir}`
AC_DEFINE_UNQUOTED(DEF_LIBDIR, "$DEF_LIBDIR", "lib dir")

DEF_BINDIR=`eval echo ${bindir}`
AC_DEFINE_UNQUOTED(DEF_BINDIR, "$DEF_BINDIR", "bin dir")


AC_CONFIG_FILES([Makefile \
				 libnetagentsfdtion-0.0.pc \
                 src/Makefile \
                 test/Makefile ])

AC_OUTPUT
