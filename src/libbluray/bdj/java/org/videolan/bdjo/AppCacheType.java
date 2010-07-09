package org.videolan.bdjo;

import java.util.logging.Level;
import java.util.logging.Logger;

public enum AppCacheType {
    JAR_FILE(1),
    DIRECTORY(2);
    
    AppCacheType(int id)
    {
        this.id = id;
    }
    
    public int getId()
    {
        return id;
    }
    
    
    public static AppCacheType fromId(int id)
    {
        for (AppCacheType res : values())
        {
            if (res.getId() == id)
                return res;
        }
        
        logger.log(Level.WARNING, "Invalid control code id: " + id);
        return null;
    }
    
    private int id;
    
    private static final Logger logger = Logger.getLogger(AppCacheType.class.getName());
}
