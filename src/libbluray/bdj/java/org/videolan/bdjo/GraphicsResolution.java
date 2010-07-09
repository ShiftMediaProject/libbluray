package org.videolan.bdjo;

import java.util.logging.Level;
import java.util.logging.Logger;

public enum GraphicsResolution
{
    HD_1920_1080(1, 1920, 1080),
    HD_1280_720(2, 1280, 720),
    SD(3, 720, 480), // FIXME: probably should take into consideration PAL
    SD_50HZ_720_576(4, 720, 576),
    SD_60HZ_720_480(5, 720, 480),
    QHD_960_540(6, 960, 540);
    
    GraphicsResolution(int id, int width, int height)
    {
        this.id = id;
        this.width = width;
        this.height = height;
    }
    
    public int getId()
    {
        return id;
    }
    
    public int getWidth()
    {
        return width;
    }
    
    public int getHeight()
    {
        return height;
    }
    
    public static GraphicsResolution fromId(int id)
    {
        for (GraphicsResolution res : values())
        {
            if (res.getId() == id)
                return res;
        }
        
        logger.log(Level.WARNING, "Invalid graphics resolution id: " + id);
        return null;
    }
    
    private int id;
    private int width;
    private int height;
    
    private static final Logger logger = Logger.getLogger(GraphicsResolution.class.getName());
}
