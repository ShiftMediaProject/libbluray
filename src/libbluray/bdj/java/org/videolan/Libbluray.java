package org.videolan;

public class Libbluray {
    public static int getCurrentTitleNum() {
        return getCurrentTitleNumN(BasicXletContext.instance.getNativePointer());
    }
    
    public static TitleInfo getTitleInfo(int titleNum)
    {
        return new TitleInfo(titleNum);
    }
    
    public static void writeGPR(int num, int value)
    {
        writeGPRN(BasicXletContext.instance.getNativePointer(), num, value);
    }
    
    public static int readGPR(int num)
    {
        return readGPRN(BasicXletContext.instance.getNativePointer(), num);
    }
    
    public static int readPSR(int num)
    {
        return readPSRN(BasicXletContext.instance.getNativePointer(), num);
    }
    
    private static native int getCurrentTitleNumN(long pBDJ);
    private static native void writeGPRN(long pBDJ, int num, int value);
    private static native int readGPRN(long pBDJ, int num);
    private static native int readPSRN(long pBDJ, int num);
}
