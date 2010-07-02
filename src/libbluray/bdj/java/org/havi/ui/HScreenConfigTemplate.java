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

public abstract class HScreenConfigTemplate {
    public HScreenConfigTemplate()
    {
        throw new Error("Not implemented");
    }

    public void setPreference(int preference, int priority)
    {
        throw new Error("Not implemented");
    }

    public void setPreference(int preference, Object object, int priority)
    {
        throw new Error("Not implemented");
    }

    public int getPreferencePriority(int preference)
    {
        throw new Error("Not implemented");
    }

    public Object getPreferenceObject(int preference)
    {
        throw new Error("Not implemented");
    }

    public static final int REQUIRED = 0x01;
    public static final int PREFERRED = 0x02;
    public static final int DONT_CARE = 0x03;
    public static final int PREFERRED_NOT = 0x04;
    public static final int REQUIRED_NOT = 0x05;

    public static final int ZERO_BACKGROUND_IMPACT = 0x01;
    public static final int ZERO_GRAPHICS_IMPACT = 0x02;
    public static final int ZERO_VIDEO_IMPACT = 0x03;
    public static final int INTERLACED_DISPLAY = 0x04;
    public static final int FLICKER_FILTERING = 0x05;
    public static final int VIDEO_GRAPHICS_PIXEL_ALIGNED = 0x06;
    public static final int PIXEL_ASPECT_RATIO = 0x07;
    public static final int PIXEL_RESOLUTION = 0x08;
    public static final int SCREEN_RECTANGLE = 0x09;
}
