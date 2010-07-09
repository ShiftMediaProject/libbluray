package org.videolan.bdjo;

import java.util.logging.Level;
import java.util.logging.Logger;

public enum AppBinding {
    TITLE_UNBOUND_DISC_UNBOUND(0),
    TITLE_UNBOUND_DISC_BOUND(1),
    TITLE_BOUND_DISC_BOUND(3);
    
    AppBinding(int id)
    {
        this.id = id;
    }
    
    public int getId()
    {
        return id;
    }
    
    
    public static AppBinding fromId(int id)
    {
        for (AppBinding res : values())
        {
            if (res.getId() == id)
                return res;
        }
        
        logger.log(Level.WARNING, "Invalid app binding id: " + id);
        return null;
    }
    
    private int id;
    
    private static final Logger logger = Logger.getLogger(AppBinding.class.getName());
}
