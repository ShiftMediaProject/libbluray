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

public class HGraphicsDevice extends HScreenDevice {
    protected HGraphicsDevice()
    {

    }

    public HGraphicsConfiguration[] getConfigurations()
    {
        throw new Error("Not implemented");
    }

    public HGraphicsConfiguration getDefaultConfiguration()
    {
        throw new Error("Not implemented");
    }

    public HGraphicsConfiguration getBestConfiguration(
            HGraphicsConfigTemplate hgct)
    {
        throw new Error("Not implemented");
    }

    public HGraphicsConfiguration getBestConfiguration(
            HGraphicsConfigTemplate hgcta[])
    {
        throw new Error("Not implemented");
    }

    public HGraphicsConfiguration getCurrentConfiguration()
    {
        throw new Error("Not implemented");
    }

    public boolean setGraphicsConfiguration(HGraphicsConfiguration hgc)
            throws SecurityException, HPermissionDeniedException,
            HConfigurationException
    {
        throw new Error("Not implemented");
    }
}
