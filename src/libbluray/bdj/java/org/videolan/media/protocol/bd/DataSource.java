package org.videolan.media.protocol.bd;

import java.io.IOException;
import javax.media.Time;
import org.bluray.net.BDLocator;
import org.davic.net.InvalidLocatorException;

public class DataSource extends javax.media.protocol.DataSource {
    public Object[] getControls()
    {
        return new Object[0];
    }

    public Object getControl(String controlType)
    {
        return null;
    }

    public Time getDuration()
    {
        throw new Error("Not implemented");
    }

    public String getContentType()
    {
        return contentType;
    }

    // TODO: support all content types
    public void connect() throws IOException
    {
        try {
            BDLocator locator = new BDLocator(getLocator().toExternalForm());
            
            if (locator.isPlayListItem()) {
                contentType = "playlist";
                playlist = locator.getPlayListId();
            }
        } catch (InvalidLocatorException e) {
            // ignore
        }
    }

    public void disconnect()
    {
        contentType = "unknown";
    }

    public void start() throws IOException
    {
        if (contentType.equals("unknown"))
            throw new IOException("Unknown content type.");
    }

    public void stop() throws IOException
    {
        
    }
    
    public int getPlaylist()
    {
        return playlist;
    }

    private String contentType = "unknown";
    private int playlist = -1;
}
