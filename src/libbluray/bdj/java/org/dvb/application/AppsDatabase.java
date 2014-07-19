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

package org.dvb.application;

import java.util.LinkedList;
import java.util.Enumeration;
import org.videolan.BDJAppsDatabase;
import org.videolan.Logger;

public class AppsDatabase {
    static public AppsDatabase getAppsDatabase() {
        return BDJAppsDatabase.getAppsDatabase();
    }

    public int size() {
        logger.unimplemented("size");
        return 0;
    }

    public Enumeration getAppIDs(AppsDatabaseFilter filter) {
        logger.unimplemented("getAppIDs");
        return null;
    }

    public Enumeration getAppAttributes(AppsDatabaseFilter filter) {
        logger.unimplemented("getAppAttributes");
        return null;
    }

    public AppAttributes getAppAttributes(AppID key) {
        logger.unimplemented("getAppAttributes");
        return null;
    }

    public AppProxy getAppProxy(AppID key) {
        logger.unimplemented("getAppProxy");
        return null;
    }

    public void addListener(AppsDatabaseEventListener listener) {
        synchronized(listeners) {
            listeners.add(listener);
        }
    }

    public void removeListener(AppsDatabaseEventListener listener) {
        synchronized(listeners) {
            listeners.remove(listener);
        }
    }

    protected void notifyListeners(int id, AppID appid) {
        LinkedList list;
        synchronized(listeners) {
            list = (LinkedList)listeners.clone();
        }
        AppsDatabaseEvent event = new AppsDatabaseEvent(id, appid, this);
        for (int i = 0; i < list.size(); i++) {
            switch (id) {
            case AppsDatabaseEvent.APP_ADDED:
                ((AppsDatabaseEventListener)list.get(i)).entryAdded(event);
                break;
            case AppsDatabaseEvent.APP_CHANGED:
                ((AppsDatabaseEventListener)list.get(i)).entryChanged(event);
                break;
            case AppsDatabaseEvent.APP_DELETED:
                ((AppsDatabaseEventListener)list.get(i)).entryRemoved(event);
                break;
            case AppsDatabaseEvent.NEW_DATABASE:
                ((AppsDatabaseEventListener)list.get(i)).newDatabase(event);
                break;
            }
        }
    }

    private LinkedList listeners = new LinkedList();
    private static final Logger logger = Logger.getLogger(AppsDatabase.class.getName());
}
