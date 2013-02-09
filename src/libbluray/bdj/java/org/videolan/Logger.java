/*
 * This file is part of libbluray
 * Copyright (C) 2012  Petri Hintukainen <phintuka@users.sourceforge.net>
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

public class Logger {
    static {
        String prop;

        use_throw = false;
        use_trace = false;

        prop = System.getProperty("debug.unimplemented.throw");
        if (prop != null && prop.equalsIgnoreCase("YES")) {
            use_throw = true;
        }
        prop = System.getProperty("debug.trace");
        if (prop == null || !prop.equalsIgnoreCase("NO")) {
            use_trace = true;
        }
    }

    public static Logger getLogger(String name) {
        return new Logger(name);
    }

    private Logger(String name) {
        this.name = name;
    }

    private static void log(String msg) {
        System.out.println(msg);
    }

    public void trace(String msg) {
        if (use_trace) {
            log("BD-J: " + name + ": " + msg);
        }
    }

    public void info(String msg) {
        log("BD-J: " + name + ": " + msg);
    }

    public void warning(String msg) {
        log("BD-J WARNING: " + name + ": " + msg);
    }

    public void error(String msg) {
        log("BD-J ERROR: " + name + ": " + msg);
    }

    public void unimplemented() {
        unimplemented(null);
    }

    public static void dumpStack() {
        StackTraceElement e[] = new Exception("Stack trace").getStackTrace();
        for (int i = 2; i < e.length; i++)
            log("    " + e[i].toString());
    }

    public void unimplemented(String func) {
        String location = name;
        if (func != null) {
            location = location + "." + func + "()";
            log("BD-J: Not implemented: " + location);
        }
        dumpStack();

        if (use_throw) {
            throw new Error("Not implemented: " + location);
        }
    }

    public static void unimplemented(String cls, String func) {
        String location = cls + "." + func + "()";
        log("BD-J: Not implemented: " + location);

        dumpStack();

        if (use_throw) {
            throw new Error("Not implemented: " + location);
        }
    }

    private String name;
    private static boolean use_trace;
    private static boolean use_throw;
}
