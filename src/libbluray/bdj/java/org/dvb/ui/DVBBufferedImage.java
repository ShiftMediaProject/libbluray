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

import java.awt.AWTException;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.util.logging.Logger;
import org.videolan.GUIManager;

public class DVBBufferedImage extends Image {
    public DVBBufferedImage(int width, int height)
    {
        this(width, height, TYPE_BASE);
    }

    public DVBBufferedImage(int width, int height, int type)
    {
        this.type = type;
        
        try {
            img = GUIManager.getInstance().createBufferedImage(width, height);
        } catch (AWTException e) {
            logger.severe("Failed to create DVBBufferedImage");
            e.printStackTrace();
        }
    }
    
    private DVBBufferedImage(int type, BufferedImage img)
    {
        this.type = type;
        this.img = img;
    }

    public DVBGraphics createGraphics()
    {
        DVBGraphics gfx = new DVBGraphicsImpl(img.createGraphics());
        gfx.type = type;
        return gfx;
    }

    public void flush()
    {
        img.flush();
    }

    public Graphics getGraphics()
    {
        return img.getGraphics();
    }

    public int getHeight()
    {
        return img.getHeight();
    }

    public int getHeight(ImageObserver observer)
    {
        return img.getHeight(observer);
    }

    public Image getImage()
    {
        return img;
    }

    public void dispose()
    {

    }

    public Object getProperty(String name, ImageObserver observer)
    {
        return img.getProperty(name, observer);
    }

    public int getRGB(int x, int y)
    {
        if (x < 0 || y < 0 || x > getWidth() || y > getHeight())
            throw new ArrayIndexOutOfBoundsException();
        
        return img.getRGB(x, y);
    }

    public int[] getRGB(int startX, int startY, int w, int h, int[] rgbArray,
            int offset, int scansize)
    {
        if (startX < 0 || startY < 0 || startX > getWidth() || startY > getHeight())
            throw new ArrayIndexOutOfBoundsException();
        if (startX + w < 0 || startY + h < 0 || startX + w > getWidth() || startY + h > getHeight())
            throw new ArrayIndexOutOfBoundsException();
        
        return img.getRGB(startX, startY, w, h, rgbArray, offset, scansize);
    }

    public ImageProducer getSource()
    {
        return img.getSource();
    }

    public DVBBufferedImage getSubimage(int x, int y, int w, int h)
            throws DVBRasterFormatException
    {
        if (x < 0 || y < 0 || x > getWidth() || y > getHeight())
            throw new DVBRasterFormatException("Out of bounds");
        if (x + w < 0 || y + h < 0 || x + w > getWidth() || y + h > getHeight())
            throw new DVBRasterFormatException("Out of bounds");
        
        return new DVBBufferedImage(type, img.getSubimage(x, y, w, h));
    }

    public int getWidth()
    {
        return img.getWidth();
    }

    public int getWidth(ImageObserver observer)
    {
        return img.getWidth(observer);
    }

    public synchronized void setRGB(int x, int y, int rgb)
    {
        img.setRGB(x, y, rgb);
    }

    public void setRGB(int startX, int startY, int w, int h, int[] rgbArray,
            int offset, int scansize)
    {
        img.setRGB(startX, startY, w, h, rgbArray, offset, scansize);
    }

    public String toString()
    {
        return new String("DVBBufferedImage");
    }

    public Image getScaledInstance(int width, int height, int hints)
    {
        return img.getScaledInstance(width, height, hints);
    }

    public static final int TYPE_ADVANCED = 20;
    public static final int TYPE_BASE = 21;

    private BufferedImage img;
    private int type = TYPE_BASE;
    
    private static final Logger logger = Logger.getLogger(DVBBufferedImage.class.getName());
}
