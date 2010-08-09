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
import java.awt.Dimension;
import java.awt.Rectangle;

import javax.tv.media.AWTVideoSize;
import javax.tv.media.AWTVideoSizeControl;

import org.videolan.BDJLoader;
import org.videolan.bdjo.GraphicsResolution;

public class AWTVideoSizeControlImpl implements AWTVideoSizeControl {
    protected AWTVideoSizeControlImpl(Handler player)
    {
        this.player = player;
    }
    
    public Component getControlComponent()
    {
        return null;
    }

    public AWTVideoSize getSize()
    {
        return getDefaultSize(); // FIXME: return actual video size instead of just default
    }

    public AWTVideoSize getDefaultSize()
    {
        GraphicsResolution res = BDJLoader.getBdjo().getTerminalInfo().getResolution();
        Rectangle rect = new Rectangle(res.getWidth(), res.getHeight());
        return new AWTVideoSize(rect, rect);
    }

    public Dimension getSourceVideoSize()
    {
        GraphicsResolution res = BDJLoader.getBdjo().getTerminalInfo().getResolution();
        return new Dimension(res.getWidth(), res.getHeight());
    }

    public boolean setSize(AWTVideoSize size)
    {
        return false; // FIXME: possibly allow changing of video size in the future?
    }

    @Override
    public AWTVideoSize checkSize(AWTVideoSize size)
    {
        return getDefaultSize(); // FIXME: allow sizes other than default
    }

    Handler player;
}
