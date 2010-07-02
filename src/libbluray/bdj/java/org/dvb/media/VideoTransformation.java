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

package org.dvb.media;

import java.awt.Rectangle;
import org.havi.ui.HScreenPoint;

public class VideoTransformation {
    public VideoTransformation()
    {
        throw new Error("Not implemented");
    }

    public VideoTransformation(Rectangle clipRect,
            float horizontalScalingFactor, float verticalScalingFactor,
            HScreenPoint location)
    {
        throw new Error("Not implemented");
    }

    public void setClipRegion(Rectangle clipRect)
    {
        throw new Error("Not implemented");
    }

    public Rectangle getClipRegion()
    {
        throw new Error("Not implemented");
    }

    public void setScalingFactors(float horizontalScalingFactor,
            float verticalScalingFactor)
    {
        throw new Error("Not implemented");
    }

    public float[] getScalingFactors()
    {
        throw new Error("Not implemented");
    }

    public void setVideoPosition(HScreenPoint location)
    {
        throw new Error("Not implemented");
    }

    public HScreenPoint getVideoPosition()
    {
        throw new Error("Not implemented");
    }

    public boolean isPanAndScan()
    {
        throw new Error("Not implemented");
    }
}
