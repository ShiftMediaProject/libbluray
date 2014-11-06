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

import javax.media.GainChangeListener;

import org.bluray.media.SecondaryGainControl;

public class SecondaryGainControlImpl implements SecondaryGainControl {
    public void setMute(boolean mute) {
        org.videolan.Logger.unimplemented("SecondaryGainControlImpl", "setMute");
        this.mute = mute;
    }

    public boolean getMute() {
        org.videolan.Logger.unimplemented("SecondaryGainControlImpl", "getMute");
        return this.mute;
    }

    public float setDB(float gain) {
        org.videolan.Logger.unimplemented("SecondaryGainControlImpl", "setDB");
        this.gain = gain;
        return this.gain;
    }

    public float getDB() {
        org.videolan.Logger.unimplemented("SecondaryGainControlImpl", "getDB");
        return gain;
    }

    public float setLevel(float level) {
        org.videolan.Logger.unimplemented("SecondaryGainControlImpl", "setLevel");
        this.level = level;
        return this.level;
    }

    public float getLevel() {
        org.videolan.Logger.unimplemented("SecondaryGainControlImpl", "getLevel");
        return level;
    }

    public void addGainChangeListener(GainChangeListener listener) {
        // TODO Not implemented
        org.videolan.Logger.unimplemented("SecondaryGainControlImpl", "addGainChangeListener");
    }

    public void removeGainChangeListener(GainChangeListener listener) {
        // TODO Not implemented
    }

    public Component getControlComponent() {
        return null;
    }

    private boolean mute = false;
    private float level = 0.0f;
    private float gain = 0.0f;
}
