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
package org.videolan;

import java.util.logging.Logger;

import javax.tv.xlet.XletContext;

import org.videolan.bdjo.AppEntry;

public class BasicXletContext implements XletContext {
    
    protected BasicXletContext(AppEntry info)
    {
        this.info = info;
    }
    
    public void notifyDestroyed()
    {
        logger.info("Xlet " + info.getOrgId() + "/" + info.getAppId() + " has entered destroyed state.");
        state = XletState.DESTROYED;
    }

    public void notifyPaused()
    {
        logger.info("Xlet " + info.getOrgId() + "/" + info.getAppId() + " has entered paused state.");
        state = XletState.PAUSED;
    }

    public Object getXletProperty(String key)
    {
        if (key.equals(ARGS))
            return info.getParams();
        else if (key.equals("dvb.org.id"))
            return info.getOrgId();
        else if (key.equals("dvb.app.id"))
            return info.getAppId();
        else {
            logger.warning("Unknown property requested: " + key);
            return null;
        }
    }

    public void resumeRequest()
    {
        logger.info("Xlet " + info.getOrgId() + "/" + info.getAppId() + " has requested to be resumed.");
    }
    
    public XletState getState()
    {
        return state;
    }
    
    private AppEntry info;
    private XletState state = XletState.ACTIVE;
    private static Logger logger = Logger.getLogger(BasicXletContext.class.getName());
}
