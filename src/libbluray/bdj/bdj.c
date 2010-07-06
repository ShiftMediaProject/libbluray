/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.s
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "bdj.h"

#include "bdjo_parser.h"
#include "common.h"
#include "libbluray/register.h"

#include <jni.h>
#include <stdlib.h>
#include <string.h>

struct bdjava_s {
    BD_REGISTERS* reg;

    // JNI
    JavaVM* jvm;
    JNIEnv* env;
};

int start_xlet(JNIEnv* env, BDJO_APP_INFO* info, BDJAVA* bdjava);

BDJAVA* bdj_open(const char *path, const char* start, void* registers)
{
    BDJAVA* bdjava = malloc(sizeof(BDJAVA));

    bdjava->reg = registers;

    // determine path of bdjo file to load
    char* bdjo_path = malloc(strlen(path) + strlen(BDJ_BDJO_PATH) + strlen(start) + 7);
    sprintf(bdjo_path, "%s%s/%s.bdjo", path, BDJ_BDJO_PATH, start);

    BDJO* bdjo = bdjo_read(bdjo_path);
    free(bdjo_path);
    

    if (bdjo != NULL && bdjo->app_info_count > 0) {
        // for now just pick the first application in the bdjo
        BDJO_APP_INFO app_info = bdjo->app_info_entries[0];

        JavaVMInitArgs args;

        // check if overriding the classpath
        const char* classpath = getenv("LIBBLURAY_CP");
        if (strlen(classpath) == 0)
            classpath = BDJ_CLASSPATH;

        // determine classpath
        char* classpath_opt = malloc(strlen(path)*2 + strlen(BDJ_BDJO_PATH) +
                strlen(app_info.base_directory) + strlen(BDJ_JAR_PATH) +
                strlen(app_info.classpath_extension) + strlen(classpath) + 30);
        sprintf(classpath_opt, "-Djava.class.path=%s:%s%s/%s.jar:%s%s%s.jar", classpath, path, BDJ_JAR_PATH,
                app_info.base_directory, path, BDJ_JAR_PATH, app_info.classpath_extension);

        JavaVMOption* option = malloc(sizeof(JavaVMOption)*1);
        option[0].optionString = classpath_opt;

        args.version = JNI_VERSION_1_6;
        args.nOptions = 1;
        args.options = option;
        args.ignoreUnrecognized = JNI_FALSE; // don't ignore unrecognized options

        int result = JNI_CreateJavaVM(&bdjava->jvm, (void**)&bdjava->env, &args);
        free(option);
        free(classpath_opt);

        if (result != JNI_OK || start_xlet(bdjava->env, &app_info, bdjava) == BDJ_ERROR) {
            free(bdjava);
            return NULL;
        }

        return bdjava;
    }

    return NULL;
}


void bdj_close(BDJAVA *bdjava)
{
    JNIEnv* env = bdjava->env;
    JavaVM* jvm = bdjava->jvm;

    jclass init_class = (*env)->FindClass(env, "org/videolan/BDJLoader");
    jmethodID shutdown_id = (*env)->GetStaticMethodID(env, init_class, "Shutdown", "()V");
    (*env)->CallStaticVoidMethod(env, init_class, shutdown_id);

    (*jvm)->DestroyJavaVM(jvm);

    free(bdjava);
}

int start_xlet(JNIEnv* env, BDJO_APP_INFO* info, BDJAVA* bdjava)
{
    jclass init_class = (*env)->FindClass(env, "org/videolan/BDJLoader");

    if (init_class == NULL) {
        (*env)->ExceptionDescribe(env);
        return BDJ_ERROR;
    }

    jmethodID load_id = (*env)->GetStaticMethodID(env, init_class, "Load", "(Ljava/lang/String;[Ljava/lang/String;J)V");

    if (load_id == NULL) {
        (*env)->ExceptionDescribe(env);
        return BDJ_ERROR;
    }

    jstring param_init_class = (*env)->NewStringUTF(env, info->initial_class);

    jclass str_class = (*env)->FindClass(env, "java/lang/String");
    jobjectArray param_params = (*env)->NewObjectArray(env, info->param_count, str_class, NULL);

    int i;
    for (i = 0; i < info->param_count; i++) {
        jstring value = (*env)->NewStringUTF(env, info->params[i]);

        (*env)->SetObjectArrayElement(env, param_params, i, value);
    }

    jlong param_bdjava_ptr = (jlong)bdjava;

    (*env)->CallStaticVoidMethod(env, init_class, load_id, param_init_class, param_params, param_bdjava_ptr);

    return BDJ_SUCCESS;
}
