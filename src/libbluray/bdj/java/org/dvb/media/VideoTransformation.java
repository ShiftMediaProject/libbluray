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
import org.videolan.BDJLoader;
import org.videolan.bdjo.GraphicsResolution;

public class VideoTransformation {
    protected VideoTransformation(boolean panAndScan) {
        this.panAndScan = panAndScan;
    }
    
    public VideoTransformation()
    {
        GraphicsResolution res = BDJLoader.getBdjo().getTerminalInfo().getResolution();
        
        this.clip = new Rectangle(res.getWidth(), res.getHeight());
        this.scaling[0] = 1.0f;
        this.scaling[1] = 1.0f;
        this.position = new HScreenPoint(0.0f, 0.0f);
    }

    public VideoTransformation(Rectangle clipRect,
            float horizontalScalingFactor, float verticalScalingFactor,
            HScreenPoint location)
    {
        this.clip = clipRect;
        this.scaling[0] = horizontalScalingFactor;
        this.scaling[1] = verticalScalingFactor;
        this.position = location;
    }

    public void setClipRegion(Rectangle clipRect)
    {
        this.clip = clipRect;
    }

    public Rectangle getClipRegion()
    {
        return clip;
    }

    public void setScalingFactors(float horizontalScalingFactor,
            float verticalScalingFactor)
    {
        this.scaling[0] = horizontalScalingFactor;
        this.scaling[1] = verticalScalingFactor;
    }

    public float[] getScalingFactors()
    {
        return scaling;
    }

    public void setVideoPosition(HScreenPoint location)
    {
        this.position = location;
    }

    public HScreenPoint getVideoPosition()
    {
        return position;
    }

    public boolean isPanAndScan()
    {
        return panAndScan;
    }
    
    private float[] scaling = new float[2];
    private HScreenPoint position = null;
    private Rectangle clip = null;
    private boolean panAndScan = false;
}
