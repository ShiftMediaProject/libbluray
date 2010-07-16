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

package javax.media;

import java.io.IOException;
import java.net.URL;
import java.util.Vector;
import javax.media.protocol.DataSource;

public final class Manager {

    private Manager()
    {
    }

    public static Player createPlayer(URL sourceURL) throws IOException,
            NoPlayerException
    {
        throw new Error("Not implemented");
    }

    public static Player createPlayer(MediaLocator sourceLocator)
            throws IOException, NoPlayerException
    {
        throw new Error("Not implemented");
    }

    public static Player createPlayer(DataSource source) throws IOException,
            NoPlayerException
    {
        throw new Error("Not implemented");
    }

    public static DataSource createDataSource(URL sourceURL)
            throws IOException, NoDataSourceException
    {
        throw new Error("Not implemented");
    }

    public static DataSource createDataSource(MediaLocator sourceLocator)
            throws IOException, NoDataSourceException
    {
        throw new Error("Not implemented");
    }

    public static TimeBase getSystemTimeBase()
    {
        throw new Error("Not implemented");
    }

    public static Vector<?> getDataSourceList(String protocolName)
    {
        throw new Error("Not implemented");
    }

    public static Vector<?> getHandlerClassList(String contentName)
    {
        throw new Error("Not implemented");
    }

    public static final String UNKNOWN_CONTENT_NAME = "unknown";
}
