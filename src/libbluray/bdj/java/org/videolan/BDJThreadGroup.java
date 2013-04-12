/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 * Copyright (C) 2013  Petri Hintukainen <phintuka@users.sourceforge.net>
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

public class BDJThreadGroup extends ThreadGroup {

    public BDJThreadGroup(String name, BDJXletContext context) {
        super(name);
        this.context = context;
    }

    public BDJXletContext getContext() {
        return context;
    }

    public void setContext(BDJXletContext context) {
        this.context = context;
    }

    public boolean waitForShutdown(int maxThreads, int timeout) {
        long startTime = System.currentTimeMillis();
        long endTime = startTime + 1000;
        while ((activeCount() > maxThreads) &&
               (System.currentTimeMillis() < endTime)) {
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) { }
        }

        boolean result = (activeCount() <= maxThreads);
        if (!result) {
            logger.error("waitForShutdown timeout");
        }
        return result;
    }

    protected void stopAll(int timeout) {

        interrupt();
        waitForShutdown(0, timeout);

        if (activeCount() > 0) {
            logger.error("stopAll(): killing threads");
            dumpThreads();

            PortingHelper.stopThreadGroup(this);
            waitForShutdown(0, 500);
        }

        try {
            destroy();
        } catch (IllegalThreadStateException e) {
            logger.error("ThreadGroup destroy failed: " + e);
        }

        context = null;
    }

    public void dumpThreads() {
        logger.info("Active threads in " + this + ":");
        Thread[] threads = new Thread[activeCount() + 1];
        while (enumerate( threads, true ) == threads.length) {
            threads = new Thread[threads.length * 2];
        }
        for (int i = 0; i < threads.length; i++) {
            if (threads[i] == null)
                continue;
            logger.info("    " + threads[i]);
        }
    }

    private BDJXletContext context;
    private static final Logger logger = Logger.getLogger(BDJThreadGroup.class.getName());
}
