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

package org.dvb.application;

import java.util.Enumeration;

public class AppsDatabase {
    static public AppsDatabase getAppsDatabase()
    {
        throw new Error("Not implemented");
    }

    public int size()
    {
        throw new Error("Not implemented");
    }

    public Enumeration<?> getAppIDs(AppsDatabaseFilter filter)
    {
        throw new Error("Not implemented");
    }

    public Enumeration<?> getAppAttributes(AppsDatabaseFilter filter)
    {
        throw new Error("Not implemented");
    }

    public AppAttributes getAppAttributes(AppID key)
    {
        throw new Error("Not implemented");
    }

    public AppProxy getAppProxy(AppID key)
    {
        throw new Error("Not implemented");
    }

    public void addListener(AppsDatabaseEventListener listener)
    {
        throw new Error("Not implemented");
    }

    public void removeListener(AppsDatabaseEventListener listener)
    {
        throw new Error("Not implemented");
    }
}
