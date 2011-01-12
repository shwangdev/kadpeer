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

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/param.h sys/socket.h sys/time.h unistd.h])

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

AC_OUTPUT(Makefile udt/Makefile tinyxml/Makefile librtmp/Makefile madmedia/Makefile src/Makefile)
