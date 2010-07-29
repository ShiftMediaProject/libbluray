package org.videolan;

public class TIChapter {
    public TIChapter(int index, long start, long duration, long offset)
    {
        this.index = index;
        this.start = start;
        this.duration = duration;
        this.offset = offset;
    }
    
    public int getIndex()
    {
        return index;
    }
    
    public long getStart()
    {
        return start;
    }
    
    public long getDuration()
    {
        return duration;
    }
    
    public long getOffset()
    {
        return offset;
    }

    private int index;
    private long start;
    private long duration;
    private long offset;
}
