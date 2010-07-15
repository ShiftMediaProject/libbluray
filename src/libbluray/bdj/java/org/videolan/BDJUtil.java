package org.videolan;

import java.text.DecimalFormat;

public class BDJUtil {
    /**
     * Make a five digit zero padded string based on an integer
     * Ex. integer 1 -> string "00001"
     * 
     * @param id
     * @return
     */
    public static String makeFiveDigitStr(int id) 
    {
        DecimalFormat fmt = new DecimalFormat();
        fmt.setMaximumIntegerDigits(5);
        fmt.setMinimumIntegerDigits(5);
        fmt.setGroupingUsed(false);
        
        return fmt.format(id);
    }
    
    /**
     * Make a path based on the disc root to an absolute path based on the filesystem of the computer
     * Ex. /BDMV/JAR/00000.jar -> /bluray/disc/mount/point/BDMV/JAR/00000.jar
     * @param path
     * @return
     */
    public static String discRootToFilesystem(String path)
    {
        return BDJLoader.getBaseDir() + path;
    }
}
