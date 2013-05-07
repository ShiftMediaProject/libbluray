/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 * Copyright (C) 2012  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#include "libbluray/bdj/bdj.h"
#include "libbluray/bdj/bdj_private.h"
#include "libbluray/bdj/bdj_util.h"
#include "libbluray/bdj/bdjo_parser.h"
#include "libbluray/register.h"
#include "libbluray/bluray.h"
#include "libbluray/bluray_internal.h"

#include "util/mutex.h"
#include "util/strutl.h"
#include "util/macro.h"
#include "util/logging.h"

#include <string.h>
#include <inttypes.h>

/* this automatically generated header is included to cross-check native function signatures */
#include "org_videolan_Libbluray.h"

/* Disable some warnings */
#if defined __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ifdef __cplusplus
#define CPP_EXTERN extern
#else
#define CPP_EXTERN
#endif

static jobject _make_title_info(JNIEnv* env, int title, int objType, int playbackType, const char* bdjoName, int hdmvOID)
{
    jstring name = bdjoName ? (*env)->NewStringUTF(env, bdjoName) : NULL;
    jobject ti = bdj_make_object(env, "org/videolan/TitleInfo",
                                 "(IIILjava/lang/String;I)V",
                                 title, objType, playbackType, name, hdmvOID);
    if (name)
        (*env)->DeleteLocalRef(env, name);
    return ti;
}

static jobjectArray _make_stream_array(JNIEnv* env, int count, BLURAY_STREAM_INFO* streams)
{
    jobjectArray streamArr = bdj_make_array(env,
                    "org/videolan/StreamInfo", count);
    for (int i = 0; i < count; i++) {
        BLURAY_STREAM_INFO s = streams[i];
        jstring lang = (*env)->NewStringUTF(env, (char*)s.lang);
        jobject streamObj = bdj_make_object(env, "org/videolan/StreamInfo",
                "(BBBCLjava/lang/String;BB)V", s.coding_type, s.format,
                s.rate, s.char_code, lang, s.aspect, s.subpath_id);
        (*env)->SetObjectArrayElement(env, streamArr, i, streamObj);
    }

    return streamArr;
}

static jobject _make_playlist_info(JNIEnv* env, BLURAY_TITLE_INFO* ti)
{
    jobjectArray marks = bdj_make_array(env, "org/videolan/TIMark",
            ti->mark_count);

    for (uint32_t i = 0; i < ti->mark_count; i++) {
        BLURAY_TITLE_MARK m = ti->marks[i];
        jobject mark = bdj_make_object(env, "org/videolan/TIMark",
                "(IIJJJI)V", m.idx, m.type, m.start, m.duration, m.offset, m.clip_ref);
        (*env)->SetObjectArrayElement(env, marks, i, mark);
    }

    jobjectArray clips = bdj_make_array(env, "org/videolan/TIClip",
            ti->clip_count);

    for (uint32_t i = 0; i < ti->clip_count; i++) {
        BLURAY_CLIP_INFO info = ti->clips[i];

        jobjectArray videoStreams = _make_stream_array(env, info.video_stream_count,
                info.video_streams);

        jobjectArray audioStreams = _make_stream_array(env, info.audio_stream_count,
                info.audio_streams);

        jobjectArray pgStreams = _make_stream_array(env, info.pg_stream_count,
                info.pg_streams);

        jobjectArray igStreams = _make_stream_array(env, info.ig_stream_count,
                info.ig_streams);

        jobjectArray secVideoStreams = _make_stream_array(env, info.sec_video_stream_count,
                info.sec_video_streams);

        jobjectArray secAudioStreams = _make_stream_array(env, info.sec_audio_stream_count,
                info.sec_audio_streams);

        jobject clip = bdj_make_object(env, "org/videolan/TIClip",
                "(I[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;)V",
                i, videoStreams, audioStreams, pgStreams, igStreams, secVideoStreams, secAudioStreams);

        (*env)->SetObjectArrayElement(env, clips, i, clip);
    }

    return bdj_make_object(env, "org/videolan/PlaylistInfo",
            "(IJI[Lorg/videolan/TIMark;[Lorg/videolan/TIClip;)V",
            ti->playlist, ti->duration, ti->angle_count, marks, clips);
}


JNIEXPORT jobject JNICALL Java_org_videolan_Libbluray_getTitleInfoN
  (JNIEnv * env, jclass cls, jlong np, jint title)
{
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "getTitleInfoN(%d)\n", (int)title);

    if (title == 65535) {
        if (bdj->index->first_play.object_type == indx_object_type_hdmv)
            return _make_title_info(env, 65535, indx_object_type_hdmv,
                                    bdj->index->first_play.hdmv.playback_type,
                                    NULL,
                                    bdj->index->first_play.hdmv.id_ref);
        else
            return _make_title_info(env, 65535, indx_object_type_bdj,
                                    bdj->index->first_play.bdj.playback_type,
                                    bdj->index->first_play.bdj.name,
                                    -1);
    } else if (title == 0) {
        if (bdj->index->top_menu.object_type == indx_object_type_hdmv)
            return _make_title_info(env, 0, indx_object_type_hdmv,
                                    bdj->index->top_menu.hdmv.playback_type,
                                    NULL,
                                    bdj->index->top_menu.hdmv.id_ref);
        else
            return _make_title_info(env, 0, indx_object_type_bdj,
                                    bdj->index->top_menu.bdj.playback_type,
                                    bdj->index->top_menu.bdj.name,
                                    -1);
    } else if ((title > 0) && (title <= bdj->index->num_titles)) {
        if (bdj->index->titles[title - 1].object_type == indx_object_type_hdmv)
            return _make_title_info(env, title, indx_object_type_hdmv,
                                    bdj->index->titles[title - 1].hdmv.playback_type,
                                    NULL,
                                    bdj->index->titles[title - 1].hdmv.id_ref);
        else
            return _make_title_info(env, title, indx_object_type_bdj,
                                    bdj->index->titles[title - 1].bdj.playback_type,
                                    bdj->index->titles[title - 1].bdj.name,
                                    -1);
    }
        return NULL;
}

JNIEXPORT jobject JNICALL Java_org_videolan_Libbluray_getPlaylistInfoN
  (JNIEnv * env, jclass cls, jlong np, jint playlist)
{
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    BLURAY_TITLE_INFO* ti;

    BD_DEBUG(DBG_JNI, "getPlaylistInfoN(%d)\n", (int)playlist);

    ti = bd_get_playlist_info(bdj->bd, playlist, 0);
    if (!ti)
        return NULL;

    jobject titleInfo = _make_playlist_info(env, ti);

    bd_free_title_info(ti);

    return titleInfo;
}

JNIEXPORT jbyteArray JNICALL Java_org_videolan_Libbluray_getVolumeIDN
  (JNIEnv * env, jclass cls, jlong np)
{
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    const uint8_t *vid = bd_get_vid(bdj->bd);

    static const uint8_t empty[16] = {0};
    if (!vid || !memcmp(vid, empty, sizeof(empty))) {
        return NULL;
    }
    jbyteArray array = (*env)->NewByteArray(env, 16);
    (*env)->SetByteArrayRegion(env, array, 0, 16, (const jbyte *)vid);
    return array;
}

JNIEXPORT jbyteArray JNICALL Java_org_videolan_Libbluray_getPMSNN
  (JNIEnv * env, jclass cls, jlong np)
{
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    const uint8_t *pmsn = bd_get_pmsn(bdj->bd);

    static const uint8_t empty[16] = {0};
    if (!pmsn || !memcmp(pmsn, empty, sizeof(empty))) {
        return NULL;
    }
    jbyteArray array = (*env)->NewByteArray(env, 16);
    (*env)->SetByteArrayRegion(env, array, 0, 16, (const jbyte *)pmsn);
    return array;
}

JNIEXPORT jbyteArray JNICALL Java_org_videolan_Libbluray_getDeviceBindingIDN
  (JNIEnv * env, jclass cls, jlong np)
{
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    const uint8_t *bid = bd_get_device_binding_id(bdj->bd);

    static const uint8_t empty[16] = {0};
    if (!bid || !memcmp(bid, empty, sizeof(empty))) {
        return NULL;
    }
    jbyteArray array = (*env)->NewByteArray(env, 16);
    (*env)->SetByteArrayRegion(env, array, 0, 16, (const jbyte *)bid);
    return array;
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_getUOMaskN(JNIEnv * env,
        jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_get_uo_mask(bdj->bd);
}

JNIEXPORT void JNICALL Java_org_videolan_Libbluray_setUOMaskN(JNIEnv * env,
        jclass cls, jlong np, jboolean menuCallMask, jboolean titleSearchMask) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    bdj->uo_mask = ((!!menuCallMask) * BDJ_MENU_CALL_MASK) | ((!!titleSearchMask) * BDJ_TITLE_SEARCH_MASK);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getTitlesN(JNIEnv * env,
                                                              jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bdj->index->num_titles;
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekN(JNIEnv * env,
        jclass cls, jlong np, jlong pos) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "seekN(%"PRId64")\n", (int64_t)pos);

    return bd_seek(bdj->bd, pos);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekTimeN(JNIEnv * env,
        jclass cls, jlong np, jlong tick) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "seekTimeN(%"PRId64")\n", (int64_t)tick);

    return bd_seek_time(bdj->bd, tick);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekChapterN(JNIEnv * env,
        jclass cls, jlong np, jint chapter) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "seekChapterN(%d)\n", (int)chapter);

    return bd_seek_chapter(bdj->bd, chapter);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_chapterPosN(JNIEnv * env,
        jclass cls, jlong np, jint chapter) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_chapter_pos(bdj->bd, chapter);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getCurrentChapterN(
        JNIEnv * env, jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_get_current_chapter(bdj->bd);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekMarkN(JNIEnv * env,
        jclass cls, jlong np, jint mark) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "seekMarkN(%d)\n", (int)mark);

    return bd_seek_mark(bdj->bd, mark);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekPlayItemN(JNIEnv * env,
        jclass cls, jlong np, jint clip) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "seekPlayItemN(%d)\n", (int)clip);

    return bd_seek_playitem(bdj->bd, clip);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_selectPlaylistN(
        JNIEnv * env, jclass cls, jlong np, jint playlist) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "selectPlaylistN(%05d.mpls)\n", (int)playlist);

    return bd_select_playlist(bdj->bd, playlist);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_selectTitleN(JNIEnv * env,
        jclass cls, jlong np, jint title) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "selectTitleN(%d)\n", (int)title);

    return bd_play_title_internal(bdj->bd, title);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_selectAngleN(JNIEnv * env,
        jclass cls, jlong np, jint angle) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_select_angle(bdj->bd, angle);
}

JNIEXPORT void JNICALL Java_org_videolan_Libbluray_seamlessAngleChangeN(
        JNIEnv * env, jclass cls, jlong np, jint angle) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    bd_seamless_angle_change(bdj->bd, angle);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_getTitleSizeN(JNIEnv * env,
        jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_get_title_size(bdj->bd);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getCurrentTitleN(
        JNIEnv * env, jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_psr_read(bdj->reg, PSR_TITLE_NUMBER);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getCurrentAngleN(
        JNIEnv * env, jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_get_current_angle(bdj->bd);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_tellN(JNIEnv * env,
        jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_tell(bdj->bd);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_tellTimeN(JNIEnv * env,
        jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_tell_time(bdj->bd);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_selectRateN(JNIEnv * env,
        jclass cls, jlong np, jfloat rate) {
    return 1;
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_writeGPRN(JNIEnv * env,
        jclass cls, jlong np, jint num, jint value) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "writeGPRN(%d,%d)\n", (int)num, (int)value);

    return bd_gpr_write(bdj->reg, num, value);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_readGPRN(JNIEnv * env,
        jclass cls, jlong np, jint num) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_gpr_read(bdj->reg, num);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_writePSRN(JNIEnv * env,
        jclass cls, jlong np, jint num, jint value) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "writePSRN(%d,%d)\n", (int)num, (int)value);

    bd_mutex_lock((BD_MUTEX*)bdj->bd);
    int res = bd_psr_write(bdj->reg, num, value);
    bd_mutex_unlock((BD_MUTEX*)bdj->bd);
    return res;
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_readPSRN(JNIEnv * env,
        jclass cls, jlong np, jint num) {
    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
    return bd_psr_read(bdj->reg, num);
}

JNIEXPORT jobject JNICALL Java_org_videolan_Libbluray_getBdjoN(JNIEnv * env,
                                                               jclass cls, jlong np, jstring name) {

  BDJAVA* bdj = (BDJAVA*)(intptr_t)np;
  const char *bdjo_name = (*env)->GetStringUTFChars(env, name, NULL);;
  char* bdjo_path = str_printf("%s%s/%s.bdjo", bdj->path, BDJ_BDJO_PATH, bdjo_name);
  (*env)->ReleaseStringUTFChars(env, name, bdjo_name);
  jobject bdjo = bdjo_read(env, bdjo_path);
  X_FREE(bdjo_path);

  return bdjo;
}

JNIEXPORT void JNICALL Java_org_videolan_Libbluray_updateGraphicN(JNIEnv * env,
        jclass cls, jlong np, jint width, jint height, jintArray rgbArray,
        jint x0, jint y0, jint x1, jint y1) {

    BDJAVA* bdj = (BDJAVA*)(intptr_t)np;

    BD_DEBUG(DBG_JNI, "updateGraphicN(%d,%d-%d,%d)\n", x0, y0, x1, y1);

    /* app callback not initialized ? */
    if (!bdj || !bdj->osd_cb) {
        return;
    }

    /* close ? */
    if (!rgbArray) {
        bdj->osd_cb(bdj->bd, NULL, (int)width, (int)height, 0, 0, 0, 0);
        return;
    }

    /* nothing to draw ? */
    if (x1 < x0 || y1 < y0 || (x1 | y1) < 0) {
        return;
    }

    if (bdj->buf && bdj->buf->buf[BD_OVERLAY_IG]) {
        jint y, *dst;
        jsize offset;

        /* copy to application-allocated buffer */

        if (bdj->buf->lock) {
            bdj->buf->lock(bdj->buf);
        }

        /* check buffer size */
        if (bdj->buf->width != width || bdj->buf->height != height) {
            BD_DEBUG(DBG_BDJ | DBG_CRIT, "Incorrect ARGB frame buffer size (is: %dx%d expect: %dx%d)\n",
                     bdj->buf->width, bdj->buf->height, width, height);
        }

        /* clip */
        if (y1 >= bdj->buf->height) {
            BD_DEBUG(DBG_BDJ | DBG_CRIT, "Cropping %d rows from bottom\n", y1 - bdj->buf->height);
            y1 = bdj->buf->height - 1;
        }
        if (x1 >= bdj->buf->width) {
            BD_DEBUG(DBG_BDJ | DBG_CRIT, "Cropping %d pixels from right\n", x1 - bdj->buf->width);
            x1 = bdj->buf->width - 1;
        }

        /* copy */

        offset = y0 * width + x0;
        dst    = (jint*)bdj->buf->buf[BD_OVERLAY_IG] + y0 * bdj->buf->width + x0;

        for (y = y0; y <= y1; y++) {
            (*env)->GetIntArrayRegion(env, rgbArray, offset, x1 - x0 + 1, dst);
            offset += width;
            dst += bdj->buf->width;
        }

        /* check for errors */
        if ((*env)->ExceptionOccurred(env)) {
            BD_DEBUG(DBG_BDJ | DBG_CRIT, "Array access error at %d (+%d)\n", offset, x1 - x0 + 1);
            (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
        }

        if (bdj->buf->unlock) {
            bdj->buf->unlock(bdj->buf);
        }

        bdj->osd_cb(bdj->bd, bdj->buf->buf[BD_OVERLAY_IG], (int)width, (int)height,
                    x0, y0, x1, y1);

    } else {

        /* return java array */

        jint *image = (jint *)(*env)->GetPrimitiveArrayCritical(env, rgbArray, NULL);
        if (image) {
            bdj->osd_cb(bdj->bd, (const unsigned *)image, (int)width, (int)height,
                        x0, y0, x1, y1);
            (*env)->ReleasePrimitiveArrayCritical(env, rgbArray, image, JNI_ABORT);
        } else {
            BD_DEBUG(DBG_BDJ | DBG_CRIT, "GetPrimitiveArrayCritical() failed\n");
        }
    }
}

#define CC (char*)             /* cast a literal from (const char*) */
#define VC (void*)(uintptr_t)  /* cast function pointer to void* */
#if defined __GNUC__
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

BD_PRIVATE CPP_EXTERN const JNINativeMethod
Java_org_videolan_Libbluray_methods[] =
{ /* AUTOMATICALLY GENERATED */
    {
        CC("getVolumeIDN"),
        CC("(J)[B"),
        VC(Java_org_videolan_Libbluray_getVolumeIDN),
    },
    {
        CC("getPMSNN"),
        CC("(J)[B"),
        VC(Java_org_videolan_Libbluray_getPMSNN),
    },
    {
        CC("getDeviceBindingIDN"),
        CC("(J)[B"),
        VC(Java_org_videolan_Libbluray_getDeviceBindingIDN),
    },
    {
        CC("getUOMaskN"),
        CC("(J)J"),
        VC(Java_org_videolan_Libbluray_getUOMaskN),
    },
    {
        CC("setUOMaskN"),
        CC("(JZZ)V"),
        VC(Java_org_videolan_Libbluray_setUOMaskN),
    },
    {
        CC("getTitleInfoN"),
        CC("(JI)Lorg/videolan/TitleInfo;"),
        VC(Java_org_videolan_Libbluray_getTitleInfoN),
    },
    {
        CC("getPlaylistInfoN"),
        CC("(JI)Lorg/videolan/PlaylistInfo;"),
        VC(Java_org_videolan_Libbluray_getPlaylistInfoN),
    },
    {
        CC("getTitlesN"),
        CC("(J)I"),
        VC(Java_org_videolan_Libbluray_getTitlesN),
    },
    {
        CC("seekN"),
        CC("(JJ)J"),
        VC(Java_org_videolan_Libbluray_seekN),
    },
    {
        CC("seekTimeN"),
        CC("(JJ)J"),
        VC(Java_org_videolan_Libbluray_seekTimeN),
    },
    {
        CC("seekChapterN"),
        CC("(JI)J"),
        VC(Java_org_videolan_Libbluray_seekChapterN),
    },
    {
        CC("chapterPosN"),
        CC("(JI)J"),
        VC(Java_org_videolan_Libbluray_chapterPosN),
    },
    {
        CC("getCurrentChapterN"),
        CC("(J)I"),
        VC(Java_org_videolan_Libbluray_getCurrentChapterN),
    },
    {
        CC("seekMarkN"),
        CC("(JI)J"),
        VC(Java_org_videolan_Libbluray_seekMarkN),
    },
    {
        CC("seekPlayItemN"),
        CC("(JI)J"),
        VC(Java_org_videolan_Libbluray_seekPlayItemN),
    },
    {
        CC("selectPlaylistN"),
        CC("(JI)I"),
        VC(Java_org_videolan_Libbluray_selectPlaylistN),
    },
    {
        CC("selectTitleN"),
        CC("(JI)I"),
        VC(Java_org_videolan_Libbluray_selectTitleN),
    },
    {
        CC("selectAngleN"),
        CC("(JI)I"),
        VC(Java_org_videolan_Libbluray_selectAngleN),
    },
    {
        CC("seamlessAngleChangeN"),
        CC("(JI)V"),
        VC(Java_org_videolan_Libbluray_seamlessAngleChangeN),
    },
    {
        CC("getTitleSizeN"),
        CC("(J)J"),
        VC(Java_org_videolan_Libbluray_getTitleSizeN),
    },
    {
        CC("getCurrentTitleN"),
        CC("(J)I"),
        VC(Java_org_videolan_Libbluray_getCurrentTitleN),
    },
    {
        CC("getCurrentAngleN"),
        CC("(J)I"),
        VC(Java_org_videolan_Libbluray_getCurrentAngleN),
    },
    {
        CC("tellN"),
        CC("(J)J"),
        VC(Java_org_videolan_Libbluray_tellN),
    },
    {
        CC("tellTimeN"),
        CC("(J)J"),
        VC(Java_org_videolan_Libbluray_tellTimeN),
    },
    {
        CC("selectRateN"),
        CC("(JF)I"),
        VC(Java_org_videolan_Libbluray_selectRateN),
    },
    {
        CC("writeGPRN"),
        CC("(JII)I"),
        VC(Java_org_videolan_Libbluray_writeGPRN),
    },
    {
        CC("writePSRN"),
        CC("(JII)I"),
        VC(Java_org_videolan_Libbluray_writePSRN),
    },
    {
        CC("readGPRN"),
        CC("(JI)I"),
        VC(Java_org_videolan_Libbluray_readGPRN),
    },
    {
        CC("readPSRN"),
        CC("(JI)I"),
        VC(Java_org_videolan_Libbluray_readPSRN),
    },
    {
        CC("getBdjoN"),
        CC("(JLjava/lang/String;)Lorg/videolan/bdjo/Bdjo;"),
        VC(Java_org_videolan_Libbluray_getBdjoN),
    },
    {
        CC("updateGraphicN"),
        CC("(JII[IIIII)V"),
        VC(Java_org_videolan_Libbluray_updateGraphicN),
    },
};

BD_PRIVATE CPP_EXTERN const int
Java_org_videolan_Libbluray_methods_count =
    sizeof(Java_org_videolan_Libbluray_methods)/sizeof(Java_org_videolan_Libbluray_methods[0]);

