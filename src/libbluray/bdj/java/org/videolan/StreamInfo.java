package org.videolan;

public class StreamInfo {
    public StreamInfo(byte coding_type, byte format, byte rate, char char_code, String lang, short pid)
    {
        this.coding_type = coding_type;
        this.format = format;
        this.rate = rate;
        this.char_code = char_code;
        this.lang = lang;
        this.pid = pid;
    }
    
    byte coding_type;
    byte format;
    byte rate;
    char char_code;
    String lang;
    short pid;
}
