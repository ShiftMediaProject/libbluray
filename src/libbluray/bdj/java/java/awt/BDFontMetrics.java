/*
 * This file is part of libbluray
 * Copyright (C) 2012  libbluray
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

package java.awt;

import java.io.*;
import java.security.*;
import java.util.*;

public class BDFontMetrics extends FontMetrics {
    static final long serialVersionUID = -4956160226949100590L;

    private static long ftLib = 0;
    private static Map fontNameMap;

    private static native long initN();
    private static native void destroyN(long ftLib);

    public static void init() {
        //System.loadLibrary("bluray");

        if (ftLib != 0)
            return;

        ftLib = initN();

        if (ftLib == 0)
            throw new AWTError("freetype lib not loaded");

        Runtime.getRuntime().addShutdownHook(
            new Thread() {
                public void run() {
                    Iterator it = fontMetricsMap.values().iterator();
                    while (it.hasNext()) {
                        try {
                            BDFontMetrics fm = (BDFontMetrics)it.next();
                            it.remove();
                            fm.finalize();
                        } catch (Throwable e) {
                            e.printStackTrace();
                        }
                    }
                    BDFontMetrics.destroyN(BDFontMetrics.ftLib);
                }
            }
        );

        String javaHome = (String) AccessController.doPrivileged(new PrivilegedAction() {
                public Object run() {
                    return System.getProperty("java.home");
                }
            }
        );
        File f = new File(javaHome, "lib" + File.separator + "fonts");
        String dir = f.getAbsolutePath() + File.separator;

        fontNameMap = new HashMap(24);
        fontNameMap.put("serif.0", dir + "LucidaBrightRegular.ttf");
        fontNameMap.put("serif.1", dir + "LucidaBrightDemiBold.ttf");
        fontNameMap.put("serif.2", dir + "LucidaBrightItalic.ttf");
        fontNameMap.put("serif.3", dir + "LucidaBrightDemiItalic.ttf");
        fontNameMap.put("sansserif.0", dir + "LucidaSansRegular.ttf");
        fontNameMap.put("sansserif.1", dir + "LucidaSansDemiBold.ttf");
        fontNameMap.put("sansserif.2", dir + "LucidaSansOblique.ttf");
        fontNameMap.put("sansserif.3", dir + "LucidaSansDemiOblique.ttf");
        fontNameMap.put("monospaced.0", dir + "LucidaTypewriterRegular.ttf");
        fontNameMap.put("monospaced.1", dir + "LucidaTypewriterBold.ttf");
        fontNameMap.put("monospaced.2", dir + "LucidaTypewriterOblique.ttf");
        fontNameMap.put("monospaced.3", dir + "LucidaTypewriterBoldOblique.ttf");
        fontNameMap.put("dialog.0", dir + "LucidaSansRegular.ttf");
        fontNameMap.put("dialog.1", dir + "LucidaSansDemiBold.ttf");
        fontNameMap.put("dialog.2", dir + "LucidaSansOblique.ttf");
        fontNameMap.put("dialog.3", dir + "LucidaSansDemiOblique.ttf");
        fontNameMap.put("dialoginput.0", dir + "LucidaTypewriterRegular.ttf");
        fontNameMap.put("dialoginput.1", dir + "LucidaTypewriterBold.ttf");
        fontNameMap.put("dialoginput.2", dir + "LucidaTypewriterOblique.ttf");
        fontNameMap.put("dialoginput.3", dir + "LucidaTypewriterBoldOblique.ttf");
        fontNameMap.put("default.0", dir + "LucidaSansRegular.ttf");
        fontNameMap.put("default.1", dir + "LucidaSansDemiBold.ttf");
        fontNameMap.put("default.2", dir + "LucidaSansOblique.ttf");
        fontNameMap.put("default.3", dir + "LucidaSansDemiOblique.ttf");
    }

    /** A map which maps a native font name and size to a font metrics object. This is used
     as a cache to prevent loading the same fonts multiple times. */
    private static Map fontMetricsMap = new HashMap();

    /** Gets the BDFontMetrics object for the supplied font. This method caches font metrics
     to ensure native fonts are not loaded twice for the same font. */
    static synchronized BDFontMetrics getFontMetrics(Font font) {
        /* See if metrics has been stored in font already. */
        BDFontMetrics fm = null;
        //BDFontMetrics fm = (BDFontMetrics)font.metrics;
        //if (fm == null) {
            /* See if a font metrics of the same native name and size has already been loaded.
             If it has then we use that one. */
            String nativeName = (String)fontNameMap.get(font.getName().toLowerCase() + "." + font.getStyle());
            if (nativeName == null)
                nativeName = (String)fontNameMap.get("default." + font.getStyle());
            String key = nativeName + "." + font.getSize();
            fm = (BDFontMetrics)fontMetricsMap.get(key);
            if (fm == null)
                fontMetricsMap.put(key, fm = new BDFontMetrics(font, nativeName));
            //font.metrics = fm;
        //}
        return fm;
    }

    static String[] getFontList() {
        init();

        ArrayList fontNames = new ArrayList();
        Iterator fonts = fontNameMap.keySet().iterator();
        int dotidx;

        while (fonts.hasNext()) {
            String fontname = (String) fonts.next();
            if ((dotidx = fontname.indexOf('.')) == -1)
                dotidx = fontname.length();
            fontname = fontname.substring(0, dotidx);
            if (!fontNames.contains(fontname))
                fontNames.add(fontname);
        }
        return (String[])fontNames.toArray(new String[fontNames.size()]);
    }

    public static void registerFont(String name, int style, String path) {
        File f = new File(path);
        path = f.getAbsolutePath();
        if (path != null) {
            name = name.toLowerCase() + "." + style;
            fontNameMap.put(name, path);
        }
    }

    public static boolean registerFont(File f) {
        //TODO
        org.videolan.Logger.unimplemented("BDFontMetrics", "registerFont");
        return false;
    }

    public static void unregisterFont(String name, int style) {
        name = name.toLowerCase() + "." + style;
        fontNameMap.remove(name);
    }

    long ftFace;
    private int ascent;
    private int descent;
    private int leading;
    private int maxAdvance;

    /** Cache of first 256 Unicode characters as these map to ASCII characters and are often used. */
    private int[] widths;

    /**
     * Creates a font metrics for the supplied font. To get a font metrics for a font
     * use the static method getFontMetrics instead which does caching.
     */
    private BDFontMetrics(Font font, String nativeName) {
        super(font);

        ftFace = loadFontN(ftLib, nativeName, font.getSize());
        if (ftFace == 0)
            throw new AWTError("font face:" + nativeName + " not loaded");

        /* Cache first 256 char widths for use by the getWidths method and for faster metric
         calculation as they are commonly used (ASCII) characters. */
        widths = new int[256];
        for (int i = 0; i < 256; i++)
            widths[i] = charWidthN(ftFace, (char)i);
    }

    private native long loadFontN(long ftLib, String fontName, int size);
    private native void destroyFontN(long ftFace);
    private native int charWidthN(long ftFace, char c);
    private native int stringWidthN(long ftFace, String string);
    private native int charsWidthN(long ftFace, char chars[], int offset, int len);

    public int getAscent() {
        return ascent;
    }

    public int getDescent() {
        return descent;
    }

    public int getLeading() {
        return leading;
    }

    public int getMaxAdvance() {
        return maxAdvance;
    }

    /**
     * Fast lookup of first 256 chars as these are always the same eg. ASCII charset.
     */
    public int charWidth(char c) {
        if (c < 256)
            return widths[c];
        return charWidthN(ftFace, c);
    }

    /**
     * Return the width of the specified string in this Font.
     */
    public int stringWidth(String string) {
        return stringWidthN(ftFace, string);
    }

    /**
     * Return the width of the specified char[] in this Font.
     */
    public int charsWidth(char chars[], int offset, int length) {
        return charsWidthN(ftFace, chars, offset, length);
    }

    /**
     * Get the widths of the first 256 characters in the font.
     */
    public int[] getWidths() {
        int[] newWidths = new int[256];
        System.arraycopy(widths, 0, newWidths, 0, 256);
        return newWidths;
    }

    protected void finalize() throws Throwable {
        if (ftFace != 0) {
            destroyFontN(ftFace);
            ftFace = 0;
        }
        super.finalize();
    }
}
