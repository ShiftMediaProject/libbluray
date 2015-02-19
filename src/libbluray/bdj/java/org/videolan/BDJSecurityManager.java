/*
 * This file is part of libbluray
 * Copyright (C) 2015  Petri Hintukainen <phintuka@users.sourceforge.net>
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

import java.io.FilePermission;
import java.security.Permission;

/*
 * Dummy security manager to grab all file access
 */
class BDJSecurityManager extends SecurityManager {
    public void checkPermission(Permission perm) {
        /*
        try {
            java.security.AccessController.checkPermission(perm);
        } catch (java.security.AccessControlException ex) {
            System.err.println(" *** caught " + ex + " at " + Logger.dumpStack());
            throw ex;
        }
        */
    }

    public void checkRead(String file) {
        //super.checkRead(file);
        BDJLoader.accessFile(file);
    }
}
