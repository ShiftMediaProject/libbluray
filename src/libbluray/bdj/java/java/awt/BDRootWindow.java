/*
 * This file is part of libbluray
 * Copyright (C) 2012  Libbluray
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

import java.awt.event.KeyEvent;
import java.util.Arrays;
import java.util.Timer;
import java.util.TimerTask;

import org.videolan.Libbluray;

public class BDRootWindow extends Frame {

    public BDRootWindow () {
        super();
        setUndecorated(true);
        setBackground(new Color(0, 0, 0, 0));
        BDToolkit.setFocusedWindow(this);
    }

    public Rectangle getDirtyRect() {
        return dirty;
    }

    public void setBounds(int x, int y, int width, int height) {
        if (!isVisible()) {
            if ((width > 0) && (height > 0)) {
                if ((backBuffer == null) || (getWidth() * getHeight() < width * height)) {
                    backBuffer = new int[width * height];
                    dirty = new Rectangle(0, 0, width, height);
                    Arrays.fill(backBuffer, 0);
                }
            }
            super.setBounds(x, y, width, height);

            Libbluray.updateGraphic(width, height, null);

            dirty.x = 0;
            dirty.y = 0;
            dirty.width = width;
            dirty.height = height;
        }
    }

    public int[] getBdBackBuffer() {
        return backBuffer;
    }

    public Image getBackBuffer() {
        /* exists only in J2SE */
        org.videolan.Logger.getLogger("BDRootWindow").unimplemented("getBackBuffer");
        return null;
    }

    public void postKeyEvent(int id, int modifiers, int keyCode) {
        Component focusOwner = KeyboardFocusManager.getCurrentKeyboardFocusManager().getGlobalFocusOwner();
        if (focusOwner != null) {
            long when = System.currentTimeMillis();
            KeyEvent event;
            try {
                if (id == KeyEvent.KEY_TYPED)
                    event = new KeyEvent(focusOwner, id, when, modifiers, KeyEvent.VK_UNDEFINED, (char)keyCode);
                else
                    event = new KeyEvent(focusOwner, id, when, modifiers, keyCode, KeyEvent.CHAR_UNDEFINED);
                Toolkit.getEventQueue().postEvent(event);
                return;
            } catch (Throwable e) {
                System.err.println(" *** " + e + "");
            }
        } else {
            org.videolan.Logger.getLogger("BDRootWindow").error("*** KEY event dropped ***");
        }
    }

    public void notifyChanged() {
        synchronized (this) {
            changeCount++;
            if (timerTask == null) {
                timerTask = new RefreshTimerTask(this);
                timer.schedule(timerTask, 50, 50);
            }
        }
    }

    public void sync() {
        synchronized (this) {
            if (timerTask != null) {
                timerTask.cancel();
                timerTask = null;
            }
            changeCount = 0;
            //Libbluray.updateGraphic(getWidth(), getHeight(), backBuffer, dirty.x, dirty.y, dirty.width, dirty.height);
            Libbluray.updateGraphic(getWidth(), getHeight(), backBuffer);
            dirty.x = getWidth();
            dirty.y = getHeight();
            dirty.width = 0;
            dirty.height = 0;
        }
    }

    private class RefreshTimerTask extends TimerTask {
        public RefreshTimerTask(BDRootWindow window) {
            this.window = window;
            this.changeCount = window.changeCount;
        }

        public void run() {
            synchronized (window) {
                if (this.changeCount == window.changeCount)
                    window.sync();
                else
                    this.changeCount = window.changeCount;
            }
        }

        private BDRootWindow window;
        private int changeCount;
    }

    private int[] backBuffer = null;
    private Rectangle dirty;
    private int changeCount = 0;
    private Timer timer = new Timer();
    private TimerTask timerTask = null;

    private static final long serialVersionUID = -8325961861529007953L;
}
