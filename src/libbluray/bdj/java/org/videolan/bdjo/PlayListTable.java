package org.videolan.bdjo;

import java.util.Arrays;

public class PlayListTable {
    public PlayListTable(boolean accessToAll, boolean autostartFirst, String[] playLists)
    {
        this.accessToAll = accessToAll;
        this.playLists = playLists;
    }
    
    public boolean isAccessToAll()
    {
        return accessToAll;
    }
    
    public String[] getPlayLists()
    {
        return playLists;
    }

    public String toString()
    {
        return "PlayListTable [accessToAll=" + accessToAll + ", playLists="
                + Arrays.toString(playLists) + "]";
    }

    private boolean accessToAll;
    private String[] playLists;
}
