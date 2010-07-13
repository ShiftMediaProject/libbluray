package org.videolan;

import java.text.DecimalFormat;

public class BDJUtil {
    public static String makeFiveDigitStr(int id) 
    {
        DecimalFormat fmt = new DecimalFormat();
        fmt.setMaximumIntegerDigits(5);
        fmt.setMinimumIntegerDigits(5);
        fmt.setGroupingUsed(false);
        
        return fmt.format(id);
    }
}
