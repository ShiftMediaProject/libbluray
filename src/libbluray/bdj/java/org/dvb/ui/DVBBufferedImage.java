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

import java.awt.image.BufferedImage;

public class DVBBufferedImage extends BufferedImage {
    public DVBBufferedImage(int width, int height)
    {
        this(width, height, TYPE_BASE);
    }

    public DVBBufferedImage(int width, int height, int type)
    {
        super(width, height, type == TYPE_BASE ? TYPE_INT_RGB : TYPE_INT_ARGB);
    }

    public DVBGraphics createGraphics()
    {
        DVBGraphics gfx = new DVBGraphicsImpl(super.createGraphics());
        gfx.type = type;
        return gfx;
    }
    
    public void dispose()
    {
    
    }

    public String toString()
    {
        return new String("DVBBufferedImage");
    }

    public static final int TYPE_ADVANCED = 20;
    public static final int TYPE_BASE = 21;

    private int type = TYPE_BASE;
}
