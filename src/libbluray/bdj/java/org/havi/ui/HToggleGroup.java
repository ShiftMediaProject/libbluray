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

public class HToggleGroup {
    private HToggleButton current = null;

    public HToggleGroup()
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "");
    }

    public HToggleButton getCurrent()
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "getCurrent");
        return current;
    }

    public void setCurrent(HToggleButton selection)
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "setCurrent");
        current = selection;
    }

    public void setForcedSelection(boolean forceSelection)
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "setForcedSelection");
    }

    public boolean getForcedSelection()
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "getForcedSelection");
        return false;
    }

    public void setEnabled(boolean enable)
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "setEnabled");
    }

    public boolean isEnabled()
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "isEnabled");
        return false;
    }

    protected void add(HToggleButton button)
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "add");
    }

    protected void remove(HToggleButton button)
    {
        org.videolan.Logger.unimplemented(HToggleGroup.class.getName(), "remove");
    }
}
