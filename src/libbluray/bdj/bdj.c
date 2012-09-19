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
#include "util/macro.h"
#include "libbluray/bdnav/bdid_parse.h"

#include <jni.h>
#include <stdlib.h>
#include <string.h>

typedef jint (JNICALL * fptr_JNI_CreateJavaVM) (JavaVM **pvm, void **penv,void *args);

void* load_jvm();

BDJAVA* bdj_open(const char *path,
                 struct bluray *bd, struct bd_registers_s *registers,
                 struct indx_root_s *index)
{
    // first load the jvm using dlopen
    void* jvm_lib = load_jvm();

    if (!jvm_lib) {
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Wasn't able to load libjvm.so\n");
        return NULL;
    }

    fptr_JNI_CreateJavaVM JNI_CreateJavaVM_fp = (fptr_JNI_CreateJavaVM)dl_dlsym(jvm_lib, "JNI_CreateJavaVM");

    if (JNI_CreateJavaVM_fp == NULL) {
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Couldn't find symbol JNI_CreateJavaVM.\n");
        return NULL;
    }

    BDJAVA* bdjava = malloc(sizeof(BDJAVA));
    bdjava->bd = bd;
    bdjava->reg = registers;
    bdjava->index = index;
    bdjava->path = path;

    JavaVMInitArgs args;

    // check if overriding the classpath
    const char* classpath = getenv("LIBBLURAY_CP");
    if (classpath == NULL)
        classpath = BDJ_CLASSPATH;

    // determine classpath
    char* classpath_opt = str_printf("-Djava.class.path=%s", classpath);

    // determine bluray.vfs.root
    char* vfs_opt;
    vfs_opt = str_printf("-Dbluray.vfs.root=%s", path);

    JavaVMOption* option = malloc(sizeof(JavaVMOption) * 9);
    int n = 0;
    option[n++].optionString = classpath_opt;
    option[n++].optionString = vfs_opt;

    args.version = JNI_VERSION_1_6;
    args.nOptions = n;
    args.options = option;
    args.ignoreUnrecognized = JNI_FALSE; // don't ignore unrecognized options

    int result = JNI_CreateJavaVM_fp(&bdjava->jvm, (void**) &bdjava->env, &args);
    free(option);
    free(classpath_opt);
    free(vfs_opt);

    if (result != JNI_OK || !bdjava->env) {
        free(bdjava);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to create new Java VM.\n");
        return NULL;
    }


    // initialize class org.videolan.Libbluray
    jclass init_class = (*bdjava->env)->FindClass(bdjava->env, "org/videolan/Libbluray");
    if (!init_class) {
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to locate org.videolan.Libbluray class\n");
        bdj_close(bdjava);
        return NULL;
    }

    char* id_path = str_printf("%s/CERTIFICATE/id.bdmv", path);
    BDID_DATA *id  = bdid_parse(id_path);
    free(id_path);

    jmethodID init_id = (*bdjava->env)->GetStaticMethodID(bdjava->env, init_class,
                                                          "init", "(JLjava/lang/String;)V");
    jlong param_bdjava_ptr = (jlong)(intptr_t) bdjava;
    jstring param_disc_id = (*bdjava->env)->NewStringUTF(bdjava->env,
                                                         id ? id->disc_id : "00000000000000000000000000000000");
    (*bdjava->env)->CallStaticVoidMethod(bdjava->env, init_class, init_id,
                                         param_bdjava_ptr, param_disc_id);
    (*bdjava->env)->DeleteLocalRef(bdjava->env, init_class);
    (*bdjava->env)->DeleteLocalRef(bdjava->env, param_disc_id);

    bdid_free(&id);

    return bdjava;
}

int bdj_start(BDJAVA *bdjava, unsigned title)
{
    JNIEnv* env = bdjava->env;

    jclass loader_class = (*env)->FindClass(env, "org/videolan/BDJLoader");

    if (!loader_class) {
        (*env)->ExceptionDescribe(env);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to locate org.videolan.BDJLoader class\n");
        return BDJ_ERROR;
    }

    jmethodID load_id = (*env)->GetStaticMethodID(env, loader_class,
                                                  "load", "(I)Z");

    if (!load_id) {
        (*env)->ExceptionDescribe(env);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to locate org.videolan.BDJLoader class \"load\" method\n");
        return BDJ_ERROR;
    }

    jboolean status = (*env)->CallStaticBooleanMethod(env, loader_class, load_id, (jint)title);

    return (status == JNI_TRUE) ? BDJ_SUCCESS : BDJ_ERROR;
}

void bdj_stop(BDJAVA *bdjava)
{
    if (!bdjava) {
        return;
    }

    JNIEnv* env = bdjava->env;

    jclass loader_class = (*env)->FindClass(env, "org/videolan/BDJLoader");
    if (!loader_class) {
        (*env)->ExceptionDescribe(env);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to locate org.videolan.BDJLoader class\n");
        return;
    }

    jmethodID unload_id = (*env)->GetStaticMethodID(env, loader_class,
            "unload", "()Z");
    /*jboolean status =*/ (*env)->CallStaticBooleanMethod(env, loader_class, unload_id);
}

void bdj_close(BDJAVA *bdjava)
{
    if (!bdjava) {
        return;
    }

    JNIEnv* env = bdjava->env;

    jclass shutdown_class = (*bdjava->env)->FindClass(bdjava->env, "org/videolan/Libbluray");
    if (!shutdown_class) {
        (*env)->ExceptionDescribe(env);
        BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to locate org.videolan.Libbluray class\n");

    } else {
      jmethodID shutdown_id = (*bdjava->env)->GetStaticMethodID(bdjava->env, shutdown_class,
                                                                "shutdown", "()V");
      (*env)->CallStaticVoidMethod(env, shutdown_class, shutdown_id);
    }

    (*bdjava->jvm)->DestroyJavaVM(bdjava->jvm);

    free(bdjava);
}

void bdj_process_event(BDJAVA *bdjava, unsigned ev, unsigned param)
{
    JNIEnv* env = bdjava->env;

    jclass event_class = (*env)->FindClass(env, "org/videolan/Libbluray");
    if (event_class) {
        jmethodID event_id = (*env)->GetStaticMethodID(env, event_class,
                                                       "processEvent", "(II)V");
        if (event_id) {
            (*env)->CallStaticVoidMethod(env, event_class, event_id, ev, param);
        } else {
            BD_DEBUG(DBG_BDJ | DBG_CRIT, "Failed to locate org.videolan.Libbluray \"processEvent\" method\n");
        }
    }
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
