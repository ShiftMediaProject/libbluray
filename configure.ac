# initilization
AC_INIT([libbluray],[0.2], [http://bd.videolan.org/])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)

case "${host_os}" in
  "")
    SYS=unknown
    ;;
  *mingw32* | *cygwin* | *wince* | *mingwce*)
    case "${host_os}" in
      *wince* | *mingwce* | *mingw32ce*)
        SYS=mingwce
        ;;
      *mingw32*)
        SYS=mingw32
        AC_DEFINE([_WIN32_WINNT], 0x0501, [Define to '0x0500' for Windows XP APIs.])
        ;;
    esac
    ;;
  *)
    SYS="${host_os}"
    ;;
esac

# messages
library_not_found="Could not find required library!"
function_not_found="Could not find required function!"
using_dlopen_crypto_libs="Using libaacs and libbdplus via dlopen."
using_normal_linking="Using libaacs and libbdplus via normal linking."
libxml2_missing="Could not find libxml2 - disclib meta parser disabled."
pkg_config_missing="Could not find pkg-config.  Assuming..."

# configure options
AC_ARG_WITH([dlopen-crypto-libs],
  [AS_HELP_STRING([--with-dlopen-crypto-libs],
  [use libaacs and libbdplus via dlopen (default is auto)])],
  [use_dlopen_crypto_libs=$withval],
  [use_dlopen_crypto_libs=auto])

AC_ARG_WITH([jdk],
  AS_HELP_STRING([--with-jdk=DIR],
  [Specify the path to the JDK (default is "/usr/lib/jvm/java-6-openjdk")]),
  [JDK_HOME=$withval],
  [
    # Set JDK_HOME unless already set in environment
    if [[ -z "$JDK_HOME" ]]; then
      JDK_HOME="/usr/lib/jvm/java-6-openjdk"
    fi
  ])

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

AC_ARG_ENABLE([examples],
  [AS_HELP_STRING([--enable-examples],
  [build examples (default is yes)])],
  [use_examples=$enableval],
  [use_examples=yes])

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
AC_CHECK_HEADERS([stdlib.h mntent.h linux/cdrom.h inttypes.h strings.h])
AC_CHECK_HEADERS([sys/time.h time.h])
if test "${SYS}" != "mingw32" ; then
    AC_CHECK_HEADERS(pthread.h,, [AC_MSG_ERROR([pthread.h required])])
fi

# required structures
AC_STRUCT_DIRENT_D_TYPE

# required system services
AC_SYS_LARGEFILE

# required functions
AC_CHECK_FUNC([snprintf],, [AC_MSG_ERROR($function_not_found)])

# required libraries
# pthreads (not on win32)
if test "${SYS}" != "mingw32" ; then
    AC_CHECK_LIB(pthread, pthread_create, [LIBS="$LIBS -lpthread"])
fi

# dlopen check
if [[ $use_dlopen_crypto_libs = "auto" ]]; then
  if test "${SYS}" != "mingw32" ; then
      AC_CHECK_LIB([dl], [dlopen],
        [DLOPEN_LDFLAGS="-ldl"; AC_MSG_NOTICE($using_dlopen_crypto_libs)
         AC_DEFINE([DLOPEN_CRYPTO_LIBS], [1], ["Define to 1 if dlopening crypto libs"])],
        [use_dlopen_crypto_libs="no"; AC_MSG_NOTICE($using_normal_linking)])
  else
         AC_DEFINE([DLOPEN_CRYPTO_LIBS], [1], ["Define to 1 if dlopening crypto libs"])
  fi
elif [[ $use_dlopen_crypto_libs = "yes" ]]; then
  AC_CHECK_LIB([dl], [dlopen],
    [DLOPEN_LDFLAGS="-ldl"; AC_MSG_NOTICE($using_dlopen_crypto_libs)
     AC_DEFINE([DLOPEN_CRYPTO_LIBS], [1], ["Define to 1 if dlopening crypto libs"])],
    [AC_MSG_ERROR($library_not_found)])
else
  AC_CHECK_LIB([aacs], [aacs_open],,
    [AC_MSG_ERROR($library_not_found)])
  AC_CHECK_LIB([bdplus], [bdplus_init],,
    [AC_MSG_ERROR($library_not_found)])
  AC_MSG_NOTICE($using_normal_linking)
fi
AM_CONDITIONAL([DLOPEN_CRYPTO_LIBS],
  [test $use_dlopen_crypto_libs = "auto" || test $use_dlopen_crypto_libs = "yes"])

# check for pkg-config itself so we don't try the m4 macro without pkg-config
AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, yes)

# libxml2 for metadata parser
if test "x$HAVE_PKG_CONFIG" = "xyes"
then
    PKG_CHECK_MODULES([LIBXML2], libxml-2.0 >= 2.6, 
        [AC_DEFINE([HAVE_LIBXML2], [1], 
            [Define to 1 if libxml2 is to be used for metadata parsing])],
        [AC_MSG_NOTICE($libxml2_missing)])
else
    AC_MSG_NOTICE($pkg_config_missing $libxml2_missing)
fi

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

# use examples
AM_CONDITIONAL([USING_EXAMPLES], [ test $use_examples = "yes" ])

# use bdjava
if [[ $use_bdjava = "yes" ]]; then
  case $target_cpu in
       x86_64) java_arch=amd64 ;;
       i?86)   java_arch=i386 ;;
       *)      java_arch=$target_cpu ;;
  esac
  case $target_os in
       linux*)   java_os=linux ;;
       win*)     java_os=win32 ;;
       freebsd*) java_os=freebsd ;;
       *)        java_os=$target_os ;;
  esac

  BDJAVA_CFLAGS="-I${JDK_HOME}/include -I${JDK_HOME}/include/$java_os"

  temp_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$BDJAVA_CFLAGS $CPPFLAGS"
  AC_CHECK_HEADERS([jni.h], [], [AC_MSG_ERROR("Could not find jni.h")])
  CPPFLAGS="$temp_CPPFLAGS"

  AC_DEFINE([USING_BDJAVA], [1], ["Define to 1 if using BD-Java"])
  AC_DEFINE_UNQUOTED([JAVA_ARCH], ["$java_arch"], ["Defines the architecture of the java vm."])
fi
AM_CONDITIONAL([USING_BDJAVA], [ test $use_bdjava = "yes" ])

# generate documentation
DX_INIT_DOXYGEN(libbluray, doc/doxygen-config, [doc/doxygen])

# generate output files
AC_SUBST(BDJAVA_CFLAGS)
AC_SUBST(DLOPEN_LDFLAGS)
AC_SUBST(SET_WARNINGS)
AC_SUBST(SET_OPTIMIZATIONS)
AC_SUBST(SET_DEBUG_OPTS)
AC_CONFIG_FILES([Makefile src/Makefile src/examples/Makefile src/libbluray.pc])
AC_OUTPUT
