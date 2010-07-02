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

import javax.tv.xlet.*;

public class BDJLoader {
    // this is for testing
    public static void main(String[] args)
    {
        Load(args[0], null);
    }
    
    public static void Load(String init_class, String[] params)
    {
        try { 
        	ClassLoader classLoader = BDJLoader.class.getClassLoader();
            Class<?> xlet_class = classLoader.loadClass(init_class);
            System.out.println("[bdj] Loaded class: " + init_class);

            xlet = (Xlet)xlet_class.newInstance();

            // make context for xlet
            XletContext context = new BasicXletContext(params);
            
            System.out.println("[bdj] Attempting to start xlet");

            xlet.initXlet(context);
            xlet.startXlet();
            
            System.out.println("[bdj] Started xlet");

        } catch (Throwable e) {
        	System.out.println("[bdj] Failed to start xlet");
            e.printStackTrace();
        }
    }

    public static void Shutdown()
    {
        try {
            if (xlet != null) {
                xlet.destroyXlet(true);
                xlet = null;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static Xlet xlet = null;
}
