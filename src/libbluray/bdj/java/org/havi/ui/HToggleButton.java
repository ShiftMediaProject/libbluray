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

import java.awt.Image;

public class HToggleButton extends HGraphicButton implements HSwitchable {
    public HToggleButton()
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image image, int x, int y, int width, int height)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image image)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image image, int x, int y, int width, int height,
            boolean state)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image imageNormal, Image imageFocused,
            Image imageActioned, Image imageNormalActioned, int x, int y,
            int width, int height, boolean state)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image imageNormal, Image imageFocused,
            Image imageActioned, Image imageNormalActioned, boolean state)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image image, int x, int y, int width, int height,
            boolean state, HToggleGroup group)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image image, boolean state, HToggleGroup group)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image imageNormal, Image imageFocused,
            Image imageActioned, Image imageNormalActioned, int x, int y,
            int width, int height, boolean state, HToggleGroup group)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public HToggleButton(Image imageNormal, Image imageFocused,
            Image imageActioned, Image imageNormalActioned, boolean state,
            HToggleGroup group)
    {
        org.videolan.Logger.unimplemented(HToggleButton.class.getName(), "");
    }

    public void setToggleGroup(HToggleGroup group)
    {
        throw new Error("Not implemented");
    }

    public HToggleGroup getToggleGroup()
    {
        throw new Error("Not implemented");
    }

    public void removeToggleGroup()
    {
        throw new Error("Not implemented");
    }

    public static void setDefaultLook(HGraphicLook hlook)
    {
        DefaultLook = hlook;
    }

    public static HGraphicLook getDefaultLook()
    {
        if (DefaultLook == null)
            org.videolan.Logger.unimplemented("", "getDefaultLook");
        return DefaultLook;
    }

    public boolean getSwitchableState()
    {
        throw new Error("Not implemented");
    }

    public void setSwitchableState(boolean state)
    {
        throw new Error("Not implemented");
    }

    public void setUnsetActionSound(HSound sound)
    {
        throw new Error("Not implemented");
    }

    public HSound getUnsetActionSound()
    {
        throw new Error("Not implemented");
    }

    private static HGraphicLook DefaultLook = null;

    private static final long serialVersionUID = 2602166176018744707L;
}
