package org.videolan;

import java.util.EventListener;

public interface BDJInputListener extends EventListener {
    void receiveKeyEvent(int type, int keyCode);
}
