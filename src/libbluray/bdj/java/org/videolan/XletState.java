package org.videolan;

public enum XletState
{
    ACTIVE, // this is just the state the xlet is in if it is not paused or destroyed
    PAUSED,
    DESTROYED;
}
