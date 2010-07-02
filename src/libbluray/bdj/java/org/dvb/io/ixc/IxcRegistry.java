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

package org.dvb.io.ixc;

import java.rmi.RemoteException;
import java.rmi.NotBoundException;
import java.rmi.AlreadyBoundException;
import java.rmi.Remote;

import javax.tv.xlet.XletContext;

public class IxcRegistry {
    public static Remote lookup(XletContext xc, String path)
            throws NotBoundException, RemoteException
    {
        throw new Error("Not implemented");
    }

    public static void bind(XletContext xc, String name, Remote obj)
            throws AlreadyBoundException
    {
        throw new Error("Not implemented");
    }

    public static void unbind(XletContext xc, String name)
            throws NotBoundException
    {
        throw new Error("Not implemented");
    }

    public static void rebind(XletContext xc, String name, Remote obj)
    {
        throw new Error("Not implemented");
    }

    public static String[] list(XletContext xc)
    {
        throw new Error("Not implemented");
    }
}
