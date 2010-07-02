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

package org.dvb.user;

public abstract class Preference {
    protected Preference()
    {
        throw new Error("Not implemented");
    }

    public Preference(String name, String value)
    {
        throw new Error("Not implemented");
    }

    public Preference(String name, String value[])
    {
        throw new Error("Not implemented");
    }

    public void add(String value)
    {
        throw new Error("Not implemented");
    }

    public void add(String values[])
    {
        throw new Error("Not implemented");
    }

    public void add(int position, String value)
    {
        throw new Error("Not implemented");
    }

    public String[] getFavourites()
    {
        throw new Error("Not implemented");
    }

    public String getMostFavourite()
    {
        throw new Error("Not implemented");
    }

    public String getName()
    {
        throw new Error("Not implemented");
    }

    public int getPosition(String value)
    {
        throw new Error("Not implemented");
    }

    public boolean hasValue()
    {
        throw new Error("Not implemented");
    }

    public void remove(String value)
    {
        throw new Error("Not implemented");
    }

    public void removeAll()
    {
        throw new Error("Not implemented");
    }

    public void setMostFavourite(String value)
    {
        throw new Error("Not implemented");
    }

    public String toString()
    {
        throw new Error("Not implemented");
    }
}
