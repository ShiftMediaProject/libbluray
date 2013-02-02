/*
 * This file is part of libbluray
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

package java.awt.peer;

import java.awt.Component;
import java.awt.KeyboardFocusManager;
import java.awt.Window;
import java.lang.reflect.Field;

public class BDKeyboardFocusManagerPeer implements KeyboardFocusManagerPeer {
    private Component focusOwner;
    private Window window; /* used in java 6 only */

    private static KeyboardFocusManager kfm = null; /* used in java 7 only */

    public static void init(Window window)
    {
        /* running in java 7 ? */
        if (kfm != null)
            return;

        /* replace default keyboard focus manager peer */
        Field kbPeer;
        try {
            Class c = Class.forName("java.awt.KeyboardFocusManager");
            kbPeer = c.getDeclaredField("peer");
            kbPeer.setAccessible(true);
        } catch (ClassNotFoundException e) {
            throw new Error("java.awt.KeyboardFocusManager not found");
        } catch (SecurityException e) {
            throw new Error("java.awt.KeyboardFocusManager not accessible");
        } catch (NoSuchFieldException e) {
            throw new Error("java.awt.KeyboardFocusManager.peer not found");
        }
        try {
            kbPeer.set(KeyboardFocusManager.getCurrentKeyboardFocusManager(),
                       new BDKeyboardFocusManagerPeer(window));
        } catch (java.lang.IllegalAccessException e) {
            throw new Error("java.awt.KeyboardFocusManager.peer not accessible:" + e);
        }
    }

    /* used in java 7 only */
    public static KeyboardFocusManagerPeer init(KeyboardFocusManager _kfm) {
        kfm = _kfm;
        return new BDKeyboardFocusManagerPeer(null);
    }

    private BDKeyboardFocusManagerPeer(Window w) {
        window = w;
        focusOwner = null;
    }

    public void clearGlobalFocusOwner(Window w) {
    }

    public Component getCurrentFocusOwner() {
        return focusOwner;
    }

    public void setCurrentFocusOwner(Component c) {
        focusOwner = c;
    }

    /* java 6 only */
    public Window getCurrentFocusedWindow() {
        return window;
    }
};


