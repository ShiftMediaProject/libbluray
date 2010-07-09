package org.videolan.bdjo;

import java.util.logging.Level;
import java.util.logging.Logger;

public enum ControlCode {
    AUTOSTART(1),
    PRESENT(2);
    
    ControlCode(int id)
    {
        this.id = id;
    }
    
    public int getId()
    {
        return id;
    }
    
    
    public static ControlCode fromId(int id)
    {
        for (ControlCode res : values())
        {
            if (res.getId() == id)
                return res;
        }
        
        logger.log(Level.WARNING, "Invalid control code id: " + id);
        return null;
    }
    
    private int id;
    
    private static final Logger logger = Logger.getLogger(ControlCode.class.getName());
}
