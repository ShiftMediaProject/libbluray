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

package org.dvb.ui;

import java.awt.Font;
import java.io.IOException;
import java.net.URL;
import java.util.logging.Logger;

public class FontFactory {
    public FontFactory() throws FontFormatException, IOException
    {
        throw new Error("Not implemented");
    }

    public FontFactory(URL u) throws IOException, FontFormatException
    {
        logger.info(u.toString());
        throw new Error("Not implemented");
    }

    public Font createFont(String name, int style, int size)
            throws FontNotAvailableException, FontFormatException, IOException
    {
        logger.info(name);
        throw new Error("Not implemented");
    }
    
    private static final Logger logger = Logger.getLogger(FontFactory.class.getName());
}
