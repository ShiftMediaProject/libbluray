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

package org.videolan;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.tv.xlet.Xlet;

import org.videolan.bdjo.AppCache;
import org.videolan.bdjo.AppEntry;
import org.videolan.bdjo.Bdjo;
import org.videolan.bdjo.ControlCode;

public class BDJLoader {
    public static void Load(String baseDir, Bdjo bdjo, long nativePointer)
    {
        try { 
            System.loadLibrary("bluray"); // load libbluray.so
            
            Libbluray.nativePointer = nativePointer;
            BDJLoader.baseDir = baseDir;
            
            BDJClassLoader classLoader = new BDJClassLoader(baseDir);
            
            // add app caches (the locations where classes are stored)
            for (AppCache cache : bdjo.getAppCaches())
                classLoader.addAppCache(cache);
            
            // now load and initialize all the xlets
            for (AppEntry entry : bdjo.getAppTable()) {
                Class<?> xlet_class = classLoader.loadClass(entry.getInitialClass());
                logger.log(Level.INFO, "Loaded class: " + entry.getInitialClass());
                
                xlet = (Xlet)xlet_class.newInstance();
                
                // make context for xlet
                BasicXletContext context = new BasicXletContext(entry);
                
                logger.log(Level.INFO, "Attempting to init a xlet");
                xlet.initXlet(context);
                
                if (entry.getControlCode().equals(ControlCode.AUTOSTART)) {
                    logger.log(Level.INFO, "Autostart requested, now starting xlet.");
                    xlet.startXlet();
                }
            }
            
            logger.log(Level.INFO, "Finished initializing and starting xlets.");

        } catch (Throwable e) {
            logger.log(Level.SEVERE, "Failed to start xlet");
            e.printStackTrace();
        }
    }

    public static void Shutdown()
    {
        try {
            if (xlet != null) {
                xlet.destroyXlet(true);
                xlet = null;
            }
            
            MountManager.unmountAll();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    protected static String getBaseDir()
    {
        return baseDir;
    }
    
    private static Xlet xlet = null;
    private static String baseDir = "";
    private static final Logger logger = Logger.getLogger(BDJLoader.class.getName());
}
