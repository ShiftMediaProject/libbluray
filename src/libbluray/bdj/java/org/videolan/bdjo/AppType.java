package org.videolan.bdjo;

import java.util.logging.Level;
import java.util.logging.Logger;

public enum AppType
{
    DVB_J(1),
    DVB_HTML(2);
    
    AppType(int id)
    {
        this.id = id;
    }
    
    public int getId()
    {
        return id;
    }
    
    
    public static AppType fromId(int id)
    {
        for (AppType res : values())
        {
            if (res.getId() == id)
                return res;
        }
        
        logger.log(Level.WARNING, "Invalid app type id: " + id);
        return null;
    }
    
    private int id;
    
    private static final Logger logger = Logger.getLogger(AppType.class.getName());
}
