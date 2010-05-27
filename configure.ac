# initilization
AC_INIT([libbluray],[0.1.0], [http://bd.videolan.org/])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)

# messages
library_not_found="Could not find required library!"

# required programs
AC_PROG_CC
AC_PROG_LIBTOOL

# required types
AC_TYPE_SIGNAL

# required headers
AC_HEADER_TIME
AC_CHECK_HEADERS([stdarg.h varargs.h sys/time.h sys/types.h dirent.h sys/file.h sys/param.h sys/statfs.h sys/vfs.h sys/statvfs.h errno.h libgen.h time.h malloc.h stdlib.h mntent.h linux/cdrom.h dlfcn.h])
# mount depends on param
AC_CHECK_HEADERS([sys/mount.h], [], [], [#if HAVE_SYS_PARAM_H
                                         #include <sys/param.h>
                                         #endif])

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
  AC_MSG_ERROR([Version of OpenSSL does not feature openssl/ecdsa.h, please upgrade]) ]
      , [])
AC_SEARCH_LIBS([gcry_cipher_open], [gcrypt])
AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([socket], [socket inet])
AC_SEARCH_LIBS([crypt], [crypt])
AC_SEARCH_LIBS([dlopen], [dl])

# required functions
AC_CHECK_FUNC([snprintf],
                [AC_DEFINE(HAVE_SNPRINTF,1,[Presence of snprintf found])],
                [AC_MSG_WARN(Adding internal snprintf function)])
AC_CHECK_FUNC([strcasestr],
                [AC_DEFINE(HAVE_STRCASESTR,1,[Presence of strcasestr found])],
                [])
AC_CHECK_FUNCS([strtoul strtouq strtoull strtoll])

# List all Makefiles here.
AC_CONFIG_FILES([Makefile src/Makefile src/libaacs/Makefile src/libbdnav/Makefile src/libbdplus/Makefile src/examples/Makefile])
AC_OUTPUT

