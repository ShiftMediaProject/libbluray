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

package org.videolan.media.content.playlist;

import java.awt.Component;

import org.dvb.media.VideoFormatControl;
import org.dvb.media.VideoFormatListener;
import org.dvb.media.VideoTransformation;
import org.videolan.BDJLoader;
import org.videolan.bdjo.GraphicsResolution;

public class VideoFormatControlImpl implements VideoFormatControl {

    public Component getControlComponent()
    {
        return null;
    }

    public int getAspectRatio()
    {
        return ASPECT_RATIO_UNKNOWN; // FIXME: get actual aspect ratio from video
    }

    public int getActiveFormatDefinition()
    {
        return AFD_NOT_PRESENT; // FIXME: get actual AFD
    }

    public int getDecoderFormatConversion()
    {
        return DFC_PROCESSING_NONE; // FIXME: get actual DFC
    }

    public VideoTransformation getVideoTransformation(int dfc)
    {
        return null; // FIXME: implement
    }

    public int getDisplayAspectRatio()
    {
        GraphicsResolution res = BDJLoader.getBdjo().getTerminalInfo().getResolution();
        
        switch (res) {
        case HD_1920_1080:
        case HD_1280_720:
            return ASPECT_RATIO_16_9;
        case SD:
        case SD_50HZ_720_576:
        case SD_60HZ_720_480:
        case QHD_960_540:
        default:
            return ASPECT_RATIO_UNKNOWN;
        }
    }

    public boolean isPlatform()
    {
        return false; // FIXME: ???
    }

    public void addVideoFormatListener(VideoFormatListener listener)
    {
        // FIXME: implement
    }

    public void removeVideoFormatListener(VideoFormatListener listener)
    {
        // FIXME: implement
    }

}
