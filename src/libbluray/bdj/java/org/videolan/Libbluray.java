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

public class Libbluray {
    public static int getCurrentTitleNum() {
        return getCurrentTitleNumN(nativePointer);
    }
    
    public static TitleInfo getTitleInfo(int titleNum)
    {
        return new TitleInfo(titleNum);
    }
    
    public static void writeGPR(int num, int value)
    {
        int ret = writeGPRN(nativePointer, num, value);
        
        if (ret == -1)
            throw new IllegalArgumentException("Invalid GPR");
    }
    
    public static int readGPR(int num)
    {
        int value = readGPRN(nativePointer, num);
        
        if (value == -1)
            throw new IllegalArgumentException("Invalid GPR");
        
        return value;
    }
    
    public static int readPSR(int num)
    {
        int value = readPSRN(nativePointer, num);

        if (value == -1)
            throw new IllegalArgumentException("Invalid PSR");
        
        return value;
    }
    
    protected static long nativePointer = 0; 
    
    private static native int getCurrentTitleNumN(long pBDJ);
    private static native int writeGPRN(long pBDJ, int num, int value);
    private static native int readGPRN(long pBDJ, int num);
    private static native int readPSRN(long pBDJ, int num);
}
