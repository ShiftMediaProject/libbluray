package org.videolan.bdjo;

import java.util.logging.Level;
import java.util.logging.Logger;

public enum AppVisibility {
    NOT_VISIBLE(0),
    VISIBLE_TO_APPS(1),
    VISIBLE_TO_APPS_AND_USERS(3);
    
    AppVisibility(int id)
    {
        this.id = id;
    }
    
    public int getId()
    {
        return id;
    }
    
    
    public static AppVisibility fromId(int id)
    {
        for (AppVisibility res : values())
        {
            if (res.getId() == id)
                return res;
        }
        
        logger.log(Level.WARNING, "Invalid app visibility id: " + id);
        return null;
    }
    
    private int id;
    
    private static final Logger logger = Logger.getLogger(AppVisibility.class.getName());
}
