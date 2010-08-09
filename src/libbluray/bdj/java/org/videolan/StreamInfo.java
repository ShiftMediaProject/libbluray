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
    
    public byte getCoding_type()
    {
        return coding_type;
    }
    
    public byte getFormat()
    {
        return format;
    }
    
    public byte getRate()
    {
        return rate;
    }
    
    public char getChar_code()
    {
        return char_code;
    }
    
    public String getLang()
    {
        return lang;
    }
    
    public short getPid()
    {
        return pid;
    }

    byte coding_type;
    byte format;
    byte rate;
    char char_code;
    String lang;
    short pid;
}
