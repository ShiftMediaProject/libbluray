/* Define to 1 if libudfread is to be used for disc image access */
/* #undef ENABLE_UDF */

/* Define to 1 if using libbluray J2ME stack */
/* #undef HAVE_BDJ_J2ME */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'. */
/* #undef HAVE_DIRENT_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define this if you have fontconfig library */
#define HAVE_FONTCONFIG 1

/* Define this if you have FreeType2 library */
#define HAVE_FT2 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <jni.h> header file. */
#define HAVE_JNI_H 1

/* Define to 1 if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define to 1 if libxml2 is to be used for metadata parsing */
#define HAVE_LIBXML2 1

/* Define to 1 if you have the <linux/cdrom.h> header file. */
/* #undef HAVE_LINUX_CDROM_H */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <mntent.h> header file. */
/* #undef HAVE_MNTENT_H */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <pthread.h> header file. */
/* #undef HAVE_PTHREAD_H */

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if `d_type' is a member of `struct dirent'. */
/* #undef HAVE_STRUCT_DIRENT_D_TYPE */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'. */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* "Defines the architecture of the java vm." */
#if defined(__x86_64) || defined(_M_X64)
#   define JAVA_ARCH "amd64"
#else
#   define JAVA_ARCH "i386"
#endif

/* "" */
#define JDK_HOME ""

#include "version.h"

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* "Define to 1 if using BD-Java" */
#define USING_BDJAVA 1
