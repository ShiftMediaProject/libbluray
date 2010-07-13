package org.videolan;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.logging.Logger;

/**
 * This class handle mounting jar files so that their contents can be accessed.
 * 
 * @author William Hahne
 *
 */
public class MountManager {
    public static String mount(int jarId) throws MountException
    {
        String jarStr = jarIdToString(jarId);
     
        logger.info("Mounting JAR: " + jarStr);
        
        if (jarStr == null)
            throw new IllegalArgumentException();
        
        String path = BDJLoader.getBaseDir() + "/BDMV/JAR/" + jarStr + ".jar";
        
        JarFile jar = null;
        File tmpDir = null;
        try {
            jar = new JarFile(path);
            tmpDir = File.createTempFile("bdj-", "");
        } catch (IOException e) {
            e.printStackTrace();
            throw new MountException();
        }
        
        // create temporary directory
        tmpDir.delete();
        tmpDir.mkdir();
        
        try {
            Enumeration<JarEntry> entries = jar.entries();
            while (entries.hasMoreElements()) {
                JarEntry entry = entries.nextElement();
                File out = new File(tmpDir + File.separator + entry.getName());
                
                if (entry.isDirectory()) {
                    out.mkdir();
                } else {
                    InputStream inStream = jar.getInputStream(entry);
                    OutputStream outStream = new FileOutputStream(out);
                    
                    while (inStream.available() > 0) {
                        outStream.write(inStream.read());
                    }
                    
                    inStream.close();
                    outStream.close();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
            recursiveDelete(tmpDir);
            throw new MountException();
        }
        
        mountPoints.put(jarId, tmpDir);
        return tmpDir.getAbsolutePath();
    }
    
    public static void unmount(int jarId)
    {
        logger.info("Unmounting JAR: " + jarId);
        
        File mountPoint = mountPoints.get(jarId);
        if (mountPoint != null) {
            recursiveDelete(mountPoint);
            mountPoints.remove(jarId);
        }
    }
    
    public static void unmountAll()
    {
        for (int key : mountPoints.keySet()) {
            unmount(key);
        }
    }
    
    public static String getMount(int jarId)
    {
        if (mountPoints.containsKey(jarId)) {
            return mountPoints.get(jarId).getAbsolutePath();
        } else {
            return null;
        }
    }
    
    private static String jarIdToString(int jarId)
    {
        if (jarId < 0 || jarId > 99999)
            return null;
        return BDJUtil.makeFiveDigitStr(jarId);
    }
    
    private static void recursiveDelete(File dir) {  
        for (File file : dir.listFiles()) {
            if (file.isDirectory()) {
                recursiveDelete(file);
            } else {
                file.delete();
            }
        }
        
        dir.delete();
    }
    
    private static Map<Integer, File> mountPoints = Collections.synchronizedMap(new HashMap<Integer, File>());
    private static final Logger logger = Logger.getLogger(MountManager.class.getName());
}
