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

class CacheDir {

    private static synchronized File getCacheRoot() throws IOException {

        if (cacheRoot != null) {
            return cacheRoot;
        }

        String base = System.getProperty("java.io.tmpdir") + File.separator +
            "libbluray-bdj-cache" + File.separator;

        for (int i = 0; i < 100; i++) {
            File tmpDir = new File(base + System.nanoTime());
            tmpDir = new File(tmpDir.getCanonicalPath());
            if (tmpDir.mkdirs()) {
                cacheRoot = tmpDir;
                logger.info("Created cache in " + tmpDir.getPath());
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
        File[] files = dir.listFiles();
        if (files != null) {
        for (int i = 0; i < files.length; i++) {
            File file = files[i];
            if (file.isDirectory()) {
                remove(file);
            } else {
                if (!file.delete()) {
                    logger.error("Error removing " + file.getPath());
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
        if (cacheRoot != null) {
            remove(cacheRoot);
        }
    }

    private static File cacheRoot = null;
    private static final Logger logger = Logger.getLogger(MountManager.class.getName());
}
