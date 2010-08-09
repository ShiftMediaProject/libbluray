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

package org.videolan.media.content.playlist;

import java.awt.Component;

import javax.tv.locator.InvalidLocatorException;
import javax.tv.locator.Locator;
import javax.tv.media.MediaSelectListener;
import javax.tv.service.selection.InsufficientResourcesException;
import javax.tv.service.selection.InvalidServiceComponentException;

import org.dvb.media.DVBMediaSelectControl;

public class DVBMediaSelectControlImpl implements DVBMediaSelectControl{

    public void select(Locator component) throws InvalidLocatorException,
            InvalidServiceComponentException, InsufficientResourcesException,
            SecurityException
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public void select(Locator[] components) throws InvalidLocatorException,
            InvalidServiceComponentException, InsufficientResourcesException,
            SecurityException
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public void add(Locator component) throws InvalidLocatorException,
            InvalidServiceComponentException, InsufficientResourcesException,
            SecurityException
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public void remove(Locator component) throws InvalidLocatorException,
            InvalidServiceComponentException, SecurityException
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public void replace(Locator fromComponent, Locator toComponent)
            throws InvalidLocatorException, InvalidServiceComponentException,
            InsufficientResourcesException, SecurityException
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public void addMediaSelectListener(MediaSelectListener listener)
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public void removeMediaSelectListener(MediaSelectListener listener)
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public Locator[] getCurrentSelection()
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public Component getControlComponent()
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

    public void selectServiceMediaComponents(Locator locator)
            throws InvalidLocatorException, InvalidServiceComponentException,
            InsufficientResourcesException
    {
        throw new Error("Not implemented"); // TODO: Not implemented
    }

}
