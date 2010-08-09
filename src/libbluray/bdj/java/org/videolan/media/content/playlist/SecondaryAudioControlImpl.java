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

import org.bluray.media.SecondaryAudioControl;
import org.bluray.media.StreamNotAvailableException;
import org.davic.media.LanguageNotAvailableException;
import org.davic.media.NotAuthorizedException;
import org.videolan.Libbluray;
import org.videolan.StreamInfo;
import org.videolan.TIClip;

public class SecondaryAudioControlImpl extends StreamControl implements SecondaryAudioControl {
    protected SecondaryAudioControlImpl(Handler player)
    {
        super(player);
    }
    
    public int[] listAvailableStreamNumbers()
    {
        TIClip clip = getCurrentClip();
        return listAvailableStreamNumbers(clip.getSecAudioStreams());
    }

    public int getCurrentStreamNumber()
    {
        return Libbluray.readPSR(Libbluray.PSR_SECONDARY_AUDIO_VIDEO) & 0xff;
    }

    public void selectStreamNumber(int num) throws StreamNotAvailableException
    {
        if (num < 0 || num > 0xff)
            throw new IllegalArgumentException();
        
        int old = Libbluray.readPSR(Libbluray.PSR_SECONDARY_AUDIO_VIDEO) & 0xffffff00;
        Libbluray.writePSR(old | num, Libbluray.PSR_SECONDARY_AUDIO_VIDEO);
    }

    public String[] listAvailableLanguages()
    {
        TIClip clip = getCurrentClip();       
        return listAvailableLanguages(clip.getSecAudioStreams());
    }

    public String getCurrentLanguage()
    {
        TIClip clip = getCurrentClip();
        StreamInfo[] streams = clip.getSecAudioStreams();
        return streams[getCurrentStreamNumber() - 1].getLang();
    }
    
    public String selectDefaultLanguage() throws NotAuthorizedException
    {
        // FIXME: should add ability to select the default language
        try {
            selectLanguage("eng");
        } catch (LanguageNotAvailableException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return "eng";
    }

    public void selectLanguage(String language)
            throws LanguageNotAvailableException, NotAuthorizedException
    {
        TIClip clip = getCurrentClip();
        StreamInfo[] streams = clip.getSecAudioStreams();
        try {
            selectStreamNumber(selectLanguage(language, streams));
        } catch (StreamNotAvailableException e) {
            throw new LanguageNotAvailableException();
        }
    }

    public Component getControlComponent()
    {
        return null;
    }
}
