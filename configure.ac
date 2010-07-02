# initilization
AC_INIT([libbluray],[0.1], [http://bd.videolan.org/])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
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

AC_ARG_WITH([java],
  [AS_HELP_STRING([--with-java],
  [set the path to the java home (use if $JAVA_HOME is not set)])],
  [with_java=$withval],
  [])

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

AC_ARG_ENABLE([bdjava],
  [AS_HELP_STRING([--enable-bdjava],
  [enable BD-Java support (default is no)])],
  [use_bdjava=$enableval],
  [use_bdjava=no])

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

# use bdjava
if [[ $use_bdjava = "yes" ]]; then
  if [[ -z $with_java ]]; then
    if [[ -z $JAVA_HOME ]]; then
      AC_MSG_ERROR([ No JAVA_HOME and --with-java not set. ])
    else 
      java_path=$JAVA_HOME
    fi
  else
    java_path=$with_java
  fi

  case $target_cpu in
       x86_64) java_arch=amd64 ;;
       i?86)   java_arch=i386 ;;
       *)      java_arch=$target_cpu ;;
  esac
  case $target_os in
       linux*) java_os=linux ;;
       win*)   java_os=win32 ;;
       *)      java_os=$target_os ;;
  esac

  BDJAVA_CFLAGS="-I$java_path/include -I$java_path/include/$java_os"
  BDJAVA_LDFLAGS="-L$java_path/jre/lib/$java_arch -L$java_path/jre/lib/$java_arch/server"
  
  temp_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$BDJAVA_CFLAGS $CPPFLAGS"
  AC_CHECK_HEADERS([jni.h], [], [AC_MSG_ERROR("Coudln't find jni.h")])
  CPPFLAGS=$temp_CPPFLAGS

  temp_LDFLAGS=$LDFLAGS
  LDFLAGS="$BDJAVA_LDFLAGS $LDFLAGS"
  AC_CHECK_LIB([jvm], [JNI_CreateJavaVM], [], [AC_MSG_ERROR("Coudn't find jvm library.")])
  LDFLAGS=$temp_LDFLAGS
  
  AC_DEFINE([USING_BDJAVA], [1], ["Define to 1 if using BD-Java"])
  AC_SUBST(BDJAVA_CFLAGS)
  AC_SUBST(BDJAVA_LDFLAGS)
fi 
AM_CONDITIONAL([USING_BDJAVA], [ test $use_bdjava = "yes" ])

# generate output files
AC_SUBST(DLOPEN_LDFLAGS)
AC_SUBST(SET_WARNINGS)
AC_SUBST(SET_OPTIMIZATIONS)
AC_SUBST(SET_DEBUG_OPTS)
AC_CONFIG_FILES([Makefile src/Makefile src/examples/Makefile src/libbluray.pc])
AC_OUTPUT
