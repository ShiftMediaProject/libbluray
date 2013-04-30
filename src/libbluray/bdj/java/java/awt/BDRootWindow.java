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

    public Area getDirtyArea() {
        return dirty;
    }

    public void setBounds(int x, int y, int width, int height) {
        if (!isVisible()) {
            if ((width > 0) && (height > 0)) {
                if ((backBuffer == null) || (getWidth() * getHeight() < width * height)) {
                    backBuffer = new int[width * height];
                    Arrays.fill(backBuffer, 0);
                }
            }
            super.setBounds(x, y, width, height);

            Libbluray.updateGraphic(width, height, null);

            dirty.add(new Rectangle(0, 0, width - 1, height - 1));
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

    public void notifyChanged() {
        if (!isVisible()) {
            org.videolan.Logger.getLogger("BDRootWindow").error("sync(): not visible");
            return;
        }
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

            Area a = dirty.getBounds();
            dirty.clear();

            if (!a.isEmpty()) {
                Libbluray.updateGraphic(getWidth(), getHeight(), backBuffer, a.x0, a.y0, a.x1, a.y1);
            }
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

    public void dispose()
    {
        if (isVisible()) {
            hide();
        }
        if (timerTask != null) {
            timerTask.cancel();
            timerTask = null;
        }
        if (timer != null) {
            timer.cancel();
            timer = null;
        }

        BDToolkit.setFocusedWindow(null);

        super.dispose();
    }

    private int[] backBuffer = null;
    private Area dirty = new Area();
    private int changeCount = 0;
    private Timer timer = new Timer();
    private TimerTask timerTask = null;

    private static final long serialVersionUID = -8325961861529007953L;
}
