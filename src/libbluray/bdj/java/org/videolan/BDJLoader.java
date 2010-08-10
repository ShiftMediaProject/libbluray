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

import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.media.PackageManager;
import javax.tv.xlet.Xlet;
import javax.tv.xlet.XletContext;
import javax.tv.xlet.XletStateChangeException;

import org.videolan.bdjo.AppCache;
import org.videolan.bdjo.AppEntry;
import org.videolan.bdjo.Bdjo;
import org.videolan.bdjo.ControlCode;
import org.videolan.bdjo.GraphicsResolution;

public class BDJLoader {
    public static void Load(String baseDir, Bdjo bdjo, long nativePointer)
    {
        try { 
            System.loadLibrary("bluray"); // load libbluray.so
            
            Libbluray.nativePointer = nativePointer;
            BDJLoader.baseDir = baseDir;
            BDJLoader.bdjo = bdjo;
            
            System.setProperty("bluray.vfs.root", baseDir);
            
            BDJClassLoader classLoader = new BDJClassLoader(baseDir);
            
            // setup JMF prefixes
            Vector<String> prefix = new Vector<String>();
            prefix.add("org.videolan");
            PackageManager.setContentPrefixList(prefix);
            PackageManager.setProtocolPrefixList(prefix);
            PackageManager.commitContentPrefixList();
            PackageManager.commitProtocolPrefixList();
            
            // add app caches (the locations where classes are stored)
            for (AppCache cache : bdjo.getAppCaches())
                classLoader.addAppCache(cache);
            
            // start test window
            GUIManager gui = GUIManager.getInstance();
            GraphicsResolution res = bdjo.getTerminalInfo().getResolution();
            gui.setSize(res.getWidth(), res.getHeight());
            gui.setVisible(true);
            
            // now load and initialize all the xlets
            AppEntry[] appTable = bdjo.getAppTable();
            xlet = new Xlet[appTable.length];
            for (int i = 0; i < appTable.length; i++) {
                AppEntry entry = appTable[i];
                Class<?> xlet_class = classLoader.loadClass(entry.getInitialClass());
                logger.log(Level.INFO, "Loaded class: " + entry.getInitialClass());
                
                xlet[i] = (Xlet)xlet_class.newInstance();
                
                // make context for xlet
                BasicXletContext context = new BasicXletContext(entry);
                
                Thread thread = new Thread(new BDJThreadGroup("", context), 
                        new XletStarter(xlet[i], entry.getControlCode().equals(ControlCode.AUTOSTART)));
                thread.start();
                thread.join();
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
                for (int i = 0; i < xlet.length; i++){
                    xlet[i].destroyXlet(true);
                    xlet[i] = null;
                }
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
    
    public static void SendKeyEvent(int type, int keyCode)
    {
        if (inputListener != null) {
            inputListener.receiveKeyEvent(type, keyCode);
        } else {
            logger.warning("Tried to send key event before listener set.");
        }
    }
    
    public static XletContext getContext()
    {
        BDJThreadGroup grp = (BDJThreadGroup)Thread.currentThread().getThreadGroup();
        return grp.getContext();
    }
    
    public static Bdjo getBdjo()
    {
        return bdjo;
    }
    
    public static BDJInputListener inputListener = null;
    
    private static Xlet[] xlet = null;
    private static Bdjo bdjo = null;
    private static String baseDir = "";
    private static final Logger logger = Logger.getLogger(BDJLoader.class.getName());

    private static class XletStarter implements Runnable {
        public XletStarter(Xlet xlet, boolean autostart)
        {
            this.xlet = xlet;
            this.autostart = autostart;
        }
        
        public void run()
        {
            try {
                XletContext context = getContext();
                logger.log(Level.INFO, "Attempting to init a xlet");
                
                xlet.initXlet(context);
                
                if (autostart) {
                    logger.log(Level.INFO, "Autostart requested, now starting xlet.");
                    xlet.startXlet();
                }
            } catch (XletStateChangeException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        
        private Xlet xlet;
        private boolean autostart;
    }
}
