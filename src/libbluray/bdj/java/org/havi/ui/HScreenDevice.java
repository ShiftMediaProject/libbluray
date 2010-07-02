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

import org.davic.resources.ResourceClient;
import org.davic.resources.ResourceProxy;
import org.davic.resources.ResourceServer;
import org.davic.resources.ResourceStatusListener;
import org.havi.ui.event.HScreenConfigurationListener;

public class HScreenDevice implements ResourceProxy, ResourceServer {
    HScreenDevice()
    {

    }

    public String getIDstring()
    {
        throw new Error("Not implemented");
    }

    public void addScreenConfigurationListener(HScreenConfigurationListener hscl)
    {
        throw new Error("Not implemented");
    }

    public void addScreenConfigurationListener(
            HScreenConfigurationListener hscl, HScreenConfigTemplate hsct)
    {
        throw new Error("Not implemented");
    }

    public void removeScreenConfigurationListener(
            HScreenConfigurationListener hscl)
    {
        throw new Error("Not implemented");
    }

    public Dimension getScreenAspectRatio()
    {
        throw new Error("Not implemented");
    }

    public boolean reserveDevice(ResourceClient client)
    {
        throw new Error("Not implemented");
    }

    public void releaseDevice()
    {
        throw new Error("Not implemented");
    }

    public ResourceClient getClient()
    {
        throw new Error("Not implemented");
    }

    public void addResourceStatusEventListener(ResourceStatusListener listener)
    {
        throw new Error("Not implemented");
    }

    public void removeResourceStatusEventListener(
            ResourceStatusListener listener)
    {
        throw new Error("Not implemented");
    }
}
