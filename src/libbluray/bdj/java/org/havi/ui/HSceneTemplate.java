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

import java.awt.Dimension;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public class HSceneTemplate extends Object {
    public HSceneTemplate() {
    }

    public void setPreference(int preference, Object object, int priority) {
        preferenceMap.put(Integer.valueOf(preference), object);
        priorityMap.put(Integer.valueOf(preference), Integer.valueOf(priority));
    }

    public Object getPreferenceObject(int preference) {
        return preferenceMap.get(Integer.valueOf(preference));
    }

    public int getPreferencePriority(int preference) {
        Object prio = priorityMap.get(Integer.valueOf(preference));
        if (prio == null)
            return UNNECESSARY;

        int priority = ((Integer)prio).intValue();
        if (priority < 0 || priority > 3)
            return UNNECESSARY;
        else
            return priority;
    }

    public static final int REQUIRED = 0x01;
    public static final int PREFERRED = 0x02;
    public static final int UNNECESSARY = 0x03;

    public static final Dimension LARGEST_PIXEL_DIMENSION = new Dimension(-1,
            -1);

    public static final int GRAPHICS_CONFIGURATION = 0x00;
    public static final int SCENE_PIXEL_DIMENSION = 0x01;
    public static final int SCENE_PIXEL_LOCATION = 0x02;
    public static final int SCENE_SCREEN_DIMENSION = 0x04;
    public static final int SCENE_SCREEN_LOCATION = 0x08;

    private Map preferenceMap = Collections.synchronizedMap(new HashMap());
    private Map priorityMap = Collections.synchronizedMap(new HashMap());
}
