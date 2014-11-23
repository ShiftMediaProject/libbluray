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
import java.awt.FontFormatException;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import org.videolan.BDJUtil;
import org.videolan.FontIndex;
import org.videolan.FontIndexData;
import org.videolan.Logger;

public class FontFactory {
    public static synchronized void loadDiscFonts() {
        unloadDiscFonts();
    }

    public static synchronized void unloadDiscFonts() {
        fonts = null;
        fontIds = null;
    }

    private static synchronized void readDiscFonts() throws FontFormatException, IOException {
        if (fonts != null)
            return;

        String path = BDJUtil.discRootToFilesystem("/BDMV/AUXDATA/dvb.fontindex");
        FontIndexData fontIndexData[] = FontIndex.parseIndex(path);

        fonts = new HashMap(fontIndexData.length);
        fontIds = new HashMap(fontIndexData.length);
        for (int i = 0; i < fontIndexData.length; i++) {
            FontIndexData data = fontIndexData[i];
            try {
                File fontFile = org.videolan.BDJLoader.addFont(data.getFileName());
                if (fontFile == null) {
                    throw new IOException("error caching font");
                }

                if (data.getStyle() == -1) {
                    logger.unimplemented("readDiscFonts(): font with all styles not supported");
                }

                Font font = Font.createFont(Font.TRUETYPE_FONT, fontFile);
                font = font.deriveFont(data.getStyle(), data.getMaxSize());

                fonts.put(data.getName(), font);
                fontIds.put(data.getFileName().substring(0, 5), font);

            } catch (IOException ex) {
                logger.error("Failed reading font " + data.getName() + " from " + data.getFileName() + ": " + ex);
                if (i == fontIndexData.length - 1 && fonts.size() < 1) {
                    logger.error("didn't load any fonts !");
                    throw ex;
                }
            } catch (FontFormatException ex) {
                logger.error("Failed reading font " + data.getName() + " from " + data.getFileName() + ": " + ex);
                if (i == fontIndexData.length - 1 && fonts.size() < 1) {
                    logger.error("didn't load any fonts !");
                    throw ex;
                }
            }
        }

    }

    public FontFactory() throws FontFormatException, IOException {
        readDiscFonts();
    }

    public FontFactory(URL u) throws IOException, FontFormatException {
        InputStream inStream = null;

        try {
            inStream = u.openStream();

            File fontFile = org.videolan.BDJLoader.addFont(inStream);
            if (fontFile == null) {
                throw new IOException("error caching font");
            }

            urlFont = Font.createFont(Font.TRUETYPE_FONT, fontFile);

        } catch (IOException ex) {
            logger.error("Failed reading font from " + u.getPath() + ": " + ex);
            throw ex;
        } catch (FontFormatException ex) {
            logger.error("Failed reading font from " + u.getPath() + ": " + ex);
            throw ex;
        } finally {
            if (inStream != null) {
                inStream.close();
            }
        }
    }

    public Font createFont(String fontId) {
        Font font = null;
        synchronized (FontFactory.class) {
            font = (Font)fontIds.get(fontId);
        }
        if (font != null) {
            return font.deriveFont(0, 12);
        }
        return null;
    }

    public Font createFont(String name, int style, int size)
            throws FontNotAvailableException, FontFormatException, IOException {
        logger.info("Creating font: " + name + " " + style + " " + size);

        if (urlFont != null && name.equals(urlFont.getName()))
        {
            return urlFont.deriveFont(style, size);
        }

        Font font = null;
        synchronized (FontFactory.class) {
            font = (Font)fonts.get(name);
        }

        if (font == null) {
            logger.info("Failed creating font: " + name + " " + style + " " + size);
            throw new FontNotAvailableException();
        }

        return font.deriveFont(style, size);
    }

    private Font urlFont = null;

    private static Map fonts = null;
    private static Map fontIds = null;

    private static final Logger logger = Logger.getLogger(FontFactory.class.getName());
}
