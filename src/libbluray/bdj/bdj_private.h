#ifndef BDJ_PRIVATE_H_
#define BDJ_PRIVATE_H_

#include "libbluray/register.h"
#include "libbluray/bluray.h"
#include "libbluray/bdnav/index_parse.h"
#include <jni.h>

struct bdjava_s {
    BLURAY       *bd;
    BD_REGISTERS *reg;
    INDX_ROOT    *index;

    // JNI
    JavaVM* jvm;
    JNIEnv* env;

    const char *path;
};

#endif
