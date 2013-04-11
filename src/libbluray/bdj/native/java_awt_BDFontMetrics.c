/*
 * This file is part of libbluray
 * Copyright (C) 2012  libbluray
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <jni.h>

#include "util/logging.h"

#ifdef HAVE_FT2
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#include "java_awt_BDFontMetrics.h"

/* Disable some warnings */
#if defined __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ifdef __cplusplus
#define CPP_EXTERN extern
#else
#define CPP_EXTERN
#endif

JNIEXPORT jlong JNICALL
Java_java_awt_BDFontMetrics_initN(JNIEnv * env, jclass cls)
{
#ifdef HAVE_FT2
    FT_Library ftLib;
    if (!FT_Init_FreeType(&ftLib)) {
        return (jlong)(intptr_t)ftLib;
    }
    BD_DEBUG(DBG_BDJ | DBG_CRIT, "Loading FreeType2 failed\n");
#else
    BD_DEBUG(DBG_BDJ | DBG_CRIT, "BD-J font support not compiled in\n");
#endif
    return 0;
}

JNIEXPORT void JNICALL
Java_java_awt_BDFontMetrics_destroyN(JNIEnv * env, jclass cls, jlong ftLib)
{
#ifdef HAVE_FT2
    FT_Library lib = (FT_Library)(intptr_t)ftLib;

    if (!lib) {
        return;
    }

    FT_Done_FreeType(lib);
#endif
}

JNIEXPORT jlong JNICALL
Java_java_awt_BDFontMetrics_loadFontN(JNIEnv * env, jobject obj, jlong ftLib, jstring fontName, jint size)
{
#ifdef HAVE_FT2
    const char *name;
    FT_Face ftFace;
    FT_Error result;
    jclass cls;
    jfieldID fid;
    FT_Library lib = (FT_Library)(intptr_t)ftLib;

    if (!lib) {
        return 0;
    }

    name = (*env)->GetStringUTFChars(env, fontName, NULL);
    result = FT_New_Face(lib, name, 0, &ftFace);
    (*env)->ReleaseStringUTFChars(env, fontName, name);
    if (result)
        return 0;
    FT_Set_Char_Size(ftFace, 0, size << 6, 0, 0);

    cls = (*env)->GetObjectClass(env, obj);
    fid = (*env)->GetFieldID(env, cls, "ascent", "I");
    (*env)->SetIntField (env, obj, fid, ftFace->size->metrics.ascender >> 6);
    fid = (*env)->GetFieldID(env, cls, "descent", "I");
    (*env)->SetIntField (env, obj, fid, -ftFace->size->metrics.descender >> 6);
    fid = (*env)->GetFieldID(env, cls, "leading", "I");
    (*env)->SetIntField (env, obj, fid, (ftFace->size->metrics.height - ftFace->size->metrics.ascender + ftFace->size->metrics.descender) >> 6);
    fid = (*env)->GetFieldID(env, cls, "maxAdvance", "I");
    (*env)->SetIntField (env, obj, fid, ftFace->size->metrics.max_advance >> 6);

    return (jlong)(intptr_t)ftFace;
#else  /* HAVE_FT2 */
    return 0;
#endif /* HAVE_FT2 */
}

JNIEXPORT void JNICALL
Java_java_awt_BDFontMetrics_destroyFontN(JNIEnv *env, jobject obj, jlong ftFace)
{
#ifdef HAVE_FT2
    FT_Face face = (FT_Face)(intptr_t)ftFace;

    if (!face) {
        return;
    }

    FT_Done_Face(face);
#endif
}

JNIEXPORT jint JNICALL
Java_java_awt_BDFontMetrics_charWidthN(JNIEnv * env, jobject obj, jlong ftFace, jchar c)
{
#ifdef HAVE_FT2
    FT_Face face = (FT_Face)(intptr_t)ftFace;

    if (!face) {
        return 0;
    }

    if (FT_Load_Char(face, c, FT_LOAD_DEFAULT))
        return 0;
    return face->glyph->metrics.horiAdvance >> 6;
#else
    return 0;
#endif
}

JNIEXPORT jint JNICALL
Java_java_awt_BDFontMetrics_stringWidthN(JNIEnv * env, jobject obj, jlong ftFace, jstring string)
{
#ifdef HAVE_FT2
    jsize length;
    const jchar *chars;
    jint i, width;
    FT_Face face = (FT_Face)(intptr_t)ftFace;

    if (!face) {
        return 0;
    }

    length = (*env)->GetStringLength(env, string);
    if (length <= 0)
        return 0;
    chars = (*env)->GetStringCritical(env, string, NULL);
    if (chars == NULL)
        return 0;

    for (i = 0, width = 0; i < length; i++) {
        if (FT_Load_Char(face, chars[i], FT_LOAD_DEFAULT) == 0) {
            width += face->glyph->metrics.horiAdvance >> 6;
        }
    }

    (*env)->ReleaseStringCritical(env, string, chars);

    return width;

#else  /* HAVE_FT2 */
    return 0;
#endif /* HAVE_FT2 */
}

JNIEXPORT jint JNICALL
Java_java_awt_BDFontMetrics_charsWidthN(JNIEnv * env, jobject obj, jlong ftFace, jcharArray charArray,
                                                jint offset, jint length)
{
#ifdef HAVE_FT2
    jchar *chars;
    jint i, width;
    FT_Face face = (FT_Face)(intptr_t)ftFace;

    if (!face) {
        return 0;
    }

    chars = (jchar *)malloc(sizeof(jchar) * length);
    if (chars == NULL)
        return 0;
    (*env)->GetCharArrayRegion(env, charArray, offset, length, chars);
    if ((*env)->ExceptionCheck(env)) {
        free(chars);
        return 0;
    }

    for (i = 0, width = 0; i < length; i++) {
        if (FT_Load_Char(face, chars[i], FT_LOAD_DEFAULT) == 0) {
            width += face->glyph->metrics.horiAdvance >> 6;
        }
    }

    free(chars);

    return width;

#else  /* HAVE_FT2 */
    return 0;
#endif /* HAVE_FT2 */
}

#define CC (char*)             /* cast a literal from (const char*) */
#define VC (void*)(uintptr_t)  /* cast function pointer to void* */
#if defined __GNUC__
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

BD_PRIVATE CPP_EXTERN const JNINativeMethod
Java_java_awt_BDFontMetrics_methods[] =
{ /* AUTOMATICALLY GENERATED */
    {
        CC("initN"),
        CC("()J"),
        VC(Java_java_awt_BDFontMetrics_initN),
    },
    {
        CC("destroyN"),
        CC("(J)V"),
        VC(Java_java_awt_BDFontMetrics_destroyN),
    },
    {
        CC("loadFontN"),
        CC("(JLjava/lang/String;I)J"),
        VC(Java_java_awt_BDFontMetrics_loadFontN),
    },
    {
        CC("destroyFontN"),
        CC("(J)V"),
        VC(Java_java_awt_BDFontMetrics_destroyFontN),
    },
    {
        CC("charWidthN"),
        CC("(JC)I"),
        VC(Java_java_awt_BDFontMetrics_charWidthN),
    },
    {
        CC("stringWidthN"),
        CC("(JLjava/lang/String;)I"),
        VC(Java_java_awt_BDFontMetrics_stringWidthN),
    },
    {
        CC("charsWidthN"),
        CC("(J[CII)I"),
        VC(Java_java_awt_BDFontMetrics_charsWidthN),
    },
};

BD_PRIVATE CPP_EXTERN const int
Java_java_awt_BDFontMetrics_methods_count =
    sizeof(Java_java_awt_BDFontMetrics_methods)/sizeof(Java_java_awt_BDFontMetrics_methods[0]);
