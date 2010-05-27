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

AH_TEMPLATE([NEED_SYS_ERRLIST_DECLARED],
        [Define this if sys_errlist must be declared (if it exists).])

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
AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([socket], [socket inet])
AC_SEARCH_LIBS([crypt], [crypt])
AC_SEARCH_LIBS([dlopen], [dl])

# libgcrypt
AC_SEARCH_LIBS([gcry_cipher_open], [gcrypt])
AC_ARG_ENABLE(libgcrypt,
  [  --disable-libgcrypt      gcrypt support (default enabled)])
AS_IF([test "${enable_libgcrypt}" != "no"], [
    libgcrypt-config --version >/dev/null || \
        AC_MSG_ERROR([gcrypt.h present but libgcrypt-config could not be found])
    AC_CHECK_LIB(gcrypt, gcry_control, [
      have_libgcrypt="yes"
      GCRYPT_CFLAGS="`libgcrypt-config --cflags`"
      GCRYPT_LIBS="`libgcrypt-config --libs`"
    ], [
      AC_MSG_ERROR([libgcrypt not found. Install libgcrypt or use --disable-libgcrypt. Have a nice day.])
    ], [`libgcrypt-config --libs`])
  ]
)
AM_CONDITIONAL([HAVE_GCRYPT], [test "${have_libgcrypt}" = "yes"])

# required functions
AC_CHECK_FUNC([snprintf],
                [AC_DEFINE(HAVE_SNPRINTF,1,[Presence of snprintf found])],
                [AC_MSG_WARN(Adding internal snprintf function)])
AC_CHECK_FUNC([strcasestr],
                [AC_DEFINE(HAVE_STRCASESTR,1,[Presence of strcasestr found])],
                [])
AC_CHECK_FUNCS([strtoul strtouq strtoull strtoll])



 AC_SUBST(GCRYPT_CFLAGS)
 AC_SUBST(GCRYPT_LIBS)

# List all Makefiles here.
AC_CONFIG_FILES([Makefile src/Makefile src/libaacs/Makefile src/libbdnav/Makefile src/libbdplus/Makefile src/examples/Makefile])
AC_OUTPUT

