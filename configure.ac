# initilization
AC_INIT([libbluray],[0.1.0], [http://bd.videolan.org/])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)

# messages
library_not_found="Could not find required library!"
function_not_found="Could not find required function!"
using_dlopen="Using libaacs and libbdplus via dlopen."
using_normal_linking="Using libaacs and libbdplus via normal linking."

# configure options
AC_ARG_WITH([dlopen],
  [AS_HELP_STRING([--with-dlopen],
  [use libaacs and libbdplus via dlopen (default is auto)])],
  [use_dlopen=$withval],
  [use_dlopen=auto])

AC_ARG_ENABLE([werror],
  [AS_HELP_STRING([--enable-werror],
  [set warnings as errors via -Werror (default is yes)])],
  [use_werror=$enableval],
  [use_werror=yes])

AC_ARG_ENABLE([optimizations],
  [AS_HELP_STRING([--enable-optimizations],
  [enable optimizations (default is yes)])],
  [use_optimizations=$enableval],
  [use_optimizations=yes])

# required programs
AC_PROG_CC
AC_PROG_LIBTOOL

# required types
AC_TYPE_SIGNAL

# required headers
AC_CHECK_HEADERS([stdarg.h sys/types.h dirent.h errno.h libgen.h malloc.h])
AC_CHECK_HEADERS([stdlib.h mntent.h linux/cdrom.h inttypes.h])
AC_CHECK_HEADERS([sys/time.h time.h])

# required structures
AC_STRUCT_DIRENT_D_TYPE

# required system services
AC_SYS_LARGEFILE

# required functions
AC_CHECK_FUNC([snprintf],, [AC_MSG_ERROR($function_not_found)])

# dlopen check
if [[ $use_dlopen = "auto" ]]; then
  AC_CHECK_LIB([dl], [dlopen],
    [DLOPEN_LDFLAGS="-ldl"; AC_MSG_NOTICE($using_dlopen)
     AC_DEFINE([USING_DLOPEN], [1], ["Define to 1 if using dlopen"])],
    [use_dlopen="no"; AC_MSG_NOTICE($using_normal_linking)])
elif [[ $use_dlopen = "yes" ]]; then
  AC_CHECK_LIB([dl], [dlopen],
    [DLOPEN_LDFLAGS="-ldl"; AC_MSG_NOTICE($using_dlopen)
     AC_DEFINE([USING_DLOPEN], [1], ["Define to 1 if using dlopen"])],
    [AC_MSG_ERROR($library_not_found)])
else
  AC_CHECK_LIB([aacs], [aacs_open],,
    [AC_MSG_ERROR($library_not_found)])
  AC_CHECK_LIB([bdplus], [bdplus_init],,
    [AC_MSG_ERROR($library_not_found)])
  AC_MSG_NOTICE($using_normal_linking)
fi
AM_CONDITIONAL([USING_DLOPEN],
  [test $use_dlopen = "auto" || test $use_dlopen = "yes"])

# use -Werror
if [[ $use_werror = "yes" ]]; then
  WERROR="-Werror"
fi

# use optimizations
if [[ $use_optimizations = "yes" ]]; then
  OPTIMIZATIONS="-O3 -fomit-frame-pointer"
fi

# generate output files
AC_SUBST(DLOPEN_LDFLAGS)
AC_SUBST(WERROR)
AC_SUBST(OPTIMIZATIONS)
AC_CONFIG_FILES([Makefile src/Makefile src/examples/Makefile src/libbluray.pc])
AC_OUTPUT
