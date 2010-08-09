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
import org.bluray.media.PrimaryAudioControl;
import org.bluray.media.StreamNotAvailableException;
import org.davic.media.LanguageNotAvailableException;
import org.davic.media.NotAuthorizedException;
import org.videolan.Libbluray;
import org.videolan.StreamInfo;
import org.videolan.TIClip;

public class PrimaryAudioControlImpl extends StreamControl implements PrimaryAudioControl {
    protected PrimaryAudioControlImpl(Handler player)
    {
        super(player);
    }
    
    public int[] listAvailableStreamNumbers()
    {
        TIClip clip = getCurrentClip();
        return listAvailableStreamNumbers(clip.getAudioStreams());
    }

    public int getCurrentStreamNumber()
    {
        return Libbluray.readPSR(Libbluray.PSR_PRIMARY_AUDIO_ID);
    }

    public void selectStreamNumber(int num) throws StreamNotAvailableException
    {
        Libbluray.writePSR(num, Libbluray.PSR_PRIMARY_AUDIO_ID);
    }

    public String[] listAvailableLanguages()
    {
        TIClip clip = getCurrentClip();       
        return listAvailableLanguages(clip.getAudioStreams());
    }

    public String getCurrentLanguage()
    {
        TIClip clip = getCurrentClip();
        StreamInfo[] streams = clip.getAudioStreams();
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
        StreamInfo[] streams = clip.getAudioStreams();
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
