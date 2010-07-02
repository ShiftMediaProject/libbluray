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

public class TitleContextImpl implements TitleContext {

    @Override
    public void start(Title title, boolean doSomething)
            throws SecurityException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void addListener(ServiceContextListener listener)
    {
        serviceContextListeners.add(listener);
    }

    @Override
    public void destroy() throws SecurityException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public Service getService()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public ServiceContentHandler[] getServiceContentHandlers()
            throws SecurityException
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void removeListener(ServiceContextListener listener)
    {
        serviceContextListeners.remove(listener);
    }

    @Override
    public void select(Service service) throws SecurityException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void select(Locator[] locators) throws InvalidLocatorException,
            InvalidServiceComponentException, SecurityException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void stop() throws SecurityException
    {
        // TODO Auto-generated method stub
        
    }

    private ArrayList<ServiceContextListener> serviceContextListeners = new ArrayList<ServiceContextListener>();
}
