/*
 * This file is part of libbluray
 * Copyright (C) 2014  Petri Hintukainen <phintuka@users.sourceforge.net>
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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.io.BDFileSystem;
import org.videolan.bdjo.AppCache;

class VFSCache {

    protected static VFSCache createInstance() {
        VFSCache cache = new VFSCache();
        try {
            cache.init();
        } catch (IOException e) {
            return null;
        }
        return cache;
    }

    private VFSCache() {}

    private void init() throws IOException {

        String disable = System.getProperty("org.videolan.vfscache");
        if (disable != null && disable.equals("NO")) {
            logger.error ("Cache disabled !");
            throw new IOException();
        }

        cacheRoot = CacheDir.create("VFSCache").getPath() + File.separator;
        fontRoot  = CacheDir.create("Font").getPath() + File.separator;
        vfsRoot   = System.getProperty("bluray.vfs.root");
        if (!vfsRoot.endsWith(File.separator)) {
            vfsRoot = vfsRoot + File.separator;
        }
        vfsRootLength = vfsRoot.length();

        new File(cacheRoot + jarDir).mkdirs();
    }

    /*
     *
     */

    private boolean copyStream(InputStream inStream, String dstPath) {
        OutputStream outStream = null;
        byte[] buffer = new byte[64*1024];
        IOException exception = null;

        try {
            int length;
            outStream = new FileOutputStream(dstPath);
            while ((length = inStream.read(buffer)) > 0) {
                outStream.write(buffer, 0, length);
            }

        } catch (IOException e) {
            exception = e;

        } finally {
            if (outStream != null) {
                try {
                    outStream.close();
                } catch (IOException e) {
                    if (exception == null)
                        exception = e;
                }
            }
        }

        if (exception != null) {
            logger.error("Error caching to " + dstPath + ": " + exception);
            new File(dstPath).delete();
            return false;
        }

        //logger.info("Cached file " + srcPath + " to " + dstPath);
        return true;
    }

    private boolean copyFile(String srcPath, String dstPath) {
        InputStream inStream = null;
        IOException exception = null;
        boolean result = false;

        try {
            inStream = new FileInputStream(srcPath);
            result = copyStream(inStream, dstPath);

        } catch (IOException e) {
            exception = e;

        } finally {
            if (inStream != null) {
                try {
                    inStream.close();
                } catch (IOException e) {
                    if (exception == null)
                        exception = e;
                }
            }
        }

        if (exception != null) {
            logger.error("Error caching " + srcPath + ": " + exception);
            new File(dstPath).delete();
            return false;
        }

        //logger.info("Cached file " + srcPath + " to " + dstPath);
        return result;
    }

    private void copyJarFile(String name) {
        /* copy file from BDMV/JAR/ */

        String relPath = jarDir + name;
        String dstPath = cacheRoot + relPath;

        if (BDFileSystem.nativeFileExists(dstPath)) {
            //logger.info(dstPath + " already cached");
            return;
        }

        Libbluray.cacheBdRomFile(relPath, dstPath);

        logger.info("cached " + relPath);
    }

    private void copyJarDir(String name) {
        /* copy directory from BDMV/JAR/ */

        new File(cacheRoot + jarDir + name).mkdirs();

        File[] files = new File(vfsRoot + jarDir + name).listFiles();
        for (int i = 0; i < files.length; i++) {
            File   file    = files[i];
            String relPath = name + File.separator + files[i].getName();
            if (file.isDirectory()) {
                copyJarDir(relPath);
            } else {
                copyJarFile(relPath);
            }
        }
        logger.info("cached " + name);
    }

    /*
     * Add files from BD-ROM filesystem to cache
     * Called by BDJLoader when starting the title
     */
    protected synchronized void add(AppCache[] appCaches) {

        for (int i = 0; i < appCaches.length; i++) {
            if (appCaches[i].getType() == AppCache.JAR_FILE) {
                copyJarFile(appCaches[i].getRefToName() + ".jar");
            } else if (appCaches[i].getType() == AppCache.DIRECTORY) {
                copyJarDir(appCaches[i].getRefToName());
            } else {
                logger.error("unknown AppCache type " + appCaches[i].getType());
            }
        }
    }

    protected synchronized File addFont(String fontFile) {

        new File(fontRoot + fontDir).mkdirs();

        String relPath = fontDir + fontFile;
        String dstPath = fontRoot + relPath;
        File dstFile = new File(dstPath);

        if (BDFileSystem.nativeFileExists(dstPath)) {
            //logger.info(dstPath + " already cached");
            return dstFile;
        }

        if (!Libbluray.cacheBdRomFile(relPath, dstPath)) {
            return null;
        }

        logger.info("cached font " + fontFile);
        return dstFile;
    }

    protected synchronized File addFont(InputStream is) {

        new File(fontRoot).mkdirs();

        // copy stream to tmp file in fontRoot. freetype can not read streams.
        File tmpFile = null;
        for (int i = 0; i < 100; i++) {
            tmpFile = new File(fontRoot + System.nanoTime() + ".otf");
            try {
                tmpFile = new File(tmpFile.getCanonicalPath());
                if (!tmpFile.exists()) {
                    break;
                }
            } catch (IOException ex) {
                logger.error("got " + ex);
            }
            tmpFile = null;
        }
        if (tmpFile == null) {
            logger.error("error creating temporary font file");
            return null;
        }

        if (!copyStream(is, tmpFile.getPath())) {
            return null;
        }

        logger.info("cached font stream to file " + tmpFile.getPath());
        return tmpFile;
    }

    /*
     * Add file from binding unit data area to cache
     */
    protected synchronized boolean add(String vpFile, String budaFile) {

        String srcPath = System.getProperty("bluray.bindingunit.root") + File.separator + budaFile;
        String dstPath = cacheRoot + vpFile;

        return copyFile(srcPath, dstPath);
    }

    /*
     * Check if file is cached.
     *
     * absPath: path in BD VFS.
     * return: path of cached file, absPath if file is not in cache.
     */
    public synchronized String map(String absPath) {

        if (!absPath.startsWith(vfsRoot)) {
            //logger.info(absPath + " not in BDMV/JAR");
            return absPath;
        }

        String cachePath = cacheRoot + absPath.substring(vfsRootLength);
        if (!BDFileSystem.nativeFileExists(cachePath)) {
            //logger.info(cachePath + " not in VFS cache");
            return absPath;
        }

        logger.info("using cached " + cachePath);
        return cachePath;
    }

    private String cacheRoot = null;
    private String vfsRoot = null;
    private String fontRoot = null;
    private int    vfsRootLength = 0;

    private static final String jarDir = "BDMV" + File.separator + "JAR" + File.separator;
    private static final String fontDir = "BDMV" + File.separator + "AUXDATA" + File.separator;

    private static final Logger logger = Logger.getLogger(VFSCache.class.getName());
}
