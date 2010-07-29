package org.videolan.media.content.playlist;

import java.awt.Component;
import java.io.IOException;

import javax.media.ClockStoppedException;
import javax.media.Control;
import javax.media.Controller;
import javax.media.ControllerListener;
import javax.media.GainControl;
import javax.media.IncompatibleSourceException;
import javax.media.IncompatibleTimeBaseException;
import javax.media.Player;
import javax.media.Time;
import javax.media.TimeBase;
import javax.media.protocol.DataSource;

import org.videolan.Libbluray;
import org.videolan.TitleInfo;

public class Handler implements Player {
    public void setSource(DataSource source) throws IOException,
            IncompatibleSourceException
    {
        if (source instanceof org.videolan.media.protocol.bd.DataSource &&
                source.getContentType().equals("playlist")) 
        {
            org.videolan.media.protocol.bd.DataSource playlistSrc = 
                (org.videolan.media.protocol.bd.DataSource)source;
            
            TitleInfo ti = Libbluray.getPlaylistInfo(playlistSrc.getPlaylist());
            
        } else {
            throw new IncompatibleSourceException();
        }
    }

    @Override
    public int getState()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public int getTargetState()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public void realize()
    {
        // TODO Auto-generated method stub
        
    }

    public void prefetch()
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void deallocate()
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void close()
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public Time getStartLatency()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Control[] getControls()
    {
        // TODO Auto-generated method stub
        return new Control[0];
    }

    @Override
    public Control getControl(String forName)
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void addControllerListener(ControllerListener listener)
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void removeControllerListener(ControllerListener listener)
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void setTimeBase(TimeBase master)
            throws IncompatibleTimeBaseException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void syncStart(Time at)
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void stop()
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void setStopTime(Time stopTime)
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public Time getStopTime()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setMediaTime(Time now)
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public Time getMediaTime()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public long getMediaNanoseconds()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public Time getSyncTime()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public TimeBase getTimeBase()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Time mapToTimeBase(Time t) throws ClockStoppedException
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public float getRate()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public float setRate(float factor)
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public Time getDuration()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Component getVisualComponent()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public GainControl getGainControl()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Component getControlPanelComponent()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void start()
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void addController(Controller newController)
            throws IncompatibleTimeBaseException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void removeController(Controller oldController)
    {
        // TODO Auto-generated method stub
        
    }

}
