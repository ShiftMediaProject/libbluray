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

package org.havi.ui;

import java.awt.Color;
import java.awt.Font;

import org.videolan.Logger;

public class HStaticText extends HVisible implements HNoInputPreferred {
    public HStaticText()
    {
        logger.unimplemented();
    }

    public HStaticText(String textNormal, int x, int y, int width, int height)
    {
        logger.info("HStaticText " + textNormal);
        logger.unimplemented();
    }

    public HStaticText(String textNormal, int x, int y, int width, int height,
            Font font, Color foreground, Color background,
            HTextLayoutManager tlm)
    {
        logger.info("HStaticText " + textNormal);
        logger.unimplemented();
    }

    public HStaticText(String textNormal)
    {
        logger.info("HStaticText " + textNormal);
        logger.unimplemented();
    }

    public HStaticText(String textNormal, Font font, Color foreground,
            Color background, HTextLayoutManager tlm)
    {
        logger.info("HStaticText " + textNormal);
        logger.unimplemented();
    }

    public void setLook(HLook hlook) throws HInvalidLookException
    {
        logger.unimplemented("setLook");
    }

    public static void setDefaultLook(HTextLook hlook)
    {
        DefaultLook = hlook;
    }

    public static HTextLook getDefaultLook()
    {
        if (DefaultLook == null)
            org.videolan.Logger.unimplemented("", "getDefaultLook");
        return DefaultLook;
    }

    private static HTextLook DefaultLook = null;

    private static final Logger logger = Logger.getLogger(HStaticText.class.getName());

    private static final long serialVersionUID = 4352450387189482885L;
}
