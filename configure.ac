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
  AC_MSG_NOTICE($using_normal_linking)
fi
AM_CONDITIONAL([USING_DLOPEN],
  [test $use_dlopen = "auto" || test $use_dlopen = "yes"])

# libbdplus checks
if [[ -d src/libbdplus ]]; then
  PKG_CHECK_MODULES([OPENSSL], [openssl],
    [OPENSSL_INCLUDES="$INCLUDES $OPENSSL_CFLAGS";
    OPENSSL_LDFLAGS="$LIBS $OPENSSL_LIBS"],
    AC_MSG_ERROR($library_not_found))
  # Make sure OpenSSL is cool enough to play with the big boys.
  AC_CHECK_HEADERS([openssl/ecdsa.h], [], [
    AC_MSG_ERROR([Version of OpenSSL does not feature openssl/ecdsa.h, please
      upgrade]) ]
        , [])
  libbdplus_src_available=1
else
  libbdplus_src_available=0
fi
AM_CONDITIONAL([HAVE_LIBBDPLUS_SRC], [test $libbdplus_src_available = "1"])

# generate output files
AC_SUBST(OPENSSL_INCLUDES)
AC_SUBST(OPENSSL_LDFLAGS)
AC_SUBST(DLOPEN_LDFLAGS)
AC_CONFIG_FILES([Makefile src/Makefile src/libbdnav/Makefile
  src/examples/Makefile src/libbluray.pc])
AC_OUTPUT
