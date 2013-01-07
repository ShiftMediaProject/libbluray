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

package org.bluray.ui;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import javax.media.Time;

import org.videolan.Logger;

public class SyncFrameAccurateAnimation extends FrameAccurateAnimation {
    public static SyncFrameAccurateAnimation getInstance(Dimension size,
            int numFrames, AnimationParameters params)
            throws NullPointerException, IllegalArgumentException
    {
        // TODO: validate params
        return new SyncFrameAccurateAnimation(size, numFrames, params);
    }

    private SyncFrameAccurateAnimation(Dimension size,
            int numFrames, AnimationParameters params)
    {
        logger.unimplemented("SyncFrameAccurateAnimation");
    }

    public boolean finishDrawing(long frameNumber)
            throws IllegalArgumentException
    {
        logger.unimplemented("finishDrawing");
        return true;
    }

    public boolean finishDrawing(long frameNumber, Rectangle[] updateArea)
            throws IllegalArgumentException
    {
        logger.unimplemented("finishDrawing");
        return true;
    }

    public Time getAnimationFrameTime(long animationFrame)
            throws IllegalStateException
    {
        logger.unimplemented("getAnimationFrameTime");
        return new Time(40000000);
    }

    public void paint(Graphics graphics)
    {
        logger.unimplemented("paint");
    }

    public void setBounds(int x, int y, int width, int height)
    {
        logger.unimplemented("setBounds");
    }

    public Graphics2D startDrawing(long frameNumber)
            throws IllegalArgumentException
    {
        logger.unimplemented("startDrawing");
        return null;
    }

    private static final long serialVersionUID = -1340138671201204543L;

    private static final Logger logger = Logger.getLogger(SyncFrameAccurateAnimation.class.getName());
}
