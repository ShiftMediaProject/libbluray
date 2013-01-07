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

import java.io.IOException;
import java.net.URL;

import org.videolan.Logger;

public class HSound {
    public HSound() {
        logger.unimplemented("HSound");
    }

    public void load(String location) throws IOException, SecurityException {
        logger.unimplemented("load");
        logger.info("load("+location+")");
    }

    public void load(URL contents) throws IOException, SecurityException {
        logger.unimplemented("load");
        logger.info("load("+contents+")");
    }

    public void set(byte data[]) {
        logger.unimplemented("set");
    }

    public void play() {
        logger.unimplemented("play");
    }

    public void stop() {
        logger.unimplemented("stop");
    }

    public void loop() {
        //logger.unimplemented("loop");
    }

    public void dispose() {
        //logger.unimplemented("dispose");
    }

    private static final Logger logger = Logger.getLogger(HSound.class.getName());
}
