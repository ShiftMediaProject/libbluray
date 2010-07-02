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

import java.io.File;
import java.io.InterruptedIOException;
import java.net.URL;
import java.security.cert.X509Certificate;

public class DSMCCObject extends File {
    public DSMCCObject(String path)
    {
        super(path);

        this.path = path;
    }

    public DSMCCObject(String path, String name)
    {
        super(path, name);

        this.path = path;
        this.name = name;
    }

    public DSMCCObject(DSMCCObject dir, String name)
    {
        super(dir.path, name);

        this.path = dir.path;
        this.name = name;
    }

    public boolean isLoaded()
    {
        throw new Error("Not implemented");
    }

    public boolean isStream()
    {
        throw new Error("Not implemented");
    }

    public boolean isStreamEvent()
    {
        throw new Error("Not implemented");
    }

    public boolean isObjectKindKnown()
    {
        throw new Error("Not implemented");
    }

    public void synchronousLoad() throws InvalidFormatException,
            InterruptedIOException, MPEGDeliveryException,
            ServerDeliveryException, InvalidPathNameException,
            NotEntitledException, ServiceXFRException,
            InsufficientResourcesException
    {
        throw new Error("Not implemented");
    }

    public void asynchronousLoad(AsynchronousLoadingEventListener l)
            throws InvalidPathNameException
    {
        throw new Error("Not implemented");
    }

    public void abort() throws NothingToAbortException
    {
        throw new Error("Not implemented");
    }

    public static boolean prefetch(String path, byte priority)
    {
        throw new Error("Not implemented");
    }

    public static boolean prefetch(DSMCCObject dir, String path, byte priority)
    {
        throw new Error("Not implemented");
    }

    public void unload() throws NotLoadedException
    {
        throw new Error("Not implemented");
    }

    public URL getURL()
    {
        throw new Error("Not implemented");
    }

    public void addObjectChangeEventListener(ObjectChangeEventListener listener)
            throws InsufficientResourcesException
    {
        throw new Error("Not implemented");
    }

    public void removeObjectChangeEventListener(
            ObjectChangeEventListener listener)
    {
    }

    public void loadDirectoryEntry(AsynchronousLoadingEventListener l)
            throws InvalidPathNameException
    {
        throw new Error("Not implemented");
    }

    public void setRetrievalMode(int retrieval_mode)
    {
        throw new Error("Not implemented");
    }

    public X509Certificate[][] getSigners()
    {
        throw new Error("Not implemented");
    }

    public X509Certificate[][] getSigners(boolean known_root)
            throws InvalidFormatException, InterruptedIOException,
            MPEGDeliveryException, ServerDeliveryException,
            InvalidPathNameException, NotEntitledException,
            ServiceXFRException, InsufficientResourcesException
    {
        throw new Error("Not implemented");
    }

    public static final int FROM_CACHE = 1;
    public static final int FROM_CACHE_OR_STREAM = 2;
    public static final int FROM_STREAM_ONLY = 3;
    public static final int FORCED_STATIC_CACHING = 4;

    private String path;
    private String name;

    private static final long serialVersionUID = -6845145080873848152L;
}
