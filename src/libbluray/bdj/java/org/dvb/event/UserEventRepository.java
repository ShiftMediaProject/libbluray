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
import java.awt.event.KeyEvent;

public class UserEventRepository extends RepositoryDescriptor {

    public UserEventRepository(String name)
    {
        super(null, name);
        throw new Error("Not implemented");
    }

    public void addUserEvent(UserEvent event)
    {
        throw new Error("Not implemented");
    }

    public UserEvent[] getUserEvent()
    {
        throw new Error("Not implemented");
    }

    public void removeUserEvent(UserEvent event)
    {
        events.remove(event);
    }

    public void addKey(int keycode)
    {
        events.add(new UserEvent(this, UserEvent.UEF_KEY_EVENT,
                KeyEvent.KEY_PRESSED, keycode, 0, 0));
        events.add(new UserEvent(this, UserEvent.UEF_KEY_EVENT,
                KeyEvent.KEY_RELEASED, keycode, 0, 0));
    }

    public void removeKey(int keycode)
    {
        for(Iterator<UserEvent> it = events.iterator(); it.hasNext() == true; ) {
            UserEvent event = it.next();
            
            if (event.getCode() == keycode)
                it.remove();
        }
    }

    public void addAllNumericKeys()
    {
        throw new Error("Not implemented");
    }

    public void addAllColourKeys()
    {
        throw new Error("Not implemented");
    }

    public void addAllArrowKeys()
    {
        throw new Error("Not implemented");
    }

    public void removeAllNumericKeys()
    {
        throw new Error("Not implemented");
    }

    public void removeAllColourKeys()
    {
        throw new Error("Not implemented");
    }

    public void removeAllArrowKeys()
    {
        throw new Error("Not implemented");
    }

    public String getName()
    {
        throw new Error("Not implemented");
    }

    private LinkedList<UserEvent> events = new LinkedList<UserEvent>();
}
