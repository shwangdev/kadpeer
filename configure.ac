#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.66])
AC_INIT([KadPeer], [1.0])
#AC_CONFIG_SRCDIR([udt/src/common.cpp])
# AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# Checks for OGG
HAVE_OGG=no
if test "x$HAVE_PKG_CONFIG" = "xyes" ; then
  PKG_CHECK_MODULES(OGG, ogg >= 1.0, HAVE_OGG=yes, HAVE_OGG=no)
fi

if test "x$HAVE_OGG" = "xno" ; then
  XIPH_PATH_OGG(HAVE_OGG="yes", HAVE_OGG="no")
  if test "x$HAVE_OGG" = "xno" ; then
    oggz_config_ok="no"
    AC_CHECK_LIB(ogg, ogg_stream_init, HAVE_OGG="maybe")
  fi
fi

AM_CONDITIONAL(HAVE_OGG, [test "x$HAVE_OGG" = "xyes"])

dnl Check for const-correfct  version of <ogg/ogg.h>
if test "x$HAVE_OGG" = "xyes" ; then
    AC_MSG_CHECKING([if ogg_page_bos() takes a const argument])
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS -Wall -Werror $OGG_CFLAGS"
    LIBS="$LIBS $OGG_LIBS"

    AC_TRY_COMPILE([#include <ogg/ogg.h>],
                   [int f(const ogg_page * cog){return ogg_page_bos(cog);}],
                   [ AC_DEFINE([OGG_H_CONST_CORRECT], [], [Define if <ogg/ogg.h> is const-correct])
                     AC_MSG_RESULT(yes) ],
                   [ AC_MSG_RESULT(no) ])

    CFLAGS="$ac_save_CFLAGS"
    LIBS="$ac_save_LIBS"
fi

dnl 
dnl Detect oggz
dnl

dnl HAVE_OGGZ="no"
dnl kadpeer_OGGZ="1.1.1"
dnl PKG_CHECK_MODULES(OGGZ, oggz >= "$kadpeer_OGGZ", HAVE_OGGZ="yes", HAVE_OGGZ="no")

dnl if test "$HAVE_OGGZ" = "yes" ; then
dnl   AC_DEFINE(HAVE_OGGZ, [], [Define if have liboggz])
dnl   AC_SUBST(OGGZ_CFLAGS)
dnl   AC_SUBST(OGGZ_LIBS)
dnl else
dnl   PKG_CHECK_MODULES(OGGZ, oggz, HAVE_OGGZ="old")

dnl  if test "$HAVE_OGGZ" = "old" ; then
dnl     AC_MSG_ERROR([
dnl *** The version of liboggz installed on your system is too old. You need
dnl *** liboggz version greater than $oggplay_OGGZ to build liboggplay.
dnl ])
dnl   else
dnl     AC_MSG_ERROR([
dnl *** liboggz-$oggplay_OGGZ or greater does not seem to exist on your system.
dnl *** Please install it and run this ./configure again.
dnl ])
dnl   fi
dnl fi

dnl Check for valgrind

dnl
dnl  Detect libkate
dnl
PKG_CHECK_MODULES(KATE, kate, HAVE_KATE="yes", HAVE_KATE="no")
if test "x$HAVE_KATE" = "xyes" ; then
  AC_DEFINE(HAVE_KATE, [], [Define if have libkate])
  AC_SUBST(KATE_CFLAGS)
  AC_SUBST(KATE_LIBS)
else
  AC_MSG_RESULT($HAVE_KATE)
fi

dnl
dnl  Detect libtiger
dnl
AC_ARG_WITH(
  tiger,
  AS_HELP_STRING(
    [--with-tiger],
    [Enable rendering of Kate streams with the Tiger rendering library, using Pango and Cairo (default autodetect)]
  ),
  [use_tiger="$withval"]
)
if test "x$use_tiger" != "xno"; then
  PKG_CHECK_MODULES(TIGER, tiger >= 0.3.1, HAVE_TIGER="yes", HAVE_TIGER="no")
  if test "x$HAVE_TIGER" = "xyes" ; then
    AC_DEFINE(HAVE_TIGER, [], [Define if have libtiger])
    AC_SUBST(TIGER_CFLAGS)
    AC_SUBST(TIGER_LIBS)
  else
    AC_MSG_RESULT($HAVE_TIGER)
    if test "x$use_tiger" = "xyes"; then
      AC_MSG_ERROR([libtiger could not be found and was explicitely requested])
    fi
  fi
else
  HAVE_TIGER=no
fi

dnl
dnl
dnl
dnl Detect libsndfile1 (used by dump-all-streams example)
dnl 

PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.0,
                  HAVE_LIBSNDFILE1="yes", HAVE_LIBSNDFILE1="no")

if test "x$HAVE_LIBSNDFILE1" = xyes ; then
  oggplay_examples="$oggplay_examples dump-all-streams"
  AC_DEFINE(HAVE_LIBSNDFILE1, [], [Define if have libsndfile])
  AC_SUBST(SNDFILE_CFLAGS)
  AC_SUBST(SNDFILE_LIBS) 
fi
AM_CONDITIONAL(HAVE_LIBSNDFILE1, [test "x$HAVE_LIBSNDFILE1" = "xyes"])

dnl
dnl  Detect pthreads libraries
dnl

PTHREAD_LIBS=error
AC_CHECK_LIB(pthread, pthread_attr_init, PTHREAD_LIBS="-lpthread")
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_LIB(pthreads, pthread_attr_init, PTHREAD_LIBS="-lpthreads")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_LIB(c_r, pthread_attr_init, PTHREAD_LIBS="-lc_r")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_FUNC(pthread_attr_init, PTHREAD_LIBS="")
fi
AC_SUBST(PTHREAD_LIBS)

dnl
dnl

VALGRIND_ENVIRONMENT=""
ac_enable_valgrind=no
AC_ARG_ENABLE(valgrind-testing,
     [  --enable-valgrind-testing     enable running of tests inside Valgrind ],     [ ac_enable_valgrind=yes ], [ ac_enable_valgrind=no] )

if test "x${ac_enable_valgrind}" = xyes ; then
  if test "x${enable_shared}" = xyes ; then
    VALGRIND_ENVIRONMENT="libtool --mode=execute "
  fi

  AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)
  if test "x$HAVE_VALGRIND" = xyes ; then
    VALGRIND_ENVIRONMENT="$VALGRIND_ENVIRONMENT valgrind -q --leak-check=yes --show-reachable=yes --num-callers=100"
    AC_SUBST(VALGRIND_ENVIRONMENT)
    TESTS_INFO="Type 'make check' to run test suite. Tests will be run under:
  ${VALGRIND_ENVIRONMENT}"
  else
    TESTS_INFO="Type 'make check' to run test suite (Valgrind not found)"
  fi
else  TESTS_INFO="Type 'make check' to run test suite (Valgrind testing not enabled)"
fi
dnl Configuration option to add -Werror to all Makefiles

AC_ARG_ENABLE(gcc-werror,
     AC_HELP_STRING([--enable-gcc-werror], [enable -Werror in all Makefiles]),
     [ ac_enable_gcc_werror=yes ], [ ac_enable_gcc_werror=no] )

if test "x${ac_enable_gcc_werror}" = xyes ; then
  CFLAGS="-Werror $CFLAGS"
fi


# Checks for header files.

AC_CHECK_HEADERS([arpa/inet.h oggz/oggz.h tinyxml.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_PROG_LIBTOOL
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_SUBST(KAD_SUPPORT_UDT)
AC_SUBST(KAD_SUPPORT_TINYXML)
AC_SUBST(KAD_SUPPORT_RTMP)
AC_SUBST(KAD_LINKLIBS)
AC_CHECK_FUNCS([floor gethostbyname gettimeofday localtime_r memchr memmove memset pow select socket sqrt strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol])

AC_OUTPUT(Makefile udt/Makefile tinyxml/Makefile librtmp/Makefile src/Makefile)
