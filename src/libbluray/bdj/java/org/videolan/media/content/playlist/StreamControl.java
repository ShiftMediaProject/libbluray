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

import org.bluray.system.RegisterAccess;
import org.davic.media.LanguageNotAvailableException;
import org.davic.media.NotAuthorizedException;
import org.videolan.StreamInfo;
import org.videolan.TIClip;

public abstract class StreamControl {
    protected StreamControl(Handler player)
    {
        this.player = player;
    }
    
    protected int[] listAvailableStreamNumbers(StreamInfo[] streams)
    {
        int[] ret = new int[streams.length];
        for (int i = 0; i < streams.length; i++)
            ret[i] = i + 1;
        
        return ret;
    }
    
    protected String[] listAvailableLanguages(StreamInfo[] streams)
    {
        String[] ret = new String[streams.length];
        for (int i = 0; i < streams.length; i++)
            ret[i] = streams[i].getLang();
        
        return ret;
    }
    
    protected TIClip getCurrentClip()
    {
        TIClip[] clips = player.ti.getClips();
        int playitem = RegisterAccess.getInstance().getPSR(RegisterAccess.PSR_PLAYITEM_ID) - 1;
        
        if (clips.length >= playitem)
            return null;
        
        return clips[playitem];
    }
    
    public int selectLanguage(String language, StreamInfo[] streams) throws LanguageNotAvailableException, NotAuthorizedException
    {
        for (int i = 0; i < streams.length; i++) {
            if (streams[i].getLang().equals(language))
                return i + 1;
        }
        
        throw new LanguageNotAvailableException();
    }
    
    protected Handler player;
}
