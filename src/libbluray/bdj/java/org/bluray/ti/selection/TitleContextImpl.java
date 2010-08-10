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
package org.bluray.ti.selection;

import java.util.ArrayList;

import javax.tv.locator.InvalidLocatorException;
import javax.tv.locator.Locator;
import javax.tv.service.Service;
import javax.tv.service.selection.InvalidServiceComponentException;
import javax.tv.service.selection.ServiceContentHandler;
import javax.tv.service.selection.ServiceContextListener;

import org.bluray.ti.Title;
import org.bluray.ti.TitleImpl;
import org.videolan.Libbluray;

public class TitleContextImpl implements TitleContext {

    public void start(Title title, boolean doSomething)
            throws SecurityException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void addListener(ServiceContextListener listener)
    {
        serviceContextListeners.add(listener);
    }

    public void destroy() throws SecurityException
    {
        
    }

    public Service getService()
    {
        return new TitleImpl(Libbluray.getCurrentTitle());
    }

    public ServiceContentHandler[] getServiceContentHandlers()
            throws SecurityException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void removeListener(ServiceContextListener listener)
    {
        serviceContextListeners.remove(listener);
    }

    public void select(Service service) throws SecurityException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void select(Locator[] locators) throws InvalidLocatorException,
            InvalidServiceComponentException, SecurityException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void stop() throws SecurityException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    private ArrayList<ServiceContextListener> serviceContextListeners = new ArrayList<ServiceContextListener>();
}
