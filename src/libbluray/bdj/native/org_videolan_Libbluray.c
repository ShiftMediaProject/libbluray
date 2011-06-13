#include "org_videolan_Libbluray.h"

#include "libbluray/bdj/bdj.h"
#include "libbluray/bdj/bdj_private.h"
#include "libbluray/bdj/bdj_util.h"
#include "libbluray/register.h"
#include "libbluray/bluray.h"

/* Disable some warnings */
#if defined __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

jobjectArray _make_stream_array(JNIEnv* env, int count, BLURAY_STREAM_INFO* streams)
{
    jobjectArray streamArr = bdj_make_array(env,
                    "org/videolan/StreamInfo", count);
    for (int i = 0; i < count; i++) {
        BLURAY_STREAM_INFO s = streams[i];
        jstring lang = (*env)->NewStringUTF(env, (char*)s.lang);
        jobject streamObj = bdj_make_object(env, "org/videolan/StreamInfo",
                "(BBBCLjava/lang/String;S)V", s.coding_type, s.format,
                s.rate, s.char_code, lang, s.pid);
        (*env)->SetObjectArrayElement(env, streamArr, i, streamObj);
    }

    return streamArr;
}

jobject _make_title_info(JNIEnv* env, BLURAY_TITLE_INFO* ti)
{
    jobjectArray chapters = bdj_make_array(env, "org/videolan/TIChapter",
            ti->chapter_count);

    for (uint32_t i = 0; i < ti->chapter_count; i++) {
        BLURAY_TITLE_CHAPTER c = ti->chapters[i];
        jobject chapter = bdj_make_object(env, "org/videolan/TIChapter",
                "(IJJJ)V", c.idx, c.start, c.duration, c.offset);
        (*env)->SetObjectArrayElement(env, chapters, i, chapter);
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
                "([Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;[Lorg/videolan/StreamInfo;)V",
                videoStreams, audioStreams, pgStreams, igStreams, secVideoStreams, secAudioStreams);

        (*env)->SetObjectArrayElement(env, clips, i, clip);
    }

    return bdj_make_object(env, "org/videolan/TitleInfo",
            "(IIJI[Lorg/videolan/TIChapter;[Lorg/videolan/TIClip;)V", ti->idx,
            ti->playlist, ti->duration, ti->angle_count, chapters, clips);
}


JNIEXPORT jobject JNICALL Java_org_videolan_Libbluray_getTitleInfoN
  (JNIEnv * env, jclass cls, jlong np, jint title)
{
    BDJAVA* bdj = (BDJAVA*)np;

    BLURAY_TITLE_INFO* ti = bd_get_title_info(bdj->bd, title, 0);
    if (!ti)
        return NULL;

    jobject titleInfo = _make_title_info(env, ti);

    bd_free_title_info(ti);

    return titleInfo;
}

JNIEXPORT jobject JNICALL Java_org_videolan_Libbluray_getPlaylistInfoN
  (JNIEnv * env, jclass cls, jlong np, jint playlist)
{
    BDJAVA* bdj = (BDJAVA*)np;
    BLURAY_TITLE_INFO* ti = bd_get_playlist_info(bdj->bd, playlist, 0);
    if (!ti)
        return NULL;

    jobject titleInfo = _make_title_info(env, ti);

    bd_free_title_info(ti);

    return titleInfo;
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getTitlesN(JNIEnv * env,
        jclass cls, jlong np, jchar flags) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_get_titles(bdj->bd, flags, 0);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekN(JNIEnv * env,
        jclass cls, jlong np, jlong pos) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_seek(bdj->bd, pos);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekTimeN(JNIEnv * env,
        jclass cls, jlong np, jlong tick) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_seek_time(bdj->bd, tick);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekChapterN(JNIEnv * env,
        jclass cls, jlong np, jint chapter) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_seek_chapter(bdj->bd, chapter);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_chapterPosN(JNIEnv * env,
        jclass cls, jlong np, jint chapter) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_chapter_pos(bdj->bd, chapter);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getCurrentChapterN(
        JNIEnv * env, jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_get_current_chapter(bdj->bd);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_seekMarkN(JNIEnv * env,
        jclass cls, jlong np, jint mark) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_seek_mark(bdj->bd, mark);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_selectPlaylistN(
        JNIEnv * env, jclass cls, jlong np, jint playlist) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_select_playlist(bdj->bd, playlist);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_selectTitleN(JNIEnv * env,
        jclass cls, jlong np, jint title) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_select_title(bdj->bd, title);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_selectAngleN(JNIEnv * env,
        jclass cls, jlong np, jint angle) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_select_angle(bdj->bd, angle);
}

JNIEXPORT void JNICALL Java_org_videolan_Libbluray_seamlessAngleChangeN(
        JNIEnv * env, jclass cls, jlong np, jint angle) {
    BDJAVA* bdj = (BDJAVA*) np;
    bd_seamless_angle_change(bdj->bd, angle);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_getTitleSizeN(JNIEnv * env,
        jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_get_title_size(bdj->bd);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getCurrentTitleN(
        JNIEnv * env, jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_get_current_title(bdj->bd);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_getCurrentAngleN(
        JNIEnv * env, jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_get_current_angle(bdj->bd);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_tellN(JNIEnv * env,
        jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_tell(bdj->bd);
}

JNIEXPORT jlong JNICALL Java_org_videolan_Libbluray_tellTimeN(JNIEnv * env,
        jclass cls, jlong np) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_tell_time(bdj->bd);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_writeGPRN(JNIEnv * env,
        jclass cls, jlong np, jint num, jint value) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_gpr_write(bdj->reg, num, value);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_readGPRN(JNIEnv * env,
        jclass cls, jlong np, jint num) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_gpr_read(bdj->reg, num);
}

JNIEXPORT jint JNICALL Java_org_videolan_Libbluray_readPSRN(JNIEnv * env,
        jclass cls, jlong np, jint num) {
    BDJAVA* bdj = (BDJAVA*) np;
    return bd_psr_read(bdj->reg, num);
}
