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

package java.awt.peer;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;

import org.videolan.Logger;

public class BDFramePeer extends BDComponentPeer implements FramePeer
{
    public BDFramePeer(Frame frame, BDRootWindow rootWindow) {
        super(frame.getToolkit(), frame);
        this.rootWindow = rootWindow;
    }

    public void setTitle(String title) {
    }

    public void setIconImage(Image im) {
    }

    public void setMenuBar(MenuBar mb) {
    }

    public void setResizable(boolean resizeable) {
    }

    public void setState(int state) {
    }

    public int getState() {
        return Frame.NORMAL;
    }

    public void setMaximizedBounds(Rectangle bounds) {
    }

    public void toFront() {
    }

    public void toBack() {
    }

    public int handleFocusTraversalEvent(KeyEvent e) {
        logger.unimplemented("handleFocusTravelsalEvent");
        return -1;
    }

    //
    // ContainerPeer
    //

    public Insets getInsets() {
        return insets;
    }

    public void beginValidate() {
    }

    public void endValidate() {
    }

    public void beginLayout() {
    }

    public void endLayout() {
    }

    public boolean isPaintPending() {
        return false;
    }

    public Insets insets() {
        return getInsets();
    }

    public void applyShape(sun.java2d.pipe.Region r) {
    }

    public Rectangle getBounds() {
        return rootWindow.getBounds();
    }

    public void layout() {
    }

    public boolean isReparentSupported() {
        return false;
    }

    public void reparent(ContainerPeer p) {
    }

    public void flip(int a, int b, int c, int d, java.awt.BufferCapabilities.FlipContents e) {
    }

    public boolean requestFocus(Component c, boolean a, boolean b, long l, sun.awt.CausedFocusEvent.Cause d) {
        if (c == null) {
            return true;
        }
        Toolkit.getDefaultToolkit().getSystemEventQueue().postEvent(new FocusEvent(c, FocusEvent.FOCUS_GAINED));
        return true;
    }

    public void setBounds(int a, int b, int c, int d, int e) {
    }

    public void setBoundsPrivate(int a, int b, int c, int d) {
    }

    public Rectangle getBoundsPrivate() {
        return null;
    }

    public void repositionSecurityWarning() {
    }

    public void  updateWindow(BufferedImage b) {
        logger.unimplemented("updateWindow");
    }

    public void setOpaque(boolean b) {
    }

    public void setOpacity(float f) {
    }

    public void updateIconImages() {
    }

    public void updateMinimumSize() {
    }

    public void setModalBlocked(Dialog d,boolean b) {
    }

    public boolean requestWindowFocus()  {
        return true;
    }

    public void updateFocusableWindowState() {
    }

    public void setAlwaysOnTop(boolean b) {
    }

    public boolean isRestackSupported() {
        return false;
    }

    public void restack() {
    }

    //
    // ComponentPeer
    //

    public void setVisible(boolean b) {
        //Toolkit.getDefaultToolkit().getSystemEventQueue().postEvent(new WindowEvent((Frame)component, WindowEvent.WINDOW_ACTIVATED));
        if (b == true) {
            component.paint(getGraphics());
        }
    }

    public Graphics getGraphics() {
        return new BDWindowGraphics(rootWindow);
    }

    public Image createImage(int width, int height) {
        return ((BDToolkit)BDToolkit.getDefaultToolkit()).createImage((Component)null, width, height);
    }

    private BDRootWindow rootWindow;
    private Insets insets = new Insets(0, 0, 0, 0);

    private static final Logger logger = Logger.getLogger(BDFramePeer.class.getName());
}
