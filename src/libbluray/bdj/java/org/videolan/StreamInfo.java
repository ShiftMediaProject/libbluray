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

public class StreamInfo {
    public StreamInfo(byte coding_type, byte format, byte rate, char char_code, String lang, short pid)
    {
        this.coding_type = coding_type;
        this.format = format;
        this.rate = rate;
        this.char_code = char_code;
        this.lang = lang;
        this.pid = pid;
    }

    public byte getCoding_type() {
        return coding_type;
    }

    public byte getFormat() {
        return format;
    }

    public byte getRate() {
        return rate;
    }

    public char getChar_code() {
        return char_code;
    }

    public String getLang() {
        return lang;
    }

    public short getPid() {
        return pid;
    }

    byte coding_type;
    byte format;
    byte rate;
    char char_code;
    String lang;
    short pid;
}
