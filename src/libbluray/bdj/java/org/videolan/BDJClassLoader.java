package org.videolan;

import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;

import org.videolan.bdjo.AppCache;
import org.videolan.bdjo.AppCacheType;

public class BDJClassLoader extends URLClassLoader {
    public BDJClassLoader(String baseDir)
    {
        super(new URL[0]);
        
        this.baseDir = baseDir;
    }
    
    public void setBaseDir(String baseDir)
    {
        this.baseDir = baseDir;
    }
    
    public String getBaseDir()
    {
        return baseDir;
    }

    public void addAppCache(AppCache cache)
    {
        String url = "file:";
        url += baseDir + "/BDMV/JAR/";
        url += cache.getRefToName();
        if (cache.getType().equals(AppCacheType.JAR_FILE))
            url += ".jar";
        else if (cache.getType().equals(AppCacheType.DIRECTORY))
            url += "/";
        
        System.out.println(url);

        try {
            addURL(new URL(url));
        } catch (MalformedURLException e) {
            e.printStackTrace();
        }
    }

    private String baseDir = "";
}
