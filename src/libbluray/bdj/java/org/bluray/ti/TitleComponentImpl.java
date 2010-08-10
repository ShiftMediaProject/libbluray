package org.bluray.ti;

import java.util.Date;

import javax.tv.locator.Locator;
import javax.tv.service.Service;
import javax.tv.service.ServiceInformationType;
import javax.tv.service.navigation.StreamType;

import org.bluray.net.BDLocator;
import org.davic.net.InvalidLocatorException;
import org.videolan.StreamInfo;

public class TitleComponentImpl implements TitleComponent {
    protected TitleComponentImpl(StreamInfo stream, StreamType type, boolean primary, int playlistId, int playitemId, Title service)
    {
        this.stream = stream;
        this.type = type;
        this.primary = primary;
        this.playlistId = playlistId;
        this.playitemId = playitemId;
        this.service = service;
    }
    
    public String getName()
    {
        BDLocator l = (BDLocator) getLocator();
        if (l == null)
            return null;
        return l.toString();
    }

    public String getAssociatedLanguage()
    {
        return stream.getLang();
    }

    public StreamType getStreamType()
    {
        return type;
    }

    public Service getService()
    {
        return service;
    }

    public Locator getLocator()
    {
        StringBuilder str = new StringBuilder();
        str.append("bd://");
        str.append(((TitleImpl)service).getTitleNum());
        str.append(".PLAYLIST:");
        str.append(playlistId);
        str.append(".ITEM:");
        str.append(playitemId);
        
        if (type.equals(StreamType.AUDIO) && primary)
            str.append(".A1:" + stream.getPid());
        else if (type.equals(StreamType.VIDEO) && primary)
            str.append(".V1:" + stream.getPid());
        else if (type.equals(StreamType.AUDIO) && !primary)
            str.append(".A2:" + stream.getPid());
        else if (type.equals(StreamType.VIDEO) && !primary)
            str.append(".V2:" + stream.getPid());
        else if (type.equals(StreamType.SUBTITLES) && primary)
            str.append(".P:" + stream.getPid());
        else
            return null;
        
        try {
            return new BDLocator(str.toString());
        } catch (InvalidLocatorException e) {
            return null;
        }
    }

    public ServiceInformationType getServiceInformationType()
    {
        return TitleInformationType.BD_ROM;
    }

    public Date getUpdateTime()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    public CodingType getCodingType()
    {
        switch (stream.getCoding_type()) {
        case (byte)0x02:
            return CodingType.MPEG2_VIDEO;
        case (byte)0x1b:
            return CodingType.MPEG4_AVC_VIDEO;
        case (byte)0xea:
            return CodingType.SMPTE_VC1_VIDEO;
        case (byte)0x80:
            return CodingType.LPCM_AUDIO;
        case (byte)0x81:
            return CodingType.DOLBY_AC3_AUDIO;
        case (byte)0x82:
            return CodingType.DTS_AUDIO;
        case (byte)0x83:
            return CodingType.DOLBY_LOSSLESS_AUDIO;
        case (byte)0x84:
        case (byte)0xA1:
            return CodingType.DOLBY_DIGITAL_PLUS_AUDIO;
        case (byte)0x85:
            return CodingType.DTS_HD_AUDIO_EXCEPT_XLL;
        case (byte)0x86:
            return CodingType.DTS_HD_AUDIO_XLL;
        case (byte)0xA2:
            return CodingType.DTS_HD_AUDIO;
        case (byte)0x90:
            return CodingType.PRESENTATION_GRAPHICS;
        case (byte)0x91:
            return CodingType.INTERACTIVE_GRAPHICS;
        case (byte)0x92:
            return CodingType.TEXT_SUBTITLE;
        default:
            return null;
        }
    }

    public int getStreamNumber()
    {
        return stream.getPid();
    }

    public int getSubPathId()
    {
        throw new Error("Not implemented"); // TODO implement
    }

    StreamInfo stream;
    StreamType type;
    boolean primary;
    int playlistId;
    int playitemId;
    Title service;
}
