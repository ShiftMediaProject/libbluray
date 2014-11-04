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
import java.io.RandomAccessFile;

class LockFile {

    private LockFile(RandomAccessFile lockFile) {
        this.lockFile = lockFile;
    }

    public static LockFile create(String path) {

        try {
            RandomAccessFile os = new RandomAccessFile(path, "rw");
            if (os.getChannel().tryLock() != null) {
                try {
                    /* Test if locking works: second tryLock() should fail */
                    if (os.getChannel().tryLock() != null) {
                        try {
                            os.close();
                        } catch (Exception e) {
                        }
                        logger.error("File locking is unreliable !");
                        return null;
                    }
                } catch (Exception e) {
                }
                return new LockFile(os);
            } else {
                logger.info("Failed locking " + path);
            }
        } catch (Exception e) {
            logger.error("Failed creating lock file: " + e);
        }
        return null;
    }

    public void release() {
        try {
            if (lockFile != null) {
                lockFile.close();
            }
        } catch (Exception e) {
        }
    }

    private RandomAccessFile lockFile;

    private static final Logger logger = Logger.getLogger(LockFile.class.getName());
}
