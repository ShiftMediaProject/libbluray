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
import java.net.URL;

import org.videolan.BDJXletContext;

public abstract class BDFileSystem extends FileSystem {

    protected final FileSystem fs;

    public static void init(Class c) {
        Field filesystem;
        try {
            filesystem = c.getDeclaredField("fs");
            filesystem.setAccessible(true);

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
        return fs.resolve(parent, child);
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
            System.err.println("***** resolve " + f);
        }
        return fs.resolve(f);
    }

    public String canonicalize(String path) throws IOException {
        return fs.canonicalize(path);
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

        org.videolan.Logger.getLogger("BDFileSystem").info("Relative path " + f.getPath() + " translated to " + url);

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

    public boolean createFileExclusively(String path) throws IOException {
        return fs.createFileExclusively(path);
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

    /*
      SE only
      public abstract long getSpace(File f, int t);
    */

    public int compare(File f1, File f2) {
        return fs.compare(f1, f2);
    }

    public int hashCode(File f) {
        return fs.hashCode(f);
    }
}
