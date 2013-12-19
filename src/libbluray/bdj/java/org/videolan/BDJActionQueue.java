/*
 * This file is part of libbluray
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

import java.util.LinkedList;

class BDJActionQueue implements Runnable {
    public BDJActionQueue() {
        this(null);
    }

    public BDJActionQueue(BDJThreadGroup threadGroup) {
        /* run all actions in given thread group / xlet context */
        thread = new Thread(threadGroup, this);
        thread.setDaemon(true);
        thread.start();
    }

    protected void finalize() throws Throwable {
        synchronized (actions) {
            actions.addLast(null);
            actions.notifyAll();
        }
        thread.join();
        super.finalize();
    }

    public void run() {
        while (true) {
            Object action;
            synchronized (actions) {
                while (actions.isEmpty()) {
                    try {
                        actions.wait();
                    } catch (InterruptedException e) {
                    }
                }
                action = actions.removeFirst();
            }
            if (action == null)
                return;
            try {
                ((BDJAction)action).process();
            } catch (Throwable e) {
                e.printStackTrace();
            }
        }
    }

    public void put(BDJAction action) {
        if (action != null) {
            synchronized (actions) {
                actions.addLast(action);
                actions.notifyAll();
            }
        }
    }

    private Thread thread;
    private LinkedList actions = new LinkedList();
}
