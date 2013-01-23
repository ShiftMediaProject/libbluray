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
import java.awt.image.*;

import org.videolan.Logger;

public abstract class BDComponentPeer implements ComponentPeer
{
    public BDComponentPeer(Toolkit toolkit, Component component) {
        this.toolkit = toolkit;
        this.component = component;
        setBounds( component.getX(), component.getY(), component.getWidth(), component.getHeight() );
    }

    public void paint(Graphics g) {
    }

    public void repaint(long tm, int x, int y, int width, int height) {
    }

    public void print(Graphics g) {
    }

    public Graphics getGraphics() {
        Component parent = component.getParent();
        if( parent != null ) {
            return parent.getGraphics().create(location.x, location.y, size.width, size.height);
        }
        logger.error("getGraphics(): no parent !");
        throw new Error();
    }

    public GraphicsConfiguration getGraphicsConfiguration() {
        return null;
    }

    public void setBounds(int x, int y, int width, int height) {
        location.x = x;
        location.y = y;
        size.width = width;
        size.height = height;
    }

    public Point getLocationOnScreen() {
        Point screen = new Point(location);
        Component parent = component.getParent();
        if( parent != null ) {
            Point parentScreen = parent.getLocationOnScreen();
            screen.translate(parentScreen.x, parentScreen.y);
        }
        return screen;
    }

    public Dimension getPreferredSize() {
        return size;
    }

    public Dimension getMinimumSize() {
        return size;
    }

    public void setVisible(boolean b) {
    }

    public void setEnabled(boolean b) {
    }

    public boolean isObscured() {
        return false;
    }

    public boolean canDetermineObscurity() {
        return false;
    }

    public boolean isFocusable() {
        return true;
    }

    public boolean requestFocus(Component lightweightChild, boolean temporary, boolean focusedWindowChangeAllowed, long time) {
        return true;
    }

    public boolean isFocusTraversable() {
        return true;
    }

    public void handleEvent(AWTEvent e) {
    }

    public void coalescePaintEvent( PaintEvent e ) {
    }

    public boolean handlesWheelScrolling() {
        return false;
    }

    public ColorModel getColorModel() {
        return toolkit.getColorModel();
    }

    public void setForeground(Color c) {
    }

    public void setBackground(Color c) {
    }

    public FontMetrics getFontMetrics( Font font ) {
        return null;
    }

    public void setFont( Font f ) {
    }

    public void updateCursorImmediately() {
    }

    public Toolkit getToolkit() {
        return toolkit;
    }

    public void dispose() {
    }

    public void createBuffers(int x, BufferCapabilities bufferCapabilities) {
    }

    public void destroyBuffers() {
    }

    public void flip(BufferCapabilities.FlipContents flipContents) {
    }

    public Image getBackBuffer() {
        logger.unimplemented("getBackBuffer");
        throw new Error();
    }

    public Image createImage(ImageProducer producer) {
        logger.unimplemented("createImage");
        throw new Error("Not implemented");
    }

    public Image createImage(int width, int height) {
        Component parent = component.getParent();
        if( parent != null ) {
            return parent.createImage( width, height );
        }
        logger.error("createImage(): no parent !");
        throw new Error();
    }

    public VolatileImage createVolatileImage(int width, int height) {
        logger.unimplemented("createVolatileImage");
        throw new Error();
    }

    public boolean prepareImage(Image img, int w, int h, ImageObserver o) {
        return ((BDToolkit)toolkit).prepareImage(img, w, h, o);
    }

    public int checkImage(Image img, int w, int h, ImageObserver o) {
        return ((BDToolkit)toolkit).checkImage(img, w, h, o);
    }

    public Dimension preferredSize() {
        return getPreferredSize();
    }

    public Dimension minimumSize() {
        return getMinimumSize();
    }

    public void show() {
        setVisible(true);
    }

    public void hide() {
        setVisible(false);
    }

    public void enable() {
        setEnabled(true);
    }

    public void disable() {
        setEnabled(false);
    }

    public void reshape(int x, int y, int width, int height) {
        setBounds(x, y, width, height);
    }

    protected Component component;
    protected Toolkit toolkit;
    protected Point location = new Point();
    protected Dimension size = new Dimension();

    private static final Logger logger = Logger.getLogger(BDComponentPeer.class.getName());
}
