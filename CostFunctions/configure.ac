#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ChoicenetCost],[0.1],[la.marentes455@uniandes.edu.co])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT
AC_CONFIG_SRCDIR([/src/TwoAuctionMechanism.cpp])


## ------------------------Begin instantiation part 
## Meaning:
## An instantiating macro is one which defines one or more tags, 
## usually referring to files that are to be translated by the generated 
## configure scripts, from a template containing Autoconf substitution 
## variables.

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

# Checks for build environment
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL

LT_INIT([dlopen])
AC_PROG_MAKE_SET

AC_LANG([C++])

AC_CHECK_LIB([dl], [dlopen])


dnl # Platform 
case "$target" in
*-linux*)
        OS=LINUX
        AC_DEFINE(LINUX, 1, "Linux OS")
        ;;
*-sunos*)
        OS=SUNOS
        AC_DEFINE(SUNOS, 1, "SUNOS")
        ;;
*-irix*)
        OS=IRIX
        AC_DEFINE(IRIX, 1, "IRIX OS")
        ;;
*-solaris*)
        OS=SOLARIS
        AC_DEFINE(SOLARIS, 1, "Solaris OS")
        ;;
*-freebsd*)
        OS=FREEBSD
        AC_DEFINE(FREEBSD, 1, "FreeBSD OS")
        ;;
*dec-os*)
        OS=ALPHA
        AC_DEFINE(ALPHA, 1, "Alpha")
        ;;
*)
        OS=UNIX
        AC_DEFINE(UNIX, 1, "Unix")
        ;;
esac

dnl # Hardware

case "$target" in
*i*86*)
        PROC=LINUX
        AC_DEFINE(INTEL, 1, "Intel")
        ;;
esac

# use our own defaults
LDFLAGS="$LDFLAGS -L/lib -L/usr/local/lib -L/usr/lib "
CFLAGS="$_CFLAGS -Wall"
CXXFLAGS="$_CXXFLAGS -Wall -Wno-deprecated "

dnl # check the options

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

dnl # check size

AC_CHECK_SIZEOF(off_t)

AC_CHECK_LIB([cppunit], [main])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdlib.h string.h sys/socket.h sys/time.h termios.h unistd.h float.h types.h limits.h ])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([alarm bzero floor getaddrinfo inet_ntoa gettimeofday memmove memset select socket strdup strchr strerror strstr atoll strtof getopt_long])


PKG_CHECK_MODULES([LIBNETAGENTSFDTION], [libnetagentsfdtion-0.0])
AC_SUBST([LIBNETAGENTSFDTION_CFLAGS])
AC_SUBST([LIBNETAGENTSFDTION_LIBS])


AC_CHECK_PROG(DOXY,doxygen,doxygen,[])
if test "$DOXY" = "" ; then
  AC_MSG_WARN([doxygen not installed, can not build documentation])
fi

dnl # set version and data

dnl # Output substitution

AC_DEFINE_UNQUOTED(BUILD_TIME, "`date +"%d/%m/%Y %H:%M:%S"`", "release date")

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

libdir="${libdir}/auctionmanager"
if test "${exec_prefix}" = "NONE" ; then
  exec_prefix=${prefix}
fi
DEF_LIBDIR=`eval echo ${libdir}`
AC_DEFINE_UNQUOTED(DEF_LIBDIR, "$DEF_LIBDIR", "lib dir")

DEF_BINDIR=`eval echo ${bindir}`
AC_DEFINE_UNQUOTED(DEF_BINDIR, "$DEF_BINDIR", "bin dir")



AC_CONFIG_FILES([Makefile \
				 src/Makefile \
				 test/Makefile  ] )

AC_OUTPUT
