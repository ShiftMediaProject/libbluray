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
import java.awt.Dimension;
import java.awt.Rectangle;

import org.dvb.media.BackgroundVideoPresentationControl;
import org.dvb.media.VideoTransformation;
import org.havi.ui.HScreenRectangle;
import org.videolan.BDJLoader;
import org.videolan.bdjo.GraphicsResolution;

public class BackgroundVideoPresentationControlImpl implements BackgroundVideoPresentationControl {

    public Dimension getInputVideoSize()
    {
        GraphicsResolution res = BDJLoader.getBdjo().getTerminalInfo().getResolution();
        return new Dimension(res.getWidth(), res.getHeight()); // FIXME: return actual input video size
    }

    public Dimension getVideoSize()
    {
        return getInputVideoSize();
    }

    public HScreenRectangle getActiveVideoArea()
    {
        return new HScreenRectangle(0.0f, 0.0f, 1.0f, 1.0f); // FIXME: return actual active video area
    }

    public HScreenRectangle getActiveVideoAreaOnScreen()
    {
        return getActiveVideoArea(); // FIXME: return actual active video area on screen
    }

    public HScreenRectangle getTotalVideoArea()
    {
        return getActiveVideoArea(); // FIXME: return actual total video area
    }

    public HScreenRectangle getTotalVideoAreaOnScreen()
    {
        return getActiveVideoArea(); // FIXME: return actual total video area on screen
    }

    public boolean supportsClipping()
    {
        return false; // FIXME: change when clipping support added
    }

    public Rectangle setClipRegion(Rectangle clipRect)
    {
        return getClipRegion(); // FIXME: implement clipping support
    }

    public Rectangle getClipRegion()
    {
        GraphicsResolution res = BDJLoader.getBdjo().getTerminalInfo().getResolution();
        return new Rectangle(res.getWidth(), res.getHeight()); // FIXME: implement clipping support
    }

    public float[] supportsArbitraryHorizontalScaling()
    {
        return null; // FIXME: implement scaling support
    }

    public float[] supportsArbitraryVerticalScaling()
    {
        return null; // FIXME: implement scaling support
    }

    public float[] getHorizontalScalingFactors()
    {
        return null;  // FIXME: implement scaling support
    }

    public float[] getVerticalScalingFactors()
    {
        return null;  // FIXME: implement scaling support
    }

    public byte getPositioningCapability()
    {
        return 0; // FIXME: set to real positioning capability
    }

    public Component getControlComponent()
    {
        return null;
    }

    public boolean setVideoTransformation(VideoTransformation transform)
    {
        return false; // FIXME: implement actually setting transform
    }

    public VideoTransformation getVideoTransformation()
    {
        return new VideoTransformation();
    }

    public VideoTransformation getClosestMatch(VideoTransformation transform)
    {
        return new VideoTransformation(); // FIXME: actually try to find closest match
    }

}
