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

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include "bdj_private.h"
#include "bdjo_parser.h"
#include "common.h"
#include "libbluray/register.h"
#include "file/dl.h"
#include "util/strutl.h"

#include <jni.h>
#include <stdlib.h>
#include <string.h>

typedef jint (JNICALL * fptr_JNI_CreateJavaVM) (JavaVM **pvm, void **penv,void *args);

int start_xlet(JNIEnv* env, const char* path, jobject bdjo, BDJAVA* bdjava);
void* load_jvm();

BDJAVA* bdj_open(const char *path, const char* start, void* bd, void* registers)
{
    // first load the jvm using dlopen
    void* jvm_lib = load_jvm();

    if (!jvm_lib) {
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Wasn't able to load libjvm.so\n");
        return NULL;
    }

    BDJAVA* bdjava = malloc(sizeof(BDJAVA));
    bdjava->bd = bd;
    bdjava->reg = registers;

    JavaVMInitArgs args;

    // check if overriding the classpath
    const char* classpath = getenv("LIBBLURAY_CP");
    if (classpath == NULL)
        classpath = BDJ_CLASSPATH;

    // determine classpath
    char* classpath_opt = str_printf("-Djava.class.path=%s", classpath);

    JavaVMOption* option = malloc(sizeof(JavaVMOption) * 1);
    option[0].optionString = classpath_opt;

    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;
    args.options = option;
    args.ignoreUnrecognized = JNI_FALSE; // don't ignore unrecognized options

    fptr_JNI_CreateJavaVM JNI_CreateJavaVM_fp = (fptr_JNI_CreateJavaVM)dl_dlsym(jvm_lib,
            "JNI_CreateJavaVM");

    if (JNI_CreateJavaVM_fp == NULL) {
        free(bdjava);
        free(option);
        free(classpath_opt);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Couldn't find symbol JNI_CreateJavaVM.\n");
        return NULL;
    }

    int result = JNI_CreateJavaVM_fp(&bdjava->jvm, (void**) &bdjava->env, &args);
    free(option);
    free(classpath_opt);

    if (result != JNI_OK || !bdjava->env) {
        free(bdjava);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to create new Java VM.\n");
        return NULL;
    }

    // determine path of bdjo file to load
    char* bdjo_path = str_printf("%s%s/%s.bdjo", path, BDJ_BDJO_PATH, start);
    jobject bdjo = bdjo_read(bdjava->env, bdjo_path);
    free(bdjo_path);

    if (!bdjo) {
        free(bdjava);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to load BDJO file.\n");
        return NULL;
    }

    if (start_xlet(bdjava->env, path, bdjo, bdjava) == BDJ_ERROR) {
        free(bdjava);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to start BDJ program.\n");
        return NULL;
    }

    return bdjava;
}

void bdj_close(BDJAVA *bdjava)
{
    JNIEnv* env = bdjava->env;
    JavaVM* jvm = bdjava->jvm;

    jclass init_class = (*env)->FindClass(env, "org/videolan/BDJLoader");
    jmethodID shutdown_id = (*env)->GetStaticMethodID(env, init_class,
            "Shutdown", "()V");
    (*env)->CallStaticVoidMethod(env, init_class, shutdown_id);

    (*jvm)->DestroyJavaVM(jvm);

    free(bdjava);
}

void bdj_send_event(BDJAVA *bdjava, int type, int keyCode)
{
	JNIEnv* env = bdjava->env;

	jclass init_class = (*env)->FindClass(env, "org/videolan/BDJLoader");
	jmethodID send_key_event_id = (*env)->GetStaticMethodID(env, init_class,
	            "SendKeyEvent", "(II)V");
	(*env)->CallStaticVoidMethod(env, init_class, send_key_event_id, type, keyCode);
}

int start_xlet(JNIEnv* env, const char* path, jobject bdjo, BDJAVA* bdjava)
{
    jclass init_class = (*env)->FindClass(env, "org/videolan/BDJLoader");

    if (init_class == NULL) {
        (*env)->ExceptionDescribe(env);
        return BDJ_ERROR;
    }

    jmethodID load_id = (*env)->GetStaticMethodID(env, init_class, "Load",
            "(Ljava/lang/String;Lorg/videolan/bdjo/Bdjo;J)V");

    if (load_id == NULL) {
        (*env)->ExceptionDescribe(env);
        return BDJ_ERROR;
    }

    jstring param_base_dir = (*env)->NewStringUTF(env, path);
    jlong param_bdjava_ptr = (jlong) bdjava;

    (*env)->CallStaticVoidMethod(env, init_class, load_id, param_base_dir, bdjo,
            param_bdjava_ptr);

    return BDJ_SUCCESS;
}

void* load_jvm()
{
    const char* java_home = getenv("JAVA_HOME"); // FIXME: should probably search multiple directories
    if (java_home == NULL) {
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "JAVA_HOME not set, can't find Java VM.\n");
        return NULL;
    }

#ifdef WIN32
    char* path = str_printf("%s/jre/bin/server/jvm", java_home);
#else	//	#ifdef WIN32
    char* path = str_printf("%s/jre/lib/%s/server/libjvm", java_home, JAVA_ARCH);
#endif	//	#ifdef WIN32

    return dl_dlopen(path, NULL);
}
