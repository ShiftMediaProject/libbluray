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
