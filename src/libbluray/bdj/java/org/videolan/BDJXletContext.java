/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 * Copyright (C) 2013  Petri Hintukainen <phintuka@users.sourceforge.net>
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
package org.videolan;

import java.awt.Container;
import java.awt.EventQueue;
import java.util.LinkedList;
import java.security.AccessController;
import java.security.PrivilegedAction;

import javax.microedition.xlet.UnavailableContainerException;

import org.dvb.application.AppID;
import org.dvb.application.AppProxy;
import org.dvb.application.AppsDatabase;
import org.havi.ui.HSceneFactory;
import org.videolan.bdjo.AppCache;
import org.videolan.bdjo.AppEntry;

public class BDJXletContext implements javax.tv.xlet.XletContext, javax.microedition.xlet.XletContext {
    public BDJXletContext(AppEntry entry, AppCache[] caches, Container container) {
        this.appid = entry.getIdentifier();
        this.args = entry.getParams();
        this.loader = BDJClassLoader.newInstance(
                caches,
                entry.getBasePath(),
                entry.getClassPathExt(),
                entry.getInitialClass());
        this.container = container;

        this.threadGroup = new BDJThreadGroup(Integer.toHexString(appid.getOID()) + "." +
                                              Integer.toHexString(appid.getAID()) + "." +
                                              entry.getInitialClass(),
                                              this);
    }

    public Object getXletProperty(String key) {
        if (key.equals(javax.tv.xlet.XletContext.ARGS) ||
            key.equals(javax.microedition.xlet.XletContext.ARGS))
            return args;
        else if (key.equals("dvb.org.id"))
            return Integer.toHexString(appid.getOID());
        else if (key.equals("dvb.app.id"))
            return Integer.toHexString(appid.getAID());
        else if (key.equals("org.dvb.application.appid"))
            return appid;
        return null;
    }

    public void notifyDestroyed() {
        AppProxy proxy = AppsDatabase.getAppsDatabase().getAppProxy(appid);
        if (proxy instanceof BDJAppProxy)
            ((BDJAppProxy)proxy).notifyDestroyed();
    }

    public void notifyPaused() {
        AppProxy proxy = AppsDatabase.getAppsDatabase().getAppProxy(appid);
        if (proxy instanceof BDJAppProxy)
            ((BDJAppProxy)proxy).notifyPaused();
    }

    public void resumeRequest() {
        AppProxy proxy = AppsDatabase.getAppsDatabase().getAppProxy(appid);
        if (proxy instanceof BDJAppProxy)
            ((BDJAppProxy)proxy).resume();
    }

    public Container getContainer() throws UnavailableContainerException {
        if (container == null) {
            logger.error("getContainer(): container is null");
            throw new UnavailableContainerException();
        }
        return container;
    }

    public ClassLoader getClassLoader() {
        return loader;
    }

    protected AppProxy getAppProxy() {
        return AppsDatabase.getAppsDatabase().getAppProxy(appid);
    }

    public boolean isDestroyed() {
        return AppsDatabase.getAppsDatabase().getAppProxy(appid).getState() == AppProxy.DESTROYED;
    }

    public BDJThreadGroup getThreadGroup() {
        return threadGroup;
    }

    protected void setEventQueue(EventQueue eq) {
        eventQueue = eq;
    }

    public EventQueue getEventQueue() {
        return eventQueue;
    }

    protected int numEventQueueThreads() {
        int cnt = 0;
        if (eventQueue != null) {
            Thread t = java.awt.BDJHelper.getEventDispatchThread(eventQueue);
            if (t != null && t.isAlive()) {
                cnt++;
            }
        }
        return cnt;
    }

    public void setSceneFactory(HSceneFactory f) {
        sceneFactory = f;
    }

    public HSceneFactory getSceneFactory() {
        return sceneFactory;
    }

    protected void addIxcThread(Thread thread) {
        synchronized (ixcThreads) {
            ixcThreads.addLast(thread);
        }
    }
    protected void removeIxcThread(Thread thread) {
        synchronized (ixcThreads) {
            ixcThreads.remove(thread);
        }
    }
    protected void stopIxcThreads() {
        synchronized (ixcThreads) {
            while (!ixcThreads.isEmpty()) {
                Thread thread = (Thread)ixcThreads.removeFirst();
                logger.info("Stopping remote thread " + thread);
                thread.interrupt();
                try {
                    thread.join(500);
                } catch (Throwable t) {
                }
                if (thread.isAlive()) {
                    PortingHelper.stopThread(thread);
                }
                if (thread.isAlive()) {
                    logger.error("Error stopping remote thread " + thread);
                }
            }
        }
    }

    public static BDJXletContext getCurrentContext() {
        Object obj = AccessController.doPrivileged(
                new PrivilegedAction() {
                    public Object run() {
                        ThreadGroup group = Thread.currentThread().getThreadGroup();
                        while ((group != null) && !(group instanceof BDJThreadGroup))
                            group = group.getParent();
                        return group;
                    }
                }
            );
        if (obj == null)
            return null;
        return ((BDJThreadGroup)obj).getContext();
    }

    protected void setArgs(String[] args) {
        this.args = args;
    }

    protected void update(AppEntry entry, AppCache[] caches) {
        args = entry.getParams();
        loader.update(
                caches,
                entry.getBasePath(),
                entry.getClassPathExt(),
                entry.getInitialClass());
    }

    protected void release() {
        stopIxcThreads();

        if (sceneFactory != null) {
            sceneFactory.dispose();
            sceneFactory = null;
        }

        EventQueue eq = eventQueue;
        eventQueue = null;
        if (eq != null) {
            java.awt.BDJHelper.stopEventQueue(eq);
        }

        threadGroup.stopAll(1000);

        threadGroup = null;
        loader = null;
        container = null;
    }

    private String[] args;
    private AppID appid;
    private BDJClassLoader loader;
    private Container container;
    private EventQueue eventQueue = null;
    private HSceneFactory sceneFactory = null;
    private BDJThreadGroup threadGroup = null;
    private LinkedList ixcThreads = new LinkedList();
    private static final Logger logger = Logger.getLogger(BDJXletContext.class.getName());
}
