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
import java.awt.Image;

import org.videolan.Logger;

public class ImageFrameAccurateAnimation extends FrameAccurateAnimation {
    public static ImageFrameAccurateAnimation getInstance(Image[] images,
            Dimension size, AnimationParameters params, int playmode)
            throws NullPointerException, IllegalArgumentException
    {
        // TODO: validate params
        logger.unimplemented("getInstance");
        return new ImageFrameAccurateAnimation(images, size, params, playmode);
    }

    private ImageFrameAccurateAnimation(Image[] images,
            Dimension size, AnimationParameters params, int playmode)
    {
        this.images = images;
        this.playmode = playmode;
        logger.unimplemented("ImageFrameAccurateAnimation");
    }

    public AnimationParameters getAnimationParameters()
    {
        logger.unimplemented("getAnimationParameters");
        return new AnimationParameters();
    }

    public Image[] getImages()
    {
        return images;
    }

    public int getPlayMode()
    {
        return playmode;
    }

    public int getPosition()
    {
        return position;
    }

    public void prepareImages()
    {
        logger.unimplemented("prepareImages");
    }

    public void setPlayMode(int mode) throws IllegalArgumentException
    {
        if (mode < PLAY_REPEATING  || mode > PLAY_ONCE)
            throw new IllegalArgumentException();
        this.playmode = mode;
    }

    public void setPosition(int position)
    {
        this.position = position;
    }

    protected void destroyImp()
    {
        logger.unimplemented("destroyImp");
    }

    protected void startImp()
    {
        logger.unimplemented("startImp");
    }

    public void paint(Graphics g)
    {
        logger.unimplemented("paint");
    }

    public static final int PLAY_REPEATING = 1;
    public static final int PLAY_ALTERNATING = 2;
    public static final int PLAY_ONCE = 3;

    private int playmode;
    private int position = 0;
    private Image[] images = null;

    private static final long serialVersionUID = 2691302238670178111L;

    private static final Logger logger = Logger.getLogger(FrameAccurateAnimation.class.getName());
}
