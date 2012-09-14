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
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

package org.videolan;

import org.videolan.bdjo.Bdjo;

/**
 * This class allows BDJ to call various libbluray functions.
 */
public class Libbluray {
    public static int getTitles() {
        return getTitlesN(nativePointer);
    }

    public static TitleInfo getTitleInfo(int titleNum) {
        if (titleNum < 0)
            throw new IllegalArgumentException();

        return getTitleInfoN(nativePointer, titleNum);
    }

    public static PlaylistInfo getPlaylistInfo(int playlist) {
        return getPlaylistInfoN(nativePointer, playlist);
    }

    public static long seek(long pos) {
        return seekN(nativePointer, pos);
    }

    public static long seekTime(long tick) {
        return seekTimeN(nativePointer, tick);
    }

    public static long seekChapter(int chapter) {
        if (chapter < 0)
            throw new IllegalArgumentException("Chapter cannot be negative");

        return seekChapterN(nativePointer, chapter);
    }

    public static long chapterPos(int chapter) {
        if (chapter < 0)
            throw new IllegalArgumentException("Chapter cannot be negative");

        return chapterPosN(nativePointer, chapter);
    }

    public static int getCurrentChapter(){
        return getCurrentChapterN(nativePointer);
    }

    public static long seekMark(int mark) {
        if (mark < 0)
            throw new IllegalArgumentException("Mark cannot be negative");

        long result = seekMarkN(nativePointer, mark);
        if (result == -1)
            throw new IllegalArgumentException("Seek error");
        return result;
    }

    public static long seekPlayItem(int clip) {
        if (clip < 0)
            throw new IllegalArgumentException("Mark cannot be negative");

        long result = seekPlayItemN(nativePointer, clip);
        if (result == -1)
            throw new IllegalArgumentException("Seek error");
        return result;
    }

    public static boolean selectPlaylist(int playlist) {
        if (playlist < 0)
            throw new IllegalArgumentException("Playlist cannot be negative");

        return selectPlaylistN(nativePointer, playlist) == 1 ? true : false;
    }

    public static boolean selectTitle(int title) {
        if (title < 0)
            throw new IllegalArgumentException("Title cannot be negative");

        return selectTitleN(nativePointer, title) == 1 ? true : false;
    }

    public static boolean selectAngle(int angle) {
        if (angle < 0)
            throw new IllegalArgumentException("Angle cannot be negative");

        return selectAngleN(nativePointer, angle) == 1 ? true : false;
    }

    public static void seamlessAngleChange(int angle) {
        if (angle < 0)
            throw new IllegalArgumentException("Angle cannot be negative");

        seamlessAngleChangeN(nativePointer, angle);
    }

    public static long getTitleSize() {
        return getTitleSizeN(nativePointer);
    }

    public static int getCurrentTitle() {
        return getCurrentTitleN(nativePointer);
    }

    public static int getCurrentAngle() {
        return getCurrentAngleN(nativePointer);
    }

    public static long tell() {
        return tellN(nativePointer);
    }

    public static long tellTime() {
        return tellTimeN(nativePointer);
    }

    public static boolean selectRate(float rate) {
        return selectRateN(nativePointer, rate) == 1 ? true : false;
    }

    public static void writeGPR(int num, int value) {
        int ret = writeGPRN(nativePointer, num, value);

        if (ret == -1)
            throw new IllegalArgumentException("Invalid GPR");
    }

    public static void writePSR(int num, int value) {
        int ret = writePSRN(nativePointer, num, value);

        if (ret == -1)
            throw new IllegalArgumentException("Invalid PSR");
    }

    public static int readGPR(int num) {
        if (num < 0 || (num >= 4096))
            throw new IllegalArgumentException("Invalid GPR");

        return readGPRN(nativePointer, num);
    }

    public static int readPSR(int num) {
        if (num < 0 || (num >= 128))
            throw new IllegalArgumentException("Invalid PSR");

        return readPSRN(nativePointer, num);
    }

    public static Bdjo getBdjo(String name) {
        return getBdjoN(nativePointer, name);
    }

    public static void updateGraphic(int width, int height, int[] rgbArray) {
        updateGraphicN(nativePointer, width, height, rgbArray);
    }

    public static final int PSR_IG_STREAM_ID     = 0;
    public static final int PSR_PRIMARY_AUDIO_ID = 1;
    public static final int PSR_PG_STREAM        = 2;
    public static final int PSR_ANGLE_NUMBER     = 3;
    public static final int PSR_TITLE_NUMBER     = 4;
    public static final int PSR_CHAPTER          = 5;
    public static final int PSR_PLAYLIST         = 6;
    public static final int PSR_PLAYITEM         = 7;
    public static final int PSR_TIME             = 8;
    public static final int PSR_NAV_TIMER        = 9;
    public static final int PSR_SELECTED_BUTTON_ID = 10;
    public static final int PSR_MENU_PAGE_ID     = 11;
    public static final int PSR_STYLE            = 12;
    public static final int PSR_PARENTAL         = 13;
    public static final int PSR_SECONDARY_AUDIO_VIDEO = 14;
    public static final int PSR_AUDIO_CAP        = 15;
    public static final int PSR_AUDIO_LANG       = 16;
    public static final int PSR_PG_AND_SUB_LANG  = 17;
    public static final int PSR_MENU_LANG        = 18;
    public static final int PSR_COUNTRY          = 19;
    public static final int PSR_REGION           = 20;
    public static final int PSR_VIDEO_CAP        = 29;
    public static final int PSR_TEXT_CAP         = 30;
    public static final int PSR_PROFILE_VERSION  = 31;
    public static final int PSR_BACKUP_PSR4      = 36;
    public static final int PSR_BACKUP_PSR5      = 37;
    public static final int PSR_BACKUP_PSR6      = 38;
    public static final int PSR_BACKUP_PSR7      = 39;
    public static final int PSR_BACKUP_PSR8      = 40;
    public static final int PSR_BACKUP_PSR10     = 42;
    public static final int PSR_BACKUP_PSR11     = 43;
    public static final int PSR_BACKUP_PSR12     = 44;

    private static native TitleInfo getTitleInfoN(long np, int title);
    private static native PlaylistInfo getPlaylistInfoN(long np, int playlist);
    private static native int getTitlesN(long np);
    private static native long seekN(long np, long pos);
    private static native long seekTimeN(long np, long tick);
    private static native long seekChapterN(long np, int chapter);
    private static native long chapterPosN(long np, int chapter);
    private static native int getCurrentChapterN(long np);
    private static native long seekMarkN(long np, int mark);
    private static native long seekPlayItemN(long np, int clip);
    private static native int selectPlaylistN(long np, int playlist);
    private static native int selectTitleN(long np, int title);
    private static native int selectAngleN(long np, int angle);
    private static native void seamlessAngleChangeN(long np, int angle);
    private static native long getTitleSizeN(long np);
    private static native int getCurrentTitleN(long np);
    private static native int getCurrentAngleN(long np);
    private static native long tellN(long np);
    private static native long tellTimeN(long np);
    private static native int selectRateN(long np, float rate);
    private static native int writeGPRN(long np, int num, int value);
    private static native int writePSRN(long np, int num, int value);
    private static native int readGPRN(long np, int num);
    private static native int readPSRN(long np, int num);
    private static native Bdjo getBdjoN(long np, String name);
    private static native void updateGraphicN(long np, int width, int height, int[] rgbArray);

    protected static long nativePointer = 0;
}
