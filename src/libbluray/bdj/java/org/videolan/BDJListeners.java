/*
 * This file is part of libbluray
 * Copyright (C) 2013  VideoLAN
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

import java.util.Iterator;
import java.util.LinkedList;

import javax.media.ControllerEvent;
import javax.media.ControllerListener;

import javax.tv.service.selection.ServiceContextEvent;
import javax.tv.service.selection.ServiceContextListener;

import org.bluray.bdplus.StatusListener;

import org.bluray.media.AngleChangeEvent;
import org.bluray.media.AngleChangeListener;
import org.bluray.media.PiPStatusEvent;
import org.bluray.media.PiPStatusListener;
import org.bluray.media.PlaybackListener;
import org.bluray.media.PlaybackMarkEvent;
import org.bluray.media.PlaybackPlayItemEvent;
import org.bluray.media.UOMaskTableListener;
import org.bluray.media.UOMaskTableChangedEvent;
import org.bluray.media.UOMaskedEvent;

import org.dvb.media.SubtitleListener;
import java.util.EventObject;


public class BDJListeners {
    private LinkedList listeners = new LinkedList();

    public void add(Object listener) {
        if (listener != null) {
            BDJXletContext ctx = BDJXletContext.getCurrentContext();
            if (ctx == null) {
                logger.error("Listener added from wrong thread: " + Logger.dumpStack());
                return;
            }
            synchronized (listeners) {
                remove(listener);
                listeners.add(new BDJListener(ctx, listener));
            }
        }
    }

    public void remove(Object listener) {
        synchronized (listeners) {
            for (Iterator it = listeners.iterator(); it.hasNext(); ) {
                BDJListener item = (BDJListener)it.next();
                if (item.listener == listener)
                    it.remove();
            }
        }
    }

    public void putCallback(Object event) {
        synchronized (listeners) {
            for (Iterator it = listeners.iterator(); it.hasNext(); ) {
                BDJListener item = (BDJListener)it.next();
                if (item.ctx == null) {
                    logger.error("Listener callback: no context: " + item.listener);
                    it.remove();
                } else if (item.ctx.isDestroyed()) {
                    logger.error("Listener terminated: " + item.ctx);
                    it.remove();
                } else {
                    item.ctx.putCallback(new Callback(event, item.listener));
                }
            }
        }
    }

    private class PSR102Status {
        private PSR102Status(int value) {
            this.value = value;
        }
        public int value;
    }

    public void putPSR102Callback(int value) {
        putCallback(new PSR102Status(value));
    }

    private class BDJListener {
        public BDJXletContext ctx;
        public Object listener;

        BDJListener(BDJXletContext ctx, Object listener) {
            this.ctx = ctx;
            this.listener = listener;
        }
    }

    private class Callback extends BDJAction {
        private Callback(Object event, Object listener) {
            this.event = event;
            this.listener = listener;
        }

        protected void doAction() {
            if (event instanceof PlaybackMarkEvent) {
                ((PlaybackListener)listener).markReached((PlaybackMarkEvent)event);
            } else if (event instanceof PlaybackPlayItemEvent) {
                ((PlaybackListener)listener).playItemReached((PlaybackPlayItemEvent)event);
            } else if (event instanceof ServiceContextEvent) {
                ((ServiceContextListener)listener).receiveServiceContextEvent((ServiceContextEvent)event);
            } else if (event instanceof UOMaskTableChangedEvent) {
                ((UOMaskTableListener)listener).receiveUOMaskTableChangedEvent((UOMaskTableChangedEvent)event);
            } else if (event instanceof UOMaskedEvent) {
                ((UOMaskTableListener)listener).receiveUOMaskedEvent((UOMaskedEvent)event);
            } else if (event instanceof PiPStatusEvent) {
                ((PiPStatusListener)listener).piPStatusChange((PiPStatusEvent)event);
            } else if (event instanceof AngleChangeEvent) {
                ((AngleChangeListener)listener).angleChange((AngleChangeEvent)event);

            } else if (event instanceof ControllerEvent) {
                ((ControllerListener)listener).controllerUpdate((ControllerEvent)event);

            /* need to use wrapper if some other callback uses EventObject */
            } else if (event instanceof EventObject &&
                       listener instanceof SubtitleListener) {
                ((SubtitleListener)listener).subtitleStatusChanged((EventObject)event);

            } else if (event instanceof PSR102Status) {
                ((StatusListener)listener).receive(((PSR102Status)event).value);

            } else {
                System.err.println("Unknown event type: " + event.getClass().getName());
            }
        }

        private Object listener;
        private Object event;
    }

    private static final Logger logger = Logger.getLogger(BDJListeners.class.getName());
}
