package org.videolan;

public class FontIndexData {
    public FontIndexData()
    {

    }
    
    public String getName()
    {
        return name;
    }
    
    public String getFormat()
    {
        return format;
    }
    
    public String getFileName()
    {
        return filename;
    }
    
    public int getMinSize()
    {
        return minSize;
    }
    
    public int getMaxSize()
    {
        return maxSize;
    }
    
    public int getStyle()
    {
        return style;
    }

    public String toString()
    {
        return "FontIndexData [filename=" + filename + ", format=" + format
                + ", maxSize=" + maxSize + ", minSize=" + minSize + ", name="
                + name + ", style=" + style + "]";
    }

    protected String name;
    protected String format;
    protected String filename;
    protected int minSize;
    protected int maxSize;
    protected int style;
}
