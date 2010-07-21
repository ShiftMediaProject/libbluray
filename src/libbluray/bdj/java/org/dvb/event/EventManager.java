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

package org.dvb.event;

import java.util.Iterator;
import java.util.LinkedList;

import javax.tv.xlet.XletContext;

import org.davic.resources.ResourceClient;
import org.davic.resources.ResourceServer;
import org.davic.resources.ResourceStatusEvent;
import org.davic.resources.ResourceStatusListener;
import org.videolan.BDJInputListener;
import org.videolan.BDJLoader;

public class EventManager implements ResourceServer, BDJInputListener {
    protected EventManager()
    {
        BDJLoader.inputListener = this;
    }

    public static EventManager getInstance()
    {
        synchronized (EventManager.class) {
            if (instance == null) {
                instance = new EventManager();
            }
        }
        
        return instance;
    }

    public boolean addUserEventListener(UserEventListener listener,
            ResourceClient client, UserEventRepository userEvents)
    {
        if (listener == null || client == null || userEvents == null)
            throw new NullPointerException();
        
        // first remove exclusive events
        UserEventItem newItem = new UserEventItem(BDJLoader.getContext(), listener, client, userEvents);
        cleanupReservedEvents(newItem);
        
        exclusiveEvents.add(newItem);
        
        sendEvent(new UserEventUnavailableEvent(userEvents));
        
        return true;
    }

    public void addUserEventListener(UserEventListener listener,
            UserEventRepository userEvents)
    {
        if (listener == null || userEvents == null)
            throw new NullPointerException();
        
        UserEventItem newItem = new UserEventItem(BDJLoader.getContext(), listener, null, userEvents);
        if (checkAlreadyReserved(newItem) != null)
            return;
        
        events.add(newItem);
    }

    public boolean addExclusiveAccessToAWTEvent(ResourceClient client,
            UserEventRepository userEvents)
    {
        if (client == null || userEvents == null)
            throw new NullPointerException();

        // first remove exclusive events
        UserEventItem newItem = new UserEventItem(BDJLoader.getContext(), null, client, userEvents);
        cleanupReservedEvents(newItem);
        
        exclusiveEvents.add(newItem);
        
        sendEvent(new UserEventUnavailableEvent(userEvents));
        
        return true;
    }

    public void removeUserEventListener(UserEventListener listener)
    {
        
        for (Iterator<UserEventItem> it = events.iterator(); it.hasNext(); ) {
            UserEventItem item = it.next();
            
            if (item.listener == listener)
                it.remove();
                
        }
        for (Iterator<UserEventItem> it = exclusiveEvents.iterator(); it.hasNext(); ) {
            UserEventItem item = it.next();
            
            if (item.listener == listener) {
                sendEvent(new UserEventAvailableEvent(item.userEvents));
                it.remove();
            }
        }
    }

    public void removeExclusiveAccessToAWTEvent(ResourceClient client)
    {
        for (Iterator<UserEventItem> it = exclusiveEvents.iterator(); it.hasNext(); ) {
            UserEventItem item = it.next();
            
            if (item.client == client) {
                sendEvent(new UserEventAvailableEvent(item.userEvents));
                it.remove();
            }
        }
    }

    public void addResourceStatusEventListener(ResourceStatusListener listener)
    {
        listeners.add(listener);
    }

    public void removeResourceStatusEventListener(
            ResourceStatusListener listener)
    {
        listeners.remove(listener);
    }
    
    public void receiveKeyEvent(int type, int keyCode)
    {
        // first go through exclusive events and return if one is found
        for (UserEventItem item : exclusiveEvents) {
            for (UserEvent evt : item.userEvents.getUserEvent()) {
                if (evt.getCode() == keyCode && evt.getType() == type) {
                    item.listener.userEventReceived(evt);
                    return;
                }
            }
        }
        
        // if there are no exclusive events go through shared events
        for (UserEventItem item : events) {
            for (UserEvent evt : item.userEvents.getUserEvent()) {
                if (evt.getCode() == keyCode && evt.getType() == type) {
                    item.listener.userEventReceived(evt);
                }
            }
        }
    }
    
    private UserEventItem checkAlreadyReserved(UserEventItem newItem) {
        for (UserEventItem item : exclusiveEvents) {
            if (newItem.context == item.context)
                continue;
            
            if (newItem.client == item.client)
                continue;
            
            if (hasOverlap(item, newItem))
                return item;
        }
        
        return null;
    }
    
    private void cleanupReservedEvents(UserEventItem newItem) 
    {
        UserEventItem oldItem = checkAlreadyReserved(newItem);
        if (oldItem != null){
            oldItem.client.release(oldItem.userEvents);
            exclusiveEvents.remove(oldItem);
        }
    }
    
    private boolean hasOverlap(UserEventItem item1, UserEventItem item2)
    {
        for (UserEvent ue1 : item1.userEvents.getUserEvent()) {
            for (UserEvent ue2 : item2.userEvents.getUserEvent()) {
                if (ue1.getFamily() != ue2.getFamily() || ue1.getCode() != ue2.getCode())
                    continue;
                else
                    return true;
            }
        }
        
        return false;
    }
    
    private void sendEvent(ResourceStatusEvent event)
    {
        for (ResourceStatusListener listener : listeners) {
            listener.statusChanged(event);
        }
    }
    
    private class UserEventItem
    {
        public UserEventItem(XletContext context, UserEventListener listener,
            ResourceClient client, UserEventRepository userEvents)
        {
            this.context = context;
            this.listener = listener;
            this.client = client;
            this.userEvents = userEvents;
        }
        
        public XletContext context;
        public UserEventListener listener;
        public ResourceClient client;
        public UserEventRepository userEvents;
    }
    

    private LinkedList<UserEventItem> exclusiveEvents = new LinkedList<UserEventItem>();
    private LinkedList<UserEventItem> events = new LinkedList<UserEventItem>();
    private LinkedList<ResourceStatusListener> listeners = new LinkedList<ResourceStatusListener>();
    
    private static EventManager instance = null;
    
}
