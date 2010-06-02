# initilization
AC_INIT([libbluray],[0.1], [http://bd.videolan.org/])
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
  [set warnings as errors via -Werror (default is no)])],
  [use_werror=$enableval],
  [use_werror=no])

AC_ARG_ENABLE([extra-warnings],
  [AS_HELP_STRING([--enable-extra-warnings],
  [set extra warnings (default is yes)])],
  [use_extra_warnings=$enableval],
  [use_extra_warnings=yes])

AC_ARG_ENABLE([optimizations],
  [AS_HELP_STRING([--enable-optimizations],
  [enable optimizations (default is yes)])],
  [use_optimizations=$enableval],
  [use_optimizations=yes])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
  [enable debugging information (default is yes)])],
  [use_debug=$enableval],
  [use_debug=yes])

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

# function testing for supported compiler options
check_cc_options()
{
  local tmpfile=$(mktemp XXXXXXXXXX)
  local tmpcfile="$tmpfile.c"
  mv $tmpfile $tmpcfile
  local retval=1
  printf "Checking if compiler supports $@... "
  if $CC -c $@ $tmpcfile -o /dev/null >/dev/null 2>&1; then
    local retval=0
  fi
  rm $tmpcfile
  if [[ $retval = "0" ]]; then
    echo "yes"
  else
    echo "no"
  fi
  return $retval
}

# set default warnings if supported
check_cc_options -Wall && \
  SET_WARNINGS="$SET_WARNINGS -Wall"
check_cc_options -Wdisabled-optimization && \
  SET_WARNINGS="$SET_WARNINGS -Wdisabled-optimization"
check_cc_options -Wpointer-arith && \
  SET_WARNINGS="$SET_WARNINGS -Wpointer-arith"
check_cc_options -Wredundant-decls && \
  SET_WARNINGS="$SET_WARNINGS -Wredundant-decls"
check_cc_options -Wcast-qual && \
  SET_WARNINGS="$SET_WARNINGS -Wcast-qual"
check_cc_options -Wwrite-strings && \
  SET_WARNINGS="$SET_WARNINGS -Wwrite-strings"
check_cc_options -Wtype-limits && \
  SET_WARNINGS="$SET_WARNINGS -Wtype-limits"
check_cc_options -Wundef && \
  SET_WARNINGS="$SET_WARNINGS -Wundef"

# use -Werror
if [[ $use_werror = "yes" ]]; then
  check_cc_options -Werror && \
    SET_WARNINGS="$SET_WARNINGS -Werror"
fi

# use extra warnings
if [[ $use_extra_warnings = "yes" ]]; then
  check_cc_options -Wextra && \
    SET_WARNINGS="$SET_WARNINGS -Wextra"
  check_cc_options -Winline && \
    SET_WARNINGS="$SET_WARNINGS -Winline"
fi

# use optimizations
if [[ $use_optimizations = "yes" ]]; then
  check_cc_options -O3 && \
    SET_OPTIMIZATIONS="$SET_OPTIMIZATIONS -O3"
  check_cc_options -fomit-frame-pointer && \
    SET_OPTIMIZATIONS="$SET_OPTIMIZATIONS -fomit-frame-pointer"
fi

# use debug
if [[ $use_debug = "yes" ]]; then
  check_cc_options -g && \
    SET_DEBUG_OPTS="$SET_DEBUG_OPTS -g"
fi

# generate output files
AC_SUBST(DLOPEN_LDFLAGS)
AC_SUBST(SET_WARNINGS)
AC_SUBST(SET_OPTIMIZATIONS)
AC_SUBST(SET_DEBUG_OPTS)
AC_CONFIG_FILES([Makefile src/Makefile src/examples/Makefile src/libbluray.pc])
AC_OUTPUT
