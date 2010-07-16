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
package org.dvb.lang;
/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders of libbluray
 * gives permission to link the code of its release of libbluray with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables.  You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL".  If you
 * modify this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to do
 * so, delete this exception statement from your version.
 */

import java.net.URL;

public abstract class DVBClassLoader extends java.security.SecureClassLoader {
    public DVBClassLoader(URL[] urls)
    {
        this.urls = urls;
        this.parent = null;
    }

    public DVBClassLoader(URL[] urls, ClassLoader parent)
    {
        this.urls = urls;
        this.parent = parent;
    }

    protected Class<?> findClass(String name) throws ClassNotFoundException
    {
        throw new Error("Not implemented");
    }

    public static DVBClassLoader newInstance(URL[] urls)
    {
        return new DVBClassLoaderImpl(urls);
    }

    public static DVBClassLoader newInstance(URL[] urls, ClassLoader parent)
    {
        return new DVBClassLoaderImpl(urls, parent);
    }

    private URL[] urls;
    private ClassLoader parent;
}
