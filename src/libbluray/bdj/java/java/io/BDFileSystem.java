/*
 * This file is part of libbluray
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

/*
 * Wrapper for java.io.FileSystem class.
 *
 * - replace getBooleanAttributes() for relative paths.
 *   Pretend files exist, if those are in xlet home directory (inside .jar).
 *   No other relative paths are allowed.
 */

package java.io;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

import java.net.URL;

import org.videolan.BDJLoader;
import org.videolan.BDJXletContext;
import org.videolan.Logger;

public abstract class BDFileSystem extends FileSystem {

    private static final Logger logger = Logger.getLogger(BDFileSystem.class.getName());

    protected final FileSystem fs;

    public static void init(Class c) {
        Field filesystem;
        try {
            filesystem = c.getDeclaredField("fs");
            filesystem.setAccessible(true);

            /* Java 8: remove "final" modifier from the field */
            Field modifiersField = Field.class.getDeclaredField("modifiers");
            modifiersField.setAccessible(true);
            modifiersField.setInt(filesystem, filesystem.getModifiers() & ~Modifier.FINAL);

            FileSystem fs = (FileSystem)filesystem.get(null);
            if (fs instanceof BDFileSystemImpl) {
                //System.err.print("FileSystem already wrapped");
            } else {
                filesystem.set(null, new BDFileSystemImpl(fs));
            }
        } catch (Throwable t) {
            System.err.print("Hooking FileSystem class failed: " + t);
        }
    }

    public BDFileSystem(FileSystem fs) {
        this.fs = fs;
    }

    public char getSeparator() {
        return fs.getSeparator();
    }

    public char getPathSeparator() {
        return fs.getPathSeparator();
    }

    public String normalize(String pathname) {
        return fs.normalize(pathname);
    }

    public int prefixLength(String pathname) {
        return fs.prefixLength(pathname);
    }

    public String resolve(String parent, String child) {
        String resolvedPath = fs.resolve(parent, child);
        String cachePath = BDJLoader.getCachedFile(resolvedPath);
        if (cachePath != resolvedPath) {
            logger.info("resolve(p,c): using cached " + cachePath + " (" + resolvedPath + ")");
        }
        return cachePath;
    }

    public String getDefaultParent() {
        return fs.getDefaultParent();
    }

    public String fromURIPath(String path) {
        return fs.fromURIPath(path);
    }

    public boolean isAbsolute(File f) {
        return fs.isAbsolute(f);
    }

    public String resolve(File f) {
        if (!f.isAbsolute()) {
            System.err.println("***** resolve " + f + " -> " + fs.resolve(f));
        }

        String resolvedPath = fs.resolve(f);
        String cachePath = BDJLoader.getCachedFile(resolvedPath);
        if (cachePath != resolvedPath) {
            logger.info("resolve(f): using cached " + cachePath + " (" + resolvedPath + ")");
        }
        return cachePath;
    }

    public String canonicalize(String path) throws IOException {
        String canonPath = fs.canonicalize(path);
        String cachePath = BDJLoader.getCachedFile(canonPath);
        if (cachePath != canonPath) {
            logger.info("canonicalize(): Using cached " + cachePath + " for " + canonPath + "(" + path + ")");
        }
        return cachePath;
    }

    public int getBooleanAttributes(File f) {
        if (f.isAbsolute()) {
            return fs.getBooleanAttributes(f);
        }

        /* try to locate file in Xlet home directory (inside JAR file) */
        URL url = BDJXletContext.getCurrentResource(f.getPath());
        if (url == null) {
            return 0;
        }

        logger.info("Relative path " + f.getPath() + " translated to " + url);

        return FileSystem.BA_EXISTS; //|BA_REGULAR
    }

    /*
      ME: public abstract boolean checkAccess(File f, boolean write);
      SE: public abstract boolean checkAccess(File f, int access);
    */

    public long getLastModifiedTime(File f) {
        return fs.getLastModifiedTime(f);
    }

    public long getLength(File f) {
        return fs.getLength(f);
    }

    /*
      SE only
      public abstract boolean setPermission(File f, int access, boolean enable, boolean owneronly);
    */

    /* this version exists in some java6 versions.
     * Use reflection to make sure build succees and right method is called.
     */
    public boolean createFileExclusively(String path, boolean restrictive) throws IOException {
        return createFileExclusivelyImpl(path, restrictive);
    }
    /* this version exists in most java versions (1.4, 1.7, some 1.6 versions) */
    public boolean createFileExclusively(String path) throws IOException {
        return createFileExclusivelyImpl(path, false);
    }

    private boolean createFileExclusivelyImpl(String path, boolean restrictive) throws IOException {
        Method m;
        Object[] args;

        /* resolve method and set up arguments */
        try {
            try {
                m = fs.getClass().getDeclaredMethod("createFileExclusively", new Class[] { String.class });
                args = new Object[] {(Object)path};
            } catch (NoSuchMethodException e) {
                m  = fs.getClass().getDeclaredMethod("createFileExclusively", new Class[] { String.class, boolean.class });
                args = new Object[] {(Object)path, (Object)new Boolean(restrictive)};
            }
        } catch (NoSuchMethodException e) {
            logger.error("no matching FileSystem.createFileExclusively found !");
            throw new IOException();
        }

        /* call */
        try {
            Boolean result = (Boolean)m.invoke(fs, args);
            return result.booleanValue();
        } catch (IllegalAccessException e) {
            logger.error("" + e);
            throw new IOException();
        } catch (InvocationTargetException e) {
            Throwable t = e.getTargetException();
            if (t instanceof IOException) {
                throw (IOException)t;
            }
            logger.error("" + t);
            throw new IOException();
        }
    }

    /*
      ME only
      public abstract boolean deleteOnExit(File f);
    */

    /*
      SE only
      public abstract long getSpace(File f, int t);
    */

    public boolean delete(File f) {
        return fs.delete(f);
    }

    public String[] list(File f) {
        return fs.list(f);
    }

    public boolean createDirectory(File f) {
        return fs.createDirectory(f);
    }

    public boolean rename(File f1, File f2) {
        return fs.rename(f1, f2);
    }

    public boolean setLastModifiedTime(File f, long time) {
        return fs.setLastModifiedTime(f, time);
    }

    public boolean setReadOnly(File f) {
        return fs.setReadOnly(f);
    }

    public File[] listRoots() {
        return fs.listRoots();
    }

    public int compare(File f1, File f2) {
        return fs.compare(f1, f2);
    }

    public int hashCode(File f) {
        return fs.hashCode(f);
    }
}
