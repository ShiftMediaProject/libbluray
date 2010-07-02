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

package org.havi.ui;

import java.awt.Rectangle;
import java.awt.Component;
import java.awt.Container;
import java.awt.Image;
import java.awt.Font;
import java.awt.Color;

public class HGraphicsConfiguration extends HScreenConfiguration {
    protected HGraphicsConfiguration()
    {

    }

    public HGraphicsDevice getDevice()
    {
        throw new Error("Not implemented");
    }

    public HGraphicsConfigTemplate getConfigTemplate()
    {
        throw new Error("Not implemented");
    }

    public HScreenRectangle getComponentHScreenRectangle(Component component)
    {
        throw new Error("Not implemented");
    }

    public Rectangle getPixelCoordinatesHScreenRectangle(HScreenRectangle sr,
            Container cont)
    {
        throw new Error("Not implemented");
    }

    public Image getCompatibleImage(Image input, HImageHints ih)
    {
        throw new Error("Not implemented");
    }

    public Font[] getAllFonts()
    {
        throw new Error("Not implemented");
    }

    public Color getPunchThroughToBackgroundColor(int percentage)
    {
        throw new Error("Not implemented");
    }

    public Color getPunchThroughToBackgroundColor(int percentage,
            HVideoDevice hvd)
    {
        throw new Error("Not implemented");
    }

    public Color getPunchThroughToBackgroundColor(Color color, int percentage)
    {
        throw new Error("Not implemented");
    }

    public Color getPunchThroughToBackgroundColor(Color color, int percentage,
            HVideoDevice v)
    {
        throw new Error("Not implemented");
    }

    public void dispose(Color c)
    {
        throw new Error("Not implemented");
    }
}
