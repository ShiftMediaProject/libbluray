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

public class TIClip {
    public TIClip(StreamInfo[] videoStreams, StreamInfo[] audioStreams, StreamInfo[] pgStreams,
            StreamInfo[] igStreams, StreamInfo[] secVideoStreams, StreamInfo[] secAudioStreams) {
        this.videoStreams = videoStreams;
        this.audioStreams = audioStreams;
        this.pgStreams = pgStreams;
        this.igStreams = igStreams;
        this.secVideoStreams = secVideoStreams;
        this.secAudioStreams = secAudioStreams;
    }

    public StreamInfo[] getVideoStreams() {
        return videoStreams;
    }

    public StreamInfo[] getAudioStreams() {
        return audioStreams;
    }

    public StreamInfo[] getPgStreams() {
        return pgStreams;
    }

    public StreamInfo[] getIgStreams() {
        return igStreams;
    }

    public StreamInfo[] getSecVideoStreams() {
        return secVideoStreams;
    }

    public StreamInfo[] getSecAudioStreams() {
        return secAudioStreams;
    }

    private StreamInfo[] videoStreams = null;
    private StreamInfo[] audioStreams = null;
    private StreamInfo[] pgStreams = null;
    private StreamInfo[] igStreams = null;
    private StreamInfo[] secVideoStreams = null;
    private StreamInfo[] secAudioStreams = null;
}
