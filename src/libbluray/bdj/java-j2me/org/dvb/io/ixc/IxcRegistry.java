/*
 * This file is part of libbluray
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

package org.dvb.io.ixc;

import java.rmi.AccessException;
import java.rmi.RemoteException;
import java.rmi.NotBoundException;
import java.rmi.AlreadyBoundException;
import java.rmi.Remote;

import javax.microedition.xlet.ixc.StubException;
import javax.tv.xlet.XletContext;

import org.dvb.application.AppID;
import org.dvb.application.AppProxy;
import org.dvb.application.AppsDatabase;
import org.videolan.BDJXletContext;

public class IxcRegistry {
    private static javax.microedition.xlet.ixc.IxcRegistry getIxcRegistry(XletContext xc) {
        if (!(xc instanceof BDJXletContext))
            throw new IllegalArgumentException();
        javax.microedition.xlet.ixc.IxcRegistry registry;
        registry = javax.microedition.xlet.ixc.IxcRegistry.getRegistry((BDJXletContext)xc);
        if (registry == null)
            throw new IllegalArgumentException();
        return registry;
    }

    public static Remote lookup(XletContext xc, String path) throws NotBoundException, RemoteException {
        int orgid, appid;
        int s1, s2;
        if (path.charAt(0) != '/')
            throw new IllegalArgumentException();
        s1 = path.indexOf('/', 1);
        if (s1 <= 1)
            throw new IllegalArgumentException();
        try {
            orgid = Integer.parseInt(path.substring(1, s1), 16);
        } catch (Exception e) {
            throw new IllegalArgumentException();
        }
        s1++;
        s2 = path.indexOf('/', s1);
        if (s2 <= s1)
            throw new IllegalArgumentException();
        try {
            appid = Integer.parseInt(path.substring(s1, s2), 16);
        } catch (Exception e) {
            throw new IllegalArgumentException();
        }
        String key = "/" + Integer.toHexString(orgid) +
                     "/" + Integer.toHexString(appid) +
                     "/" + path.substring(s2 + 1, path.length());
        return getIxcRegistry(xc).lookup(key);
    }

    public static void bind(XletContext xc, String name, Remote obj) throws AlreadyBoundException {
        if ((xc == null) || (name == null) || (obj == null))
            throw new NullPointerException();
        String orgid = (String)xc.getXletProperty("dvb.org.id");
        String appid = (String)xc.getXletProperty("dvb.app.id");
        AppID id = new AppID(Integer.parseInt(orgid, 16), Integer.parseInt(appid, 16));
        if (AppsDatabase.getAppsDatabase().getAppProxy(id).getState() == AppProxy.DESTROYED)
            return;
        String key = "/" + orgid + "/" + appid + "/" + name;
        try {
            getIxcRegistry(xc).bind(key, obj);
        } catch (StubException e) {
            throw new IllegalArgumentException();
        }
    }

    public static void unbind(XletContext xc, String name) throws NotBoundException {
        if ((xc == null) || (name == null))
            throw new NullPointerException();
        String key = "/" + (String)xc.getXletProperty("dvb.org.id") +
                     "/" + (String)xc.getXletProperty("dvb.app.id") +
                     "/" + name;
        try {
            getIxcRegistry(xc).unbind(key);
        } catch (AccessException e) {
            throw new IllegalArgumentException();
        }
    }

    public static void rebind(XletContext xc, String name, Remote obj) {
        if ((xc == null) || (name == null) || (obj == null))
            throw new NullPointerException();
        String orgid = (String)xc.getXletProperty("dvb.org.id");
        String appid = (String)xc.getXletProperty("dvb.app.id");
        AppID id = new AppID(Integer.parseInt(orgid, 16), Integer.parseInt(appid, 16));
        if (AppsDatabase.getAppsDatabase().getAppProxy(id).getState() == AppProxy.DESTROYED)
            return;
        String key = "/" + orgid + "/" + appid + "/" + name;
        try {
            getIxcRegistry(xc).rebind(key, obj);
        } catch (StubException e) {
            throw new IllegalArgumentException();
        } catch (AccessException e) {
            throw new IllegalArgumentException();
        }
    }

    public static String[] list(XletContext xc) {
        return getIxcRegistry(xc).list();
    }
}
