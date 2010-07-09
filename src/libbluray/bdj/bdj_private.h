#ifndef BDJ_PRIVATE_H_
#define BDJ_PRIVATE_H_

#include "libbluray/register.h"
#include "libbluray/bluray.h"
#include <jni.h>

struct bdjava_s {
    BD_REGISTERS* reg;
    BLURAY* bd;

    // JNI
    JavaVM* jvm;
    JNIEnv* env;
};

#endif
