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

import java.awt.Component;
import java.awt.Graphics;

import org.videolan.Logger;

public abstract class FrameAccurateAnimation extends Component {

    public static final float getDefaultFrameRate()
    {
        return defaultRate;
    }

    public static boolean setDefaultFrameRate(float framerate)
    {
        if (framerate != FRAME_RATE_23_976 &&
            framerate != FRAME_RATE_24 &&
            framerate != FRAME_RATE_25 &&
            framerate != FRAME_RATE_29_97 &&
            framerate != FRAME_RATE_50 &&
            framerate != FRAME_RATE_59_94) {

            return false;
        }

        defaultRate = framerate;
        return true;
    }

    public FrameAccurateAnimation()
    {
        logger.unimplemented("FrameAccurateAnimation");
    }

    public synchronized void destroy()
    {
        logger.unimplemented("destroy");
    }

    public long getCompletedFrameCount()
    {
        logger.unimplemented("getCompletedFrameCount");
        return 0;
    }

    public float getFrameRate()
    {
        logger.unimplemented("getFrameRate");
        // TODO: rate of background video. if none, defaultRate.
        return getDefaultFrameRate();
    }

    public Graphics getGraphics()
    {
        logger.unimplemented("getGraphics");
        return super.getGraphics();
    }

    public int[] getRepeatCounts()
    {
        logger.unimplemented("getRepeatCounts");
        int[] repeatCount = null;
        return repeatCount;
    }

    public int getThreadPriority()
    {
        logger.unimplemented("getThreadPriority");
        return 5;
    }

    public synchronized boolean isAnimated()
    {
        logger.unimplemented("isAnimated");
        return false;
    }

    public void paint(Graphics g)
    {
        logger.unimplemented("paint");
    }

    public synchronized void resetStartStopTime(
            FrameAccurateAnimationTimer newTimer)
    {
        logger.unimplemented("resetStartStopTime");
    }

    public void setBounds(int x, int y, int width, int height)
    {
        logger.unimplemented("setBounds");
    }

    public void setLocation(int x, int y)
    {
        logger.unimplemented("setLocation");
    }

    public void setThreadPriority(int p)
    {
        logger.unimplemented("setThreadPriority");
    }

    public synchronized void start()
    {
        logger.unimplemented("start");
    }

    public synchronized void stop()
    {
        logger.unimplemented("stop");
    }

    public String toString()
    {
        return "FrameAccurateAnimation";
    }

    public static final float FRAME_RATE_23_976 = 23.976F;
    public static final float FRAME_RATE_24 = 24.0F;
    public static final float FRAME_RATE_25 = 25.0F;
    public static final float FRAME_RATE_29_97 = 29.969999F;
    public static final float FRAME_RATE_50 = 50.0F;
    public static final float FRAME_RATE_59_94 = 59.939999F;

    private static final long serialVersionUID = 76982966057159330L;

    private static float defaultRate = FRAME_RATE_25;

    private static final Logger logger = Logger.getLogger(FrameAccurateAnimation.class.getName());
}
