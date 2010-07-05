package org.videolan;

import org.bluray.ti.PlayList;

public class TitleInfo {
    protected TitleInfo(int titleNum)
    {
        
    }
    
    public PlayList[] getPlayLists()
    {
        throw new Error("Not implemented");
    }
    
    public boolean hasAutoPlayList()
    {
        throw new Error("Not implemented");
    }
}
