
#include "util/logging.h"

#include "bdj_util.h"

jobject bdj_make_object(JNIEnv* env, const char* name, const char* sig, ...)
{
    jclass obj_class = (*env)->FindClass(env, name);
    jmethodID obj_constructor = (*env)->GetMethodID(env, obj_class, "<init>", sig);

    if (!obj_class) {
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Class %s not found\n", name);
        return NULL;
    }

    va_list ap;
    va_start(ap, sig);
    jobject obj = (*env)->NewObjectV(env, obj_class, obj_constructor, ap);
    va_end(ap);

    return obj;
}

jobjectArray bdj_make_array(JNIEnv* env, const char* name, int count)
{
    jclass arr_class = (*env)->FindClass(env, name);
    return (*env)->NewObjectArray(env, count, arr_class, NULL);
}

