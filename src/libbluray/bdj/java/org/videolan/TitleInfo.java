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

public class TitleInfo {
    public TitleInfo(int index, int playlist, long duration, int angles, TIChapter[] chapters, TIClip[] clips)
    {
        this.index = index;
        this.playlist = playlist;
        this.duration = duration;
        this.angles = angles;
        this.chapters = chapters;
        this.clips = clips;
    }
    
    public int getIndex()
    {
        return index;
    }

    public int getPlaylist()
    {
        return playlist;
    }

    public long getDuration()
    {
        return duration;
    }

    public TIChapter[] getChapters()
    {
        return chapters;
    }
    
    public TIClip[] getClips()
    {
        return clips;
    }
    
    public int getAngleCount()
    {
        return angles;
    }

    private int index;
    private int playlist;
    private long duration;
    private int angles;
    private TIChapter[] chapters;
    private TIClip[] clips;
    
}
