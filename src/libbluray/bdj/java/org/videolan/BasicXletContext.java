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

public class BasicXletContext implements javax.tv.xlet.XletContext {
    
    protected BasicXletContext(String[] params, long nativePointer)
    {
        this.params = params;
        this.nativePointer = nativePointer;
    }
    
    public void notifyDestroyed()
    {
        
    }

    public void notifyPaused()
    {
        
    }

    public Object getXletProperty(String key)
    {
        if (key.equals(ARGS))
            return params;
        else {
            log.warning("[bdj] BasicXletContext: Unknown property requested: " + key);
            return null;
        }
    }

    public void resumeRequest()
    {
        
    }
    
    protected long getNativePointer()
    {
        return nativePointer;
    }
    
    protected static BasicXletContext instance = null;
    
    private String[] params;
    private long nativePointer;
    private static Logger log = Logger.getLogger(BasicXletContext.class.getName());
}
