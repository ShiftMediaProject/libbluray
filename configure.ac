dnl library version number
m4_define([bluray_major], 0)
m4_define([bluray_minor], 3)
m4_define([bluray_micro], 0)
m4_define([bluray_version],[bluray_major.bluray_minor.bluray_micro])

dnl shared library version (.so version)
dnl
dnl - If there are no interface changes, increase revision.
dnl - If interfaces have been added, increase current and age. Set revision to 0.
dnl - If interfaces have been changed or removed, increase current and set age and revision to 0.
dnl
dnl Library file name will be libbluray.so.(current-age).age.revision
dnl
m4_define([lt_current],  4)
m4_define([lt_revision], 0)
m4_define([lt_age],      3)

dnl initilization
AC_INIT([libbluray], bluray_version, [http://www.videolan.org/developers/libbluray.html])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([foreign tar-ustar dist-bzip2 no-dist-gzip])
AC_CONFIG_HEADERS(config.h)

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
  *darwin*)
    SYS=darwin
    ;;
  *)
    SYS="${host_os}"
    ;;
esac

AM_CONDITIONAL(HAVE_WIN32,   test "${SYS}" = "mingw32")
AM_CONDITIONAL(HAVE_DARWIN,  test "${SYS}" = "darwin")

dnl messages
library_not_found="Could not find required library!"
function_not_found="Could not find required function!"

dnl configure options
AC_ARG_VAR([JDK_HOME], [Path to the JDK @<:@default=/usr/lib/jvm/java-6-openjdk@:>@])
AS_IF([test -z "$JDK_HOME"], [JDK_HOME="/usr/lib/jvm/java-6-openjdk"])

AC_ARG_ENABLE([werror],
  [AS_HELP_STRING([--enable-werror], [set warnings as errors via -Werror @<:@default=disabled@:>@])])

AC_ARG_ENABLE([extra-warnings],
  [AS_HELP_STRING([--disable-extra-warnings], [set extra warnings @<:@default=enabled@:>@])])

AC_ARG_ENABLE([optimizations],
  [AS_HELP_STRING([--disable-optimizations], [disable optimizations @<:@default=enabled@:>@])])

AC_ARG_ENABLE([examples],
  [AS_HELP_STRING([--enable-examples],
  [build examples (default is yes)])],
  [use_examples=$enableval],
  [use_examples=yes])

AC_ARG_ENABLE([bdjava],
  [AS_HELP_STRING([--enable-bdjava],
  [enable BD-Java support (default is no)])],
  [use_bdjava=$enableval],
  [use_bdjava=no])

AC_ARG_WITH([libxml2],
  [AS_HELP_STRING([--without-libxml2], [build without libxml2 support @<:@default=with@:>@])])

AC_ARG_WITH([freetype],
  [AS_HELP_STRING([--without-freetype], [build without freetype support @<:@default=with@:>@])])

AC_ARG_WITH([bdj-type],
  [AS_HELP_STRING([--with-bdj-type=TYPE],
  [Specify the type of BD-J implementation (j2se, j2me). Default is j2se.])],
  [BDJ_TYPE=$withval],
  [BDJ_TYPE=j2se])

AC_ARG_WITH([bdj-bootclasspath],
  [AS_HELP_STRING([--with-bdj-bootclasspath=PATH],
  [Specify BD-J build-time bootclass path (example /opt/PhoneME/lib/btclasses.zip;/opt/PhoneME/lib/xmlparer.jar). Default is empty.])],
  [BDJ_BOOTCLASSPATH=$withval],
  [BDJ_BOOTCLASSPATH=])

dnl required programs
AC_PROG_CC
AC_PROG_LIBTOOL

dnl required types
AC_TYPE_SIGNAL

dnl required headers
AC_CHECK_HEADERS([stdarg.h sys/types.h dirent.h errno.h libgen.h malloc.h])
AC_CHECK_HEADERS([stdlib.h mntent.h linux/cdrom.h inttypes.h strings.h])
AC_CHECK_HEADERS([sys/time.h time.h mntent.h])

dnl required structures
AC_STRUCT_DIRENT_D_TYPE

dnl required system services
AC_SYS_LARGEFILE

dnl required functions
AC_CHECK_FUNC([snprintf],, [AC_MSG_ERROR($function_not_found)])

dnl required libraries
dnl pthreads (not on win32)
AS_IF([test "${SYS}" != "mingw32"], [
  AC_CHECK_HEADERS([pthread.h], ,
    [AC_MSG_ERROR([pthread required])])
  AC_SEARCH_LIBS([pthread_create], [pthread], ,
    [AC_MSG_ERROR([pthread required])])
  AC_SEARCH_LIBS([dlopen], [dl])
  DLOPEN_LIBS="$ac_cv_search_dlopen"
  AS_CASE([$DLOPEN_LIBS],
    [no|none\ required], [DLOPEN_LIBS=""])
  AC_SUBST([DLOPEN_LIBS])
])

dnl libxml2 for metadata parser
AS_IF([test "x$with_libxml2" != "xno"], [
  PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 >= 2.6],
    [with_libxml2=yes; AC_DEFINE([HAVE_LIBXML2], [1],
        [Define to 1 if libxml2 is to be used for metadata parsing])])
])

dnl FreeType2
AS_IF([test x"$use_bdjava" != x"no" -a x"$with_freetype" != x"no"], [
  PKG_CHECK_MODULES([FT2], [freetype2],
    [with_freetype=yes; AC_DEFINE([HAVE_FT2], 1, [Define this if you have FreeType2 library])])
])

CC_CHECK_CFLAGS_APPEND([-Wall -Wdisabled-optimization -Wpointer-arith ]dnl
[-Wredundant-decls -Wcast-qual -Wwrite-strings -Wtype-limits -Wundef ]dnl
[-Wmissing-prototypes -Wshadow])

CC_CHECK_CFLAGS_APPEND([-Werror=implicit-function-declaration ]dnl
                       [-Werror-implicit-function-declaration],
                       [break;])

CC_CHECK_WERROR
AS_IF([test "x$enable_werror" = "xyes"], [
  CFLAGS="${CFLAGS} $cc_cv_werror"
])

AS_IF([test "x$enable_extra_warnings" != "xno"], [
  CC_CHECK_CFLAGS_APPEND([-Wextra -Winline])
])

AS_IF([test "x$enable_optimizations" != "xno"], [
  CC_CHECK_CFLAGS_APPEND([-O3 -fomit-frame-pointer])
])

dnl use examples
AM_CONDITIONAL([USING_EXAMPLES], [ test $use_examples = "yes" ])

dnl use bdjava
if [[ $use_bdjava = "yes" ]]; then
  case $host_cpu in
       x86_64) java_arch=amd64 ;;
       i?86)   java_arch=i386 ;;
       *)      java_arch=$host_cpu ;;
  esac
  case $host_os in
       linux*)   java_os=linux ;;
       win*)     java_os=win32 ;;
       freebsd*) java_os=freebsd ;;
       *)        java_os=$host_os ;;
  esac

  BDJAVA_CFLAGS="-I${JDK_HOME}/include -I${JDK_HOME}/include/$java_os"

  temp_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$BDJAVA_CFLAGS $CPPFLAGS"
  AC_CHECK_HEADERS([jni.h], [], [AC_MSG_ERROR("Could not find jni.h")])
  CPPFLAGS="$temp_CPPFLAGS"

  AC_DEFINE([USING_BDJAVA], [1], ["Define to 1 if using BD-Java"])
  AC_DEFINE_UNQUOTED([JAVA_ARCH], ["$java_arch"], ["Defines the architecture of the java vm."])
  AC_DEFINE_UNQUOTED([JDK_HOME], ["$JDK_HOME"], [""])
fi
AM_CONDITIONAL([USING_BDJAVA], [ test $use_bdjava = "yes" ])

dnl BD-J type
if test "$BDJ_TYPE" = "j2me"; then
  AC_DEFINE([HAVE_BDJ_J2ME], [1], [Define to 1 if using libbluray J2ME stack])
else
  BDJ_TYPE=j2se
fi
AC_SUBST(BDJ_TYPE)
dnl bootclasspath
AC_SUBST(BDJ_BOOTCLASSPATH)

dnl generate documentation
DX_INIT_DOXYGEN(libbluray, doc/doxygen-config, [doc/doxygen])

dnl export library version number
BLURAY_VERSION_MAJOR=bluray_major()
AC_SUBST(BLURAY_VERSION_MAJOR)
BLURAY_VERSION_MINOR=bluray_minor()
AC_SUBST(BLURAY_VERSION_MINOR)
BLURAY_VERSION_MICRO=bluray_micro()
AC_SUBST(BLURAY_VERSION_MICRO)

dnl export library (.so) version
LT_VERSION_INFO="lt_current():lt_revision():lt_age()"
AC_SUBST(LT_VERSION_INFO)

dnl generate output files
AC_SUBST(BDJAVA_CFLAGS)
AC_CONFIG_FILES([Makefile src/Makefile src/examples/Makefile src/libbluray.pc src/libbluray/bluray-version.h])
AC_OUTPUT


dnl ---------------------------------------------
dnl Some infos:
dnl ---------------------------------------------

echo "  Summary:"
echo "  --------"
echo "  BD-J support:                  $use_bdjava"
if [[ $use_bdjava = "yes" ]]; then
echo "  BD-J font support (freetype2): $with_freetype"
echo "  BD-J type:                     $BDJ_TYPE"
if test x"$BDJ_BOOTCLASSPATH" != x""; then
echo "  BD-J bootclasspath:            $BDJ_BOOTCLASSPATH"
fi
fi
echo "  Metadata support (libxml2):    $with_libxml2"
echo "  Build examples:                $use_examples"

