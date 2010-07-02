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

public class HGraphicsConfigTemplate extends HScreenConfigTemplate {
    public HGraphicsConfigTemplate()
    {
        throw new Error("Not implemented");
    }

    public boolean isConfigSupported(HGraphicsConfiguration hgc)
    {
        throw new Error("Not implemented");
    }

    public void setPreference(int preference, int priority)
    {
        throw new Error("Not implemented");
    }

    public int getPreferencePriority(int preference)
    {
        throw new Error("Not implemented");
    }

    public void setPreference(int preference, Object object, int priority)
    {
        throw new Error("Not implemented");
    }

    public Object getPreferenceObject(int preference)
    {
        throw new Error("Not implemented");
    }

    public static final int VIDEO_MIXING = 0x0C;
    public static final int MATTE_SUPPORT = 0x0D;
    public static final int IMAGE_SCALING_SUPPORT = 0x0E;
}
