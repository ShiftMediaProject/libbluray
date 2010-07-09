package org.videolan.bdjo;

public class AppName {
    public AppName(String language, String name) {
        this.language = language;
        this.name = name;
    }

    public String getLanguage() 
    {
        return language;
    }
    
    public String getName()
    {
        return name;
    }
    
    public String toString()
    {
        return "AppName [language=" + language + ", name=" + name + "]";
    }
    
    private String language;
    private String name;
}
