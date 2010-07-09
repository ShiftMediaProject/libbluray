package org.videolan.bdjo;


public class AppCache {
    public AppCache(int type, String refToName, String language)
    {
        this.type = AppCacheType.fromId(type);
        this.refToName = refToName;
        this.language = language;
    }

    public AppCacheType getType()
    {
        return type;
    }
    
    public String getRefToName()
    {
        return refToName;
    }
    
    public String getLanguage()
    {
        return language;
    }
    
    public String toString()
    {
        return "AppCache [language=" + language + ", refToName=" + refToName
                + ", type=" + type + "]";
    }
    
    private AppCacheType type;
    private String refToName;
    private String language;
}
