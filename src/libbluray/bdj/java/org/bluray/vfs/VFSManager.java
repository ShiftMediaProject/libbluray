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

package org.bluray.vfs;

import org.videolan.Logger;

public class VFSManager {
    public static VFSManager getInstance() throws SecurityException,
            UnsupportedOperationException
    {
        logger.unimplemented("getInstance");
        return new VFSManager();
        //throw new UnsupportedOperationException();
    }

    protected VFSManager()
    {
        state = PREPARING;
        logger.unimplemented("VFSManager");
    }

    public boolean disableClip(String streamfile)
    {
        logger.unimplemented("disableClip");
        return true;
    }

    public boolean enableClip(String streamfile)
    {
        logger.unimplemented("enableClip");
        return true;
    }

    public String[] getDisabledClipIDs()
    {
        logger.unimplemented("getDisabledClipIDs");
        return new String[]{};
    }

    public int getState()
    {
        logger.unimplemented("getState");
        return STABLE;
    }

    public boolean isEnabledClip(String clipID)
    {
        logger.unimplemented("isEnabledClip");
        return true;
    }

    public void requestUpdating(String manifestfile, String signaturefile,
            boolean initBackupRegs) throws PreparingFailedException
    {
        logger.unimplemented("requestUpdating(" + manifestfile + ")");
    }

    private int state;

    public static final int STABLE = 1;
    public static final int PREPARING = 2;
    public static final int PREPARED = 3;
    public static final int UPDATING = 4;

    private static final Logger logger = Logger.getLogger(VFSManager.class.getName());
}
