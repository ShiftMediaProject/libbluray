/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

package org.videolan.media.content.playlist;

import java.awt.Component;
import java.io.IOException;
import java.util.LinkedList;

import javax.media.Clock;
import javax.media.ClockStartedError;
import javax.media.ClockStoppedException;
import javax.media.Control;
import javax.media.Controller;
import javax.media.ControllerClosedEvent;
import javax.media.ControllerEvent;
import javax.media.ControllerListener;
import javax.media.DeallocateEvent;
import javax.media.GainControl;
import javax.media.IncompatibleSourceException;
import javax.media.IncompatibleTimeBaseException;
import javax.media.Manager;
import javax.media.NotPrefetchedError;
import javax.media.Player;
import javax.media.PrefetchCompleteEvent;
import javax.media.RealizeCompleteEvent;
import javax.media.StartEvent;
import javax.media.StopByRequestEvent;
import javax.media.Time;
import javax.media.TimeBase;
import javax.media.TransitionEvent;
import javax.media.protocol.DataSource;

import org.bluray.media.OverallGainControl;
import org.videolan.Libbluray;
import org.videolan.TitleInfo;

public class Handler implements Player {
    public Handler()
    {
        controls = new Control[20];
        
        controls[0] = new AngleControlImpl(this);
        controls[1] = new AsynchronousPiPControlImpl();
        controls[2] = new AudioMetadataControlImpl();
        controls[3] = new AWTVideoSizeControlImpl(this);
        controls[4] = new BackgroundVideoPresentationControlImpl();
        controls[5] = new DVBMediaSelectControlImpl();
        controls[6] = new MediaTimeEventControlImpl();
        controls[7] = new MediaTimePositionControlImpl();
        controls[8] = new OverallGainControlImpl();
        controls[9] = new PanningControlImpl();
        controls[10] = new PiPControlImpl();
        controls[11] = new PlaybackControlImpl(this);
        controls[12] = new PlayListChangeControlImpl(this);
        controls[13] = new PrimaryAudioControlImpl(this);
        controls[14] = new PrimaryGainControlImpl();
        controls[15] = new SecondaryAudioControlImpl(this);
        controls[16] = new SecondaryGainControlImpl();
        controls[17] = new SubtitlingControlImpl(this);
        controls[18] = new UOMaskTableControlImpl();
        controls[19] = new VideoFormatControlImpl();
    }
    
    public void setSource(DataSource source) throws IOException,
            IncompatibleSourceException
    {
        if (source instanceof org.videolan.media.protocol.bd.DataSource &&
                source.getContentType().equals("playlist")) 
        {
            org.videolan.media.protocol.bd.DataSource playlistSrc = 
                (org.videolan.media.protocol.bd.DataSource)source;
            
            ti = Libbluray.getPlaylistInfo(playlistSrc.getPlaylist());
        } else {
            throw new IncompatibleSourceException();
        }
    }

    public int getState()
    {
        return state;
    }

    public int getTargetState()
    {
        return state;
    }

    public void realize()
    {
        sendControllerEvent(new TransitionEvent(this, Unrealized, Realizing, Realized));
        sendControllerEvent(new RealizeCompleteEvent(this, Realizing, Realized, Realized));
    }

    public void prefetch()
    {
        if (state == Unrealized)
            realize();
        
        sendControllerEvent(new TransitionEvent(this, Realized, Prefetching, Prefetched));
        sendControllerEvent(new PrefetchCompleteEvent(this, Prefetching, Prefetched, Prefetched));
    }

    public void deallocate()
    {
        if (state == Started)
            throw new ClockStartedError();
        
        if (state != Unrealized || state != Realized) {
            sendControllerEvent(new DeallocateEvent(this, state, Realized, Realized, currentTime));
        }
    }

    public void close()
    {
        sendControllerEvent(new ControllerClosedEvent(this, "closed"));
    }

    public Time getStartLatency()
    {
        return new Time(Time.ONE_SECOND); // this is arbitrary since the start latency can't be determined
    }

    public Control[] getControls()
    {
        return controls;
    }

    public Control getControl(String forName)
    {
        try {
            Class<?> cls = Class.forName(forName);
            
            for (Control c : controls) {
                if (cls.isInstance(c))
                    return c;
            }
            
            return null;
        } catch (ClassNotFoundException e) {
            return null;
        }
        
    }

    public void addControllerListener(ControllerListener listener)
    {
        listeners.add(listener);
    }
    
    public void removeControllerListener(ControllerListener listener)
    {
        listeners.remove(listener);
    }

    public void setTimeBase(TimeBase master)
            throws IncompatibleTimeBaseException
    {
        // this probably isn't useful due to the actual clock being outside of bdj
        // so just ignore this
    }

    public void syncStart(Time at)
    {
        // TODO signal player to actually start playing
        if (state == Started)
            throw new ClockStartedError();
        
        if (state != Prefetched)
            throw new NotPrefetchedError("syncStart");
        
        currentTime = at;
        sendControllerEvent(new StartEvent(this, Prefetched, Started, Started, at, at));
    }

    public void stop()
    {
        // TODO signal player to actually stop playing
        if (state == Started) {
            sendControllerEvent(new StopByRequestEvent(this, Started, Prefetched, Prefetched, currentTime));
        }
    }

    public void setStopTime(Time stopTime)
    {
        // TODO: actually stopping when stop time is hit needs to be implemented
        this.stopTime = stopTime; 
    }

    public Time getStopTime()
    {
        return stopTime;
    }

    public void setMediaTime(Time now)
    {
        // TODO: again this doesn't really do much right now
        if (state != Started)
            currentTime = now;
        else
            throw new ClockStartedError();
    }

    public Time getMediaTime()
    {
        return currentTime;
    }

    public long getMediaNanoseconds()
    {
        return currentTime.getNanoseconds();
    }

    public Time getSyncTime()
    {
        return getMediaTime();
    }

    public TimeBase getTimeBase()
    {
        return Manager.getSystemTimeBase();
    }

    public Time mapToTimeBase(Time t) throws ClockStoppedException
    {
        if (state != Started)
            throw new ClockStoppedException();
        return getMediaTime();
    }

    public float getRate()
    {
        return rate;
    }

    public float setRate(float factor)
    {
        if (state == Started)
            throw new ClockStartedError();
        
        return 1.0f; // TODO: maybe allow changing rate?
    }

    public Time getDuration()
    {
        long duration = ti.getDuration() ;
        return new Time(duration * TO_SECONDS);
    }

    public Component getVisualComponent()
    {
        return null;
    }

    public GainControl getGainControl()
    {
        for (Control c : controls) {
            if (c instanceof OverallGainControl)
                return (GainControl)c;
        }
        
        return null;
    }

    public Component getControlPanelComponent()
    {
        return null;
    }

    public void start()
    {
        if (state != Prefetched)
            throw new NotPrefetchedError("start");
        
        sendControllerEvent(new StartEvent(this, Prefetched, Started, Started, currentTime, currentTime));
    }

    public void addController(Controller newController)
            throws IncompatibleTimeBaseException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void removeController(Controller oldController)
    {
        throw new Error("Not implemented"); // TODO implement
    }
    
    private void sendControllerEvent(ControllerEvent event) 
    {
        if (event instanceof TransitionEvent)
            state = ((TransitionEvent)event).getCurrentState();
        
        for (ControllerListener listener : listeners) {
            listener.controllerUpdate(event);
        }
    }
    
    protected static final double TO_SECONDS = 1 / 90000.0;
    protected static final double FROM_SECONDS = 90000.0;
    protected TitleInfo ti;

    private Control[] controls;
    private Time stopTime = Clock.RESET;
    private Time currentTime = new Time(0);
    private float rate = 1.0f;
    
    private LinkedList<ControllerListener> listeners = new LinkedList<ControllerListener>();
    
    private int state = Unrealized;
}
