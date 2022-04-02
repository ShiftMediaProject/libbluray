/*
 * This file is part of libbluray
 * Copyright (C) 2021  VideoLAN
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

package sun.awt;

/* This class replaces java.awt.graphicsenv property in Java 13+ */

import java.awt.GraphicsEnvironment;
import java.awt.Toolkit;

public class PlatformGraphicsInfo {

    public static GraphicsEnvironment createGE() {
        return new java.awt.BDGraphicsEnvironment();
    }

    public static Toolkit createToolkit() {
        return new java.awt.BDToolkit();
    }

    public static boolean getDefaultHeadlessProperty() {
        return false;
    }

    public static String getDefaultHeadlessMessage() {
        org.videolan.Logger.getLogger(PlatformGraphicsInfo.class.getName())
            .error("Headless graphics environment ?\n" + org.videolan.Logger.dumpStack());
        return "Internal error: headless graphics environment ?";
    }
}
