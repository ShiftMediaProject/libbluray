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
import java.io.IOException;

import java.io.BDFileSystem;

class CacheDir {

    private static LockFile lockCache(String path) {
        return LockFile.create(path + File.separator + "lock");
    }

    private static void cleanupCache() {
        File baseDirFile = new File(baseDir);
        String[] files = BDFileSystem.nativeList(baseDirFile);
        if (files != null) {
            for (int i = 0; i < files.length; i++) {
                File dir = new File(baseDirFile, files[i]);
                if (dir.isDirectory()) {
                    LockFile lock = lockCache(dir.getPath());
                    if (lock != null) {
                        lock.release();
                        removeImpl(dir);
                    }
                }
            }
        }
    }

    private static synchronized File getCacheRoot() throws IOException {

        if (cacheRoot != null) {
            return cacheRoot;
        }

        SecurityManager sm = System.getSecurityManager();
        if (sm != null && sm instanceof BDJSecurityManager) {
            ((BDJSecurityManager)sm).setCacheRoot(baseDir);
        }

        cleanupCache();

        for (int i = 0; i < 100; i++) {
            File tmpDir = new File(baseDir + System.nanoTime());
            tmpDir = new File(tmpDir.getCanonicalPath());

            if (tmpDir.mkdirs()) {
                cacheRoot = tmpDir;
                lockFile  = lockCache(cacheRoot.getPath());
                logger.info("Created cache in " + tmpDir.getPath());

                if (sm != null && sm instanceof BDJSecurityManager) {
                    ((BDJSecurityManager)sm).setCacheRoot(cacheRoot.getPath());
                }

                return cacheRoot;
            }
            logger.error("error creating " + tmpDir.getPath());
        }

        logger.error("failed to create temporary cache directory");
        throw new IOException();
    }

    public static synchronized File create(String domain) throws IOException {

        File tmpDir = new File(getCacheRoot().getPath() + File.separator + domain);
        if (!tmpDir.exists() && !tmpDir.mkdirs()) {
            logger.error("Error creating " + tmpDir.getPath());
            throw new IOException();
        }

        return tmpDir;
    }

    public static File create(String domain, String name) throws IOException {
        return create(domain + File.separator + name);
    }

    private static void removeImpl(File dir) {
        String[] files = BDFileSystem.nativeList(dir);
        if (files != null) {
        for (int i = 0; i < files.length; i++) {
            File file = new File(dir, files[i]);
            if (file.isDirectory()) {
                removeImpl(file);
            } else {
                if (!file.delete()) {
                    logger.info("Error removing " + file.getPath());
                }
            }
        }
        }

        if (!dir.delete()) {
            logger.error("Error removing " + dir.getPath());
        }
    }

    public static synchronized void remove(File dir) {
        String path = dir.getPath();
        if (path.indexOf(cacheRoot.getPath()) != 0) {
            logger.error("Error removing " + dir.getPath() + ": not inside cache !");
            return;
        }
        if (path.indexOf("..") >= 0) {
            logger.error("Error removing " + dir.getPath() + ": not canonical path !");
            return;
        }

        removeImpl(dir);
    }

    public static synchronized void remove() {

        if (lockFile != null) {
            lockFile.release();
            lockFile = null;
        }

        if (cacheRoot != null) {
            remove(cacheRoot);
            cacheRoot = null;
        }
    }

    private static File cacheRoot = null;
    private static LockFile lockFile = null;

    private static final String baseDir = System.getProperty("java.io.tmpdir") + File.separator + "libbluray-bdj-cache" + File.separator;
    private static final Logger logger = Logger.getLogger(CacheDir.class.getName());
}
