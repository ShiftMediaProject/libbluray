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

package org.havi.ui.event;

import java.awt.Color;
import java.awt.Image;

public class HEventRepresentation extends Object {
    public static final int ER_TYPE_NOT_SUPPORTED = 0;
    public static final int ER_TYPE_STRING = 1;
    public static final int ER_TYPE_COLOR = 2;
    public static final int ER_TYPE_SYMBOL = 4;

    protected HEventRepresentation(int support, String text, Color color, Image symbol)
    {
        throw new Error("Not implemented");
    }

    public boolean isSupported()
    {
        throw new Error("Not implemented");
    }

    protected void setType(int aType)
    {
        throw new Error("Not implemented");
    }

    public int getType()
    {
        throw new Error("Not implemented");
    }

    protected void setColor(Color aColor)
    {
        throw new Error("Not implemented");
    }

    public Color getColor()
    {
        throw new Error("Not implemented");
    }

    protected void setString(String aText)
    {
        throw new Error("Not implemented");
    }

    public String getString()
    {
        throw new Error("Not implemented");
    }

    protected void setSymbol(Image aSymbol)
    {
        throw new Error("Not implemented");
    }

    public Image getSymbol()
    {
        throw new Error("Not implemented");
    }

}
