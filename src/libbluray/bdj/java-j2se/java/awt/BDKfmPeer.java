/*
 * This file is part of libbluray
 * Copyright (C) 2012  libbluray
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

package java.awt;

import java.awt.peer.KeyboardFocusManagerPeer;
import java.lang.reflect.Field;

class BDKfmPeer implements KeyboardFocusManagerPeer {
    private Component focusOwner;
    private Window window;

    public static void init(Window window)
    {
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
            kbPeer.set(KeyboardFocusManager.getCurrentKeyboardFocusManager(), new BDKfmPeer(window));
        } catch (java.lang.IllegalAccessException e) {
            throw new Error("java.awt.KeyboardFocusManager.peer not accessible:" + e);
        }
    }

    private BDKfmPeer(Window w) {
        window = w;
    }

    public void clearGlobalFocusOwner(java.awt.Window w) {
    }

    public Component getCurrentFocusOwner() {
        return focusOwner;
    }

    public void setCurrentFocusOwner(java.awt.Component c) {
        focusOwner = c;
    }

    public java.awt.Window getCurrentFocusedWindow()  {
        System.err.println("************************************* focused windw **********************");
        return window;
    }
};


