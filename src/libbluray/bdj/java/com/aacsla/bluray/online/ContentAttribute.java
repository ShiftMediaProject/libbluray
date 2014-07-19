/*
 * This file is part of libbluray
 * Copyright (C) 2012  libbluray
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

package com.aacsla.bluray.online;

import java.io.File;
import java.io.FileInputStream;

public class ContentAttribute {
    public ContentAttribute() {
    }

    public byte[] getContentCertID() {
        try {
            FileInputStream is = new FileInputStream(
                System.getProperty("bluray.vfs.root") + File.separator + "AACS/Content000.cer");
            is.skip(14);
            byte[] bytes = new byte[6];
            is.read(bytes, 0, 6);
            is.close();
            return bytes;
        } catch (Throwable e) {
            e.printStackTrace();
            return null;
        }
    }
}
