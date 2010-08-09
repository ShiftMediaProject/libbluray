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
import java.util.LinkedList;

import org.bluray.media.AngleChangeEvent;
import org.bluray.media.AngleChangeListener;
import org.bluray.media.PlaybackControl;
import org.bluray.media.PlaybackListener;
import org.bluray.media.PlaybackMarkEvent;
import org.bluray.media.PlaybackPlayItemEvent;
import org.videolan.Libbluray;
import org.videolan.TIClip;

public class PlaybackControlImpl implements PlaybackControl {
    protected PlaybackControlImpl(Handler player)
    {
        this.player = player;
    }
    
    public Component getControlComponent()
    {
        return null;
    }

    public void addPlaybackControlListener(PlaybackListener listener)
    {
        listeners.add(listener);
    }

    public void removePlaybackControlListener(PlaybackListener listener)
    {
        listeners.remove(listener);
    }

    public void skipToMark(int mark) throws IllegalArgumentException
    {
        Libbluray.seekMark(mark);
        sendEvent(new PlaybackMarkEvent(this, mark));
    }

    public boolean skipToNextMark(int type) throws IllegalArgumentException
    {
        throw new Error("Not implemented");
    }

    public boolean skipToPreviousMark(int type) throws IllegalArgumentException
    {
        throw new Error("Not implemented");
    }

    public void skipToPlayItem(int item) throws IllegalArgumentException
    {
        throw new Error("Not implemented");
    }
    
    private void sendEvent(PlaybackMarkEvent event)
    {
        for (PlaybackListener listener : listeners)
            listener.markReached(event);
    }
    
    // DON'T REMOVE: this will be used when skipToPlayItem is implemented
    private void sendEvent(PlaybackPlayItemEvent event)
    {
        for (PlaybackListener listener : listeners)
            listener.playItemReached(event);
    }
    
    private LinkedList<PlaybackListener> listeners = new LinkedList<PlaybackListener>();
    private Handler player;
}
