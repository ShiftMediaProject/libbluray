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

import org.bluray.media.PiPControl;
import org.bluray.media.PiPStatusListener;
import org.bluray.media.StreamNotAvailableException;
import org.havi.ui.HScreenRectangle;

public class PiPControlImpl implements PiPControl {
    public Dimension getInputVideoSize()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public Dimension getVideoSize()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public HScreenRectangle getActiveVideoArea()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public HScreenRectangle getActiveVideoAreaOnScreen()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public HScreenRectangle getTotalVideoArea()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public HScreenRectangle getTotalVideoAreaOnScreen()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean supportsClipping()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public Rectangle setClipRegion(Rectangle clipRect)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public Rectangle getClipRegion()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public float[] supportsArbitraryHorizontalScaling()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public float[] supportsArbitraryVerticalScaling()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public float[] getHorizontalScalingFactors()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public float[] getVerticalScalingFactors()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public byte getPositioningCapability()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public Component getControlComponent()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void addPiPStatusListener(PiPStatusListener paramPiPStatusListener)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void removePiPStatusListener(PiPStatusListener paramPiPStatusListener)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public int getCurrentStreamNumber()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void selectStreamNumber(int num) throws StreamNotAvailableException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void setDisplay(boolean value)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean getDisplay()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void setFullScreen(boolean value)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean getFullScreen()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean getIsSyncedDuringTrickPlay()
    {
        throw new Error("Not implemented"); // TODO implement
    }

}
