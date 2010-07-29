package org.videolan;

public class TIClip {
    public TIClip(StreamInfo[] videoStreams, StreamInfo[] audioStreams, StreamInfo[] pgStreams,
            StreamInfo[] igStreams, StreamInfo[] secVideoStreams, StreamInfo[] secAudioStreams)
    {
        this.videoStreams = videoStreams;
        this.audioStreams = audioStreams;
        this.pgStreams = pgStreams;
        this.igStreams = igStreams;
        this.secVideoStreams = secVideoStreams;
        this.secAudioStreams = secAudioStreams;
    }
    
    public StreamInfo[] getVideoStreams()
    {
        return videoStreams;
    }
    
    public StreamInfo[] getAudioStreams()
    {
        return audioStreams;
    }
    
    public StreamInfo[] getPgStreams()
    {
        return pgStreams;
    }
    
    public StreamInfo[] getIgStreams()
    {
        return igStreams;
    }
    
    public StreamInfo[] getSecVideoStreams()
    {
        return secVideoStreams;
    }
    
    public StreamInfo[] getSecAudioStreams()
    {
        return secAudioStreams;
    }

    private StreamInfo[] videoStreams = null;
    private StreamInfo[] audioStreams = null;
    private StreamInfo[] pgStreams = null;
    private StreamInfo[] igStreams = null;
    private StreamInfo[] secVideoStreams = null;
    private StreamInfo[] secAudioStreams = null;
}
