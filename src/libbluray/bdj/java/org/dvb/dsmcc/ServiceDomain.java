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
package org.dvb.dsmcc;

import java.io.FileNotFoundException;
import java.io.InterruptedIOException;
import org.davic.net.InvalidLocatorException;
import org.davic.net.Locator;

public class ServiceDomain {
    public ServiceDomain()
    {
        throw new Error("Not implemented");
    }

    public void attach(org.davic.net.Locator aDVBService, int aCarouselId)
            throws ServiceXFRException, InterruptedIOException,
            MPEGDeliveryException
    {
        throw new Error("Not implemented");
    }

    public void attach(org.davic.net.Locator locator) throws DSMCCException,
            InterruptedIOException, MPEGDeliveryException
    {
        throw new Error("Not implemented");
    }

    public void attach(byte[] NSAPAddress) throws DSMCCException,
            InterruptedIOException, InvalidAddressException,
            MPEGDeliveryException
    {
        throw new Error("Not implemented");
    }

    public void detach() throws NotLoadedException
    {
        throw new Error("Not implemented");
    }

    public byte[] getNSAPAddress() throws NotLoadedException
    {
        throw new Error("Not implemented");
    }

    public static java.net.URL getURL(org.davic.net.Locator locator)
            throws NotLoadedException, InvalidLocatorException,
            FileNotFoundException
    {
        throw new Error("Not implemented");
    }

    public DSMCCObject getMountPoint()
    {
        throw new Error("Not implemented");
    }

    public boolean isNetworkConnectionAvailable()
    {
        throw new Error("Not implemented");
    }

    public boolean isAttached()
    {
        throw new Error("Not implemented");
    }

    public Locator getLocator()
    {
        throw new Error("Not implemented");
    }
}
