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

package javax.tv.service;

import javax.tv.locator.InvalidLocatorException;
import javax.tv.locator.Locator;
import javax.tv.service.navigation.ServiceFilter;
import javax.tv.service.navigation.ServiceList;
import javax.tv.service.transport.Transport;

public class SIManagerImpl extends SIManager {

    @Override
    public ServiceList filterServices(ServiceFilter filter)
    {
        throw new Error("Not implemented");
    }

    @Override
    public String getPreferredLanguage()
    {
        throw new Error("Not implemented");
    }

    @Override
    public RatingDimension getRatingDimension(String name) throws SIException
    {
        throw new Error("Not implemented");
    }

    @Override
    public Service getService(Locator locator) throws InvalidLocatorException,
            SecurityException
    {
        throw new Error("Not implemented");
    }

    @Override
    public String[] getSupportedDimensions()
    {
        throw new Error("Not implemented");
    }

    @Override
    public Transport[] getTransports()
    {
        throw new Error("Not implemented");
    }

    @Override
    public void registerInterest(Locator locator, boolean active)
            throws InvalidLocatorException, SecurityException
    {
        throw new Error("Not implemented");
        
    }

    @Override
    public SIRequest retrieveProgramEvent(Locator locator, SIRequestor requestor)
            throws InvalidLocatorException, SecurityException
    {
        throw new Error("Not implemented");
    }

    @Override
    public SIRequest retrieveSIElement(Locator locator, SIRequestor requestor)
            throws InvalidLocatorException, SecurityException
    {
        throw new Error("Not implemented");
    }

    @Override
    public SIRequest retrieveServiceDetails(Locator locator,
            SIRequestor requestor) throws InvalidLocatorException,
            SecurityException
    {
        throw new Error("Not implemented");
    }

    @Override
    public void setPreferredLanguage(String language)
    {
        throw new Error("Not implemented");
    }

}
