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

package org.dvb.ui;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;

public class DVBBufferedImage extends Image {
    public DVBBufferedImage(int width, int height)
    {
        throw new Error("Not implemented");
    }

    public DVBBufferedImage(int width, int height, int type)
    {
        throw new Error("Not implemented");
    }

    public DVBGraphics createGraphics()
    {
        throw new Error("Not implemented");
    }

    public void flush()
    {
        throw new Error("Not implemented");
    }

    public Graphics getGraphics()
    {
        throw new Error("Not implemented");
    }

    public int getHeight()
    {
        throw new Error("Not implemented");
    }

    public int getHeight(ImageObserver observer)
    {
        throw new Error("Not implemented");
    }

    public Image getImage()
    {
        throw new Error("Not implemented");
    }

    public void dispose()
    {
        throw new Error("Not implemented");
    }

    public Object getProperty(String name, ImageObserver observer)
    {
        throw new Error("Not implemented");
    }

    public int getRGB(int x, int y)
    {
        throw new Error("Not implemented");
    }

    public int[] getRGB(int startX, int startY, int w, int h, int[] rgbArray,
            int offset, int scansize)
    {
        throw new Error("Not implemented");
    }

    public ImageProducer getSource()
    {
        throw new Error("Not implemented");
    }

    public DVBBufferedImage getSubimage(int x, int y, int w, int h)
            throws DVBRasterFormatException
    {
        throw new Error("Not implemented");
    }

    public int getWidth()
    {
        throw new Error("Not implemented");
    }

    public int getWidth(ImageObserver observer)
    {
        throw new Error("Not implemented");
    }

    public synchronized void setRGB(int x, int y, int rgb)
    {
        throw new Error("Not implemented");
    }

    public void setRGB(int startX, int startY, int w, int h, int[] rgbArray,
            int offset, int scansize)
    {
        throw new Error("Not implemented");
    }

    public String toString()
    {
        return new String("DVBBufferedImage");
    }

    public Image getScaledInstance(int width, int height, int hints)
    {
        throw new Error("Not implemented");
    }

    public static final int TYPE_ADVANCED = 20;
    public static final int TYPE_BASE = 21;
}
