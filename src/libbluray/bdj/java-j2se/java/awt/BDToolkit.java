/*
 * This file is part of libbluray
 * Copyright (C) 2012  libbluray
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

package java.awt;

import java.awt.image.ColorModel;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.peer.KeyboardFocusManagerPeer;
import java.io.File;
import java.net.URL;
import java.util.Collections;
import java.util.Hashtable;
import java.util.WeakHashMap;
import java.util.Map;
import java.util.Iterator;

import sun.awt.image.ByteArrayImageSource;
import sun.awt.image.FileImageSource;
import sun.awt.image.URLImageSource;
import sun.awt.KeyboardFocusManagerPeerProvider;

import java.awt.peer.BDFramePeer;
import java.awt.peer.BDKeyboardFocusManagerPeer;

import org.videolan.BDJXletContext;
import org.videolan.Logger;

public class BDToolkit extends Toolkit implements KeyboardFocusManagerPeerProvider {
    private EventQueue eventQueue = new EventQueue();
    private BDGraphicsEnvironment localEnv = new BDGraphicsEnvironment();
    private BDGraphicsConfiguration defaultGC = (BDGraphicsConfiguration)localEnv.getDefaultScreenDevice().getDefaultConfiguration();
    private static Hashtable cachedImages = new Hashtable();
    private static final Logger logger = Logger.getLogger(BDToolkit.class.getName());

    public BDToolkit () {
    }

    public static void shutdown() {
        Toolkit toolkit = getDefaultToolkit();
        if (toolkit instanceof BDToolkit) {
            ((BDToolkit)toolkit).dispose();
        }
    }

    public void dispose() {
        if (eventQueue != null) {
            BDJHelper.stopEventQueue(eventQueue);
            eventQueue = null;
        }

        KeyboardFocusManager.getCurrentKeyboardFocusManager().clearGlobalFocusOwner();
        KeyboardFocusManager.getCurrentKeyboardFocusManager().setGlobalCurrentFocusCycleRoot(null);
        KeyboardFocusManager.getCurrentKeyboardFocusManager().setGlobalFocusedWindow(null);
        KeyboardFocusManager.getCurrentKeyboardFocusManager().setGlobalActiveWindow(null);
        KeyboardFocusManager.getCurrentKeyboardFocusManager().setGlobalPermanentFocusOwner(null);

        BDKeyboardFocusManagerPeer.shutdown();

        KeyboardFocusManager.setCurrentKeyboardFocusManager(null);
    }

    public static void setFocusedWindow(Window window) {
        /* hook KeyboardFocusManagerPeer (not doing this leads to crash) */
        BDKeyboardFocusManagerPeer.init(window);
    }

    public KeyboardFocusManagerPeer createKeyboardFocusManagerPeer(KeyboardFocusManager kfm)
    {
        return BDKeyboardFocusManagerPeer.getInstance();
    }

    public Dimension getScreenSize() {
        Rectangle dims = defaultGC.getBounds();
        return new Dimension(dims.width, dims.height);
    }

    Graphics getGraphics(Window window) {
        if (!(window instanceof BDRootWindow)) {
            System.err.println("getGraphics(): not BDRootWindow");
            throw new Error("Not implemented");
        }
        return new BDWindowGraphics((BDRootWindow)window);
    }

    GraphicsEnvironment getLocalGraphicsEnvironment() {
        return localEnv;
    }

    public int getScreenResolution() {
        return 72;
    }

    public ColorModel getColorModel() {
        return defaultGC.getColorModel();
    }

    public String[] getFontList() {
        return BDFontMetrics.getFontList();
    }

    public FontMetrics getFontMetrics(Font font) {
        return BDFontMetrics.getFontMetrics(font);
    }

    public void sync() {
        org.videolan.GUIManager.getInstance().sync();
    }

    static void clearCache(BDImage image) {
        synchronized (cachedImages) {
            Iterator i = cachedImages.entrySet().iterator();
            while (i.hasNext()) {
                Map.Entry entry = (Map.Entry) i.next();
                if (entry.getValue() == image) {
                    i.remove();
                    return;
                }
            }
        }
    }

    public Image getImage(String filename) {
        if (cachedImages.containsKey(filename))
            return (Image)cachedImages.get(filename);
        Image newImage = createImage(filename);
        if (newImage != null)
            cachedImages.put(filename, newImage);
        return newImage;
    }

    public Image getImage(URL url) {
        if (cachedImages.containsKey(url))
            return (Image)cachedImages.get(url);
        Image newImage = createImage(url);
        if (newImage != null)
            cachedImages.put(url, newImage);
        return newImage;
    }

    public Image createImage(String filename) {
        if (!new File(filename).exists()) {
            BDJXletContext ctx = BDJXletContext.getCurrentContext();
            if (ctx != null) {
                ClassLoader cldr = (ClassLoader)ctx.getClassLoader();
                URL url = cldr.getResource(filename);
                if (url != null) {
                    logger.warning("" + filename + " translated to " + url);
                    return createImage(url);
                } else {
                    logger.error("ClassLoader failed to translate " + filename);
                }
            } else {
                logger.error("createImage(" + filename + "): no XletContext available!\n" + logger.dumpStack());
            }
        }

        ImageProducer ip = new FileImageSource(filename);
        Image newImage = createImage(ip);
        return newImage;
    }

    public Image createImage(URL url) {
        ImageProducer ip = new URLImageSource(url);
        Image newImage = createImage(ip);
        return newImage;
    }

    public Image createImage(byte[] imagedata,
        int imageoffset,
        int imagelength) {
        ImageProducer ip = new ByteArrayImageSource(imagedata, imageoffset, imagelength);
        Image newImage = createImage(ip);
        return newImage;
    }

    public Image createImage(ImageProducer producer) {
        return new BDImageConsumer(producer);
    }

    public Image createImage(Component component, int width, int height) {
        return new BDImage(component, width, height, defaultGC);
   }

    public boolean prepareImage(Image image, int width, int height, ImageObserver observer) {
        if (!(image instanceof BDImageConsumer))
            return true;
        BDImageConsumer img = (BDImageConsumer)image;
        return img.prepareImage(observer);
    }

    public int checkImage(Image image, int width, int height,
        ImageObserver observer) {
        if (!(image instanceof BDImageConsumer)) {
            return ImageObserver.ALLBITS;
        }
        BDImageConsumer img = (BDImageConsumer)image;
        return img.checkImage(observer);
    }

    public void beep() {
    }

    // mapping of Components to AppContexts, WeakHashMap<Component,AppContext>
    private static final Map contextMap =
        Collections.synchronizedMap(new WeakHashMap());

    public static void addComponent(Component component) {

        BDJXletContext context = BDJXletContext.getCurrentContext();
        if (context == null) {
            logger.warning("addComponent() outside of app context");
            return;
        }
        contextMap.put(component, context);
    }

    public static EventQueue getEventQueue(Component component) {
        if (component != null) {
            do {
                BDJXletContext ctx = (BDJXletContext)contextMap.get(component);
                if (ctx != null) {
                    EventQueue eq = ctx.getEventQueue();
                    if (eq == null) {
                        logger.warning("getEventQueue() failed: no context event queue");
                    }
                    return eq;
                }

                component = component.getParent();
            } while (component != null);

            logger.warning("getEventQueue() failed: no context");
        }
        return null;
    }

    protected EventQueue getSystemEventQueueImpl() {
        BDJXletContext ctx = BDJXletContext.getCurrentContext();
        if (ctx != null) {
            EventQueue eq = ctx.getEventQueue();
            if (eq != null) {
                return eq;
            }
        }
        return eventQueue;
    }

    public Map mapInputMethodHighlight(java.awt.im.InputMethodHighlight h) {
        throw new Error("Not implemented");
    }
    public  boolean isModalExclusionTypeSupported(Dialog.ModalExclusionType modalExclusionType) {
        throw new Error("Not implemented");
    }
    public  boolean isModalityTypeSupported(Dialog.ModalityType modalityType) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.FramePeer createFrame(Frame target) {
        return new BDFramePeer(target, (BDRootWindow)target);
    }

    protected java.awt.peer.ButtonPeer createButton(Button target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.CanvasPeer createCanvas(Canvas target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.CheckboxPeer createCheckbox(Checkbox target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.CheckboxMenuItemPeer createCheckboxMenuItem(CheckboxMenuItem target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.ChoicePeer createChoice(Choice target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.DesktopPeer createDesktopPeer(Desktop target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.DialogPeer createDialog(Dialog target) {
        throw new Error("Not implemented");
    }
    public java.awt.dnd.peer.DragSourceContextPeer createDragSourceContextPeer(java.awt.dnd.DragGestureEvent dge) {
        throw new Error("Not implemented");
    }
    public java.awt.peer.FileDialogPeer createFileDialog(FileDialog target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.LabelPeer createLabel(Label target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.ListPeer createList(List target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.MenuPeer createMenu(Menu target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.MenuBarPeer createMenuBar(MenuBar target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.MenuItemPeer createMenuItem(MenuItem target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.PanelPeer createPanel(Panel target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.PopupMenuPeer createPopupMenu(PopupMenu target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.ScrollbarPeer createScrollbar(Scrollbar target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.ScrollPanePeer createScrollPane(ScrollPane target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.TextAreaPeer createTextArea(TextArea target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.TextFieldPeer createTextField(TextField target) {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.WindowPeer createWindow(Window target) {
        throw new Error("Not implemented");
    }
    public java.awt.datatransfer.Clipboard getSystemClipboard() {
        throw new Error("Not implemented");
    }
    public PrintJob getPrintJob(Frame frame, String jobtitle, java.util.Properties props)  {
        throw new Error("Not implemented");
    }
    protected java.awt.peer.FontPeer getFontPeer(String name, int style) {
        throw new Error("Not implemented");
    }
}
