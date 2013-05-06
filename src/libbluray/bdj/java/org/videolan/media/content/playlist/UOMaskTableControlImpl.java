/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 * Copyright (C) 2012  Petri Hintukainen <phintuka@users.sourceforge.net>
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

import org.bluray.media.UOMaskTableControl;
import org.bluray.media.UOMaskTableListener;
import org.bluray.media.UOMaskTableChangedEvent;
import org.bluray.media.UOMaskedEvent;

import org.videolan.BDJAction;
import org.videolan.BDJActionManager;
import org.videolan.Libbluray;
import org.videolan.PlaylistInfo;

public class UOMaskTableControlImpl implements UOMaskTableControl {
    protected UOMaskTableControlImpl(Handler player) {
        this.player = player;
    }

    public Component getControlComponent() {
        return null;
    }

    public void addUOMaskTableEventListener(UOMaskTableListener listener) {
        synchronized(listeners) {
            listeners.add(listener);
        }
    }

    public void removeUOMaskTableEventListener(UOMaskTableListener listener) {
        synchronized(listeners) {
            listeners.remove(listener);
        }
    }

    public boolean[] getMaskedUOTable() {
        long mask = Libbluray.getUOMask();
        boolean[] table = new boolean[64];
        for (int i = 0; i < table.length; i++)
            if (0L != (mask & 1L << i))
                table[i] = true;
            else
                table[i] = false;

        return table;
    }

    protected void onUOMasked(int position) {
        // TODO: this method is not called
        notifyListeners(new UOMaskedEvent(this, position));
    }

    protected void onPlayItemReach(int param) {
        // TODO: check if masked UO table actually changed
        notifyListeners(new UOMaskTableChangedEvent(this));
    }

    private void notifyListeners(Object event) {
        synchronized (listeners) {
            if (!listeners.isEmpty())
                BDJActionManager.getInstance().putCallback(
                        new UOMaskTableCallback(this, event));
        }
    }

    private class UOMaskTableCallback extends BDJAction {
        private UOMaskTableCallback(UOMaskTableControlImpl control, Object event) {
            this.control = control;
            this.event = event;
        }

        protected void doAction() {
            LinkedList list;
            synchronized (control.listeners) {
                list = (LinkedList)control.listeners.clone();
            }
            if (event instanceof UOMaskTableChangedEvent) {
                for (int i = 0; i < list.size(); i++)
                    ((UOMaskTableListener)list.get(i)).receiveUOMaskTableChangedEvent((UOMaskTableChangedEvent)event);
            }
            else if (event instanceof UOMaskedEvent) {
                for (int i = 0; i < list.size(); i++)
                    ((UOMaskTableListener)list.get(i)).receiveUOMaskedEvent((UOMaskedEvent)event);
            }
        }

        private UOMaskTableControlImpl control;
        private Object event;
    }

    private LinkedList listeners = new LinkedList();
    private Handler player;
}
