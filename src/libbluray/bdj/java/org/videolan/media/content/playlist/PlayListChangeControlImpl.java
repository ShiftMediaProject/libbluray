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

import javax.media.ClockStartedError;
import javax.tv.locator.InvalidLocatorException;

import org.bluray.media.InvalidPlayListException;
import org.bluray.media.PlayListChangeControl;
import org.bluray.net.BDLocator;
import org.bluray.ti.PlayList;
import org.videolan.Libbluray;

public class PlayListChangeControlImpl implements PlayListChangeControl {
    protected PlayListChangeControlImpl(Handler player)
    {
        this.player = player;
    }
    
    public Component getControlComponent()
    {
        return null;
    }

    public void selectPlayList(PlayList pl) throws InvalidPlayListException,
            ClockStartedError
    {
        if (pl == null)
            throw new NullPointerException();
        
        try {
            selectPlayList((BDLocator)pl.getLocator());
        } catch (InvalidLocatorException e) {
            throw new InvalidPlayListException();
        }
    }

    public void selectPlayList(BDLocator locator)
            throws InvalidPlayListException, InvalidLocatorException,
            ClockStartedError
    {
        if (locator == null)
            throw new NullPointerException();
        
        if (player.getState() == Handler.Started)
            throw new ClockStartedError();
        
        if (!Libbluray.selectPlaylist(locator.getPlayListId()))
            throw new InvalidPlayListException();
        
        player.ti = Libbluray.getPlaylistInfo(locator.getPlayListId());
    }

    public BDLocator getCurrentPlayList()
    {
        try {
            return new BDLocator(null, player.ti.getIndex(), player.ti.getPlaylist());
        } catch (org.davic.net.InvalidLocatorException e) {
            return null;
        }
    }

    private Handler player;
}
