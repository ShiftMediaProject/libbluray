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

package org.dvb.media;

import javax.media.Time;
import java.io.IOException;
import javax.media.protocol.DataSource;

public class DripFeedDataSource extends DataSource {
    public DripFeedDataSource()
    {
        throw new Error("Not implemented");
    }

    public void feed(byte[] clip_part)
    {
        throw new Error("Not implemented");
    }

    public String getContentType()
    {
        throw new Error("Not implemented");
    }

    public void connect() throws IOException
    {
        throw new Error("Not implemented");
    }

    public void disconnect()
    {
        throw new Error("Not implemented");
    }

    public void start() throws IOException
    {
        throw new Error("Not implemented");
    }

    public void stop() throws IOException
    {
        throw new Error("Not implemented");
    }

    public Time getDuration()
    {
        throw new Error("Not implemented");
    }

    public Object[] getControls()
    {
        throw new Error("Not implemented");
    }

    public Object getControl(String controlType)
    {
        throw new Error("Not implemented");
    }

}
