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

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import java.net.URL;
import java.net.URLConnection;

import org.videolan.BDJUtil;
import org.videolan.FontIndex;
import org.videolan.FontIndexData;
import org.videolan.Logger;

import sun.net.www.ParseUtil;

public class FontFactory {
    public static void loadDiscFonts() {
        unloadDiscFonts();

        fonts = FontIndex.parseIndex(BDJUtil.discRootToFilesystem("/BDMV/AUXDATA/dvb.fontindex"));

        if (fonts != null) {
            logger.trace("Disc fonts:");
            for (int i = 0; i < fonts.length; i++)
                System.err.println("    "+(i+1)+": " + fonts[i].toString());
        } else {
            logger.trace("No fonts loaded from disc");
        }
    }

    public static void unloadDiscFonts() {
        if (fonts != null) {
            fonts = null;
        }
    }

    public FontFactory() throws FontFormatException, IOException {
        if (fonts == null)
            throw new IOException("There is no font data");
    }

    public FontFactory(URL u) throws IOException, FontFormatException {
        String path = u.getPath().replace('/', File.separatorChar);
        path = ParseUtil.decode(path);
        if (u.getProtocol().equals("file")) {
            path = path.replace('/', File.separatorChar);
            file = new File(path);
        } else {
            int index = path.lastIndexOf('/');
            if (index >= 0)
                path = path.substring(index + 1);
            path = System.getProperty("bluray.vfs.root") + File.separator + path;

            URLConnection uc = u.openConnection();
            InputStream is = uc.getInputStream();
            FileOutputStream os = null;
            try {
                os = new FileOutputStream(path);
                byte[] buf = new byte[1024];
                int len;
                while ((len = is.read(buf)) > 0)
                    os.write(buf, 0, len);
                file = new File(path);
            } finally {
                try {
                    is.close();
                    os.close();
                } catch (Throwable e) {
                    file = null;
                }
            }
        }
        if (file == null)
            throw new IOException();
    }

    public Font createFont(String name, int style, int size)
            throws FontNotAvailableException, FontFormatException, IOException {
        logger.info("Creating font: " + name + " " + style + " " + size);

        return new Font(name, style, size);
    }

    private File file = null;
    private static FontIndexData[] fonts = null;

    private static final Logger logger = Logger.getLogger(FontFactory.class.getName());
}
