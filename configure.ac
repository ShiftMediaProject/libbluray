# initilization
AC_INIT([libbluray],[0.1.0], [http://bd.videolan.org/])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)

# messages
library_not_found="Could not find required library!"
function_not_found="Could not find required function!"

# required programs
AC_PROG_CC
AC_PROG_LIBTOOL

# required types
AC_TYPE_SIGNAL

# required headers
AC_CHECK_HEADERS([stdarg.h sys/types.h dirent.h errno.h libgen.h malloc.h])
AC_CHECK_HEADERS([stdlib.h mntent.h linux/cdrom.h dlfcn.h inttypes.h])
AC_CHECK_HEADERS([sys/time.h time.h])

# required structures
AC_STRUCT_DIRENT_D_TYPE

# required system services
AC_SYS_LARGEFILE

# required libraries
PKG_CHECK_MODULES([OPENSSL], [openssl],
  [INCLUDES="$INCLUDES $OPENSSL_CFLAGS"; LIBS="$LIBS $OPENSSL_LIBS"],
  AC_MSG_ERROR($library_not_found))
# Make sure OpenSSL is cool enough to play with the big boys.
AC_CHECK_HEADERS([openssl/ecdsa.h], [], [
  AC_MSG_ERROR([Version of OpenSSL does not feature openssl/ecdsa.h, please
    upgrade]) ]
      , [])
AC_SEARCH_LIBS([dlopen], [dl])

# required functions
AC_CHECK_FUNC([snprintf],, [AC_MSG_ERROR($function_not_found)])

# libaacs checks
if [[ -d src/libaacs ]]; then
  libaacs_src_available=1
else
  libaacs_src_available=0
fi
AM_CONDITIONAL([HAVE_LIBAACS_SRC], [test $libaacs_src_available = "1"])

# libbdplus checks
if [[ -d src/libbdplus ]]; then
  libbdplus_src_available=1
else
  libbdplus_src_available=0
fi
AM_CONDITIONAL([HAVE_LIBBDPLUS_SRC], [test $libbdplus_src_available = "1"])

# generate output files
AC_CONFIG_FILES([Makefile src/Makefile src/libbdnav/Makefile
  src/examples/Makefile])
AC_OUTPUT
