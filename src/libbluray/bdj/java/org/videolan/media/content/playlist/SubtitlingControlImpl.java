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

import org.bluray.media.StreamNotAvailableException;
import org.bluray.media.SubtitleStyleNotAvailableException;
import org.bluray.media.SubtitlingControl;
import org.bluray.media.TextSubtitleNotAvailableException;
import org.bluray.ti.CodingType;
import org.davic.media.LanguageNotAvailableException;
import org.davic.media.NotAuthorizedException;
import org.dvb.media.SubtitleListener;
import org.videolan.Libbluray;
import org.videolan.StreamInfo;
import org.videolan.TIClip;

public class SubtitlingControlImpl extends StreamControl implements SubtitlingControl {

    protected SubtitlingControlImpl(Handler player)
    {
        super(player);
    }

    public void addSubtitleListener(SubtitleListener listener)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void removeSubtitleListener(SubtitleListener listener)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean isSubtitlingOn()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean setSubtitling(boolean value)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public Component getControlComponent()
    {
        throw new Error("Not implemented"); // TODO implement
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

    public void selectStreamNumber(int num) throws StreamNotAvailableException
    {
        Libbluray.writePSR(num, Libbluray.PSR_PG_STREAM);
    }

    public void selectSubtitle(int subtitle) throws StreamNotAvailableException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean setPipSubtitleMode(boolean mode)
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public void setSubtitleStyle(int style)
            throws TextSubtitleNotAvailableException,
            SubtitleStyleNotAvailableException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public String getCurrentLanguage()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public String[] listAvailableLanguages()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public int getCurrentStreamNumber()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public int[] listAvailableStreamNumbers()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public CodingType getCurrentSubtitleType()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public int getSubtitleStyle() throws TextSubtitleNotAvailableException,
            SubtitleStyleNotAvailableException
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public boolean isPipSubtitleMode()
    {
        throw new Error("Not implemented"); // TODO implement
    }

}
