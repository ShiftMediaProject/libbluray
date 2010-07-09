package org.videolan.bdjo;

import java.util.Arrays;

import org.davic.net.Locator;

public class AppEntry {
    public AppEntry(int controlCode, int type, int orgId,
            short appId, AppProfile[] profiles, short priority,
            int binding, int visibility, AppName[] names,
            String iconLocator, short iconFlags, String basePath,
            String classpathExt, String initialClass, String[] params)
    {
        this.controlCode = ControlCode.fromId(controlCode);
        this.type = AppType.fromId(type);
        this.orgId = orgId;
        this.appId = appId;

        this.profiles = profiles;
        this.priority = priority;
        this.binding = AppBinding.fromId(binding);
        this.visibility = AppVisibility.fromId(visibility);
        this.names = names;
        this.iconLocator = new Locator(iconLocator);
        this.iconFlags = iconFlags;
        this.basePath = basePath;
        this.classpathExt = classpathExt;
        this.initialClass = initialClass;
        this.params = params;
    }

    public ControlCode getControlCode()
    {
        return controlCode;
    }
    
    public AppType getType()
    {
        return type;
    }
    
    public int getOrgId()
    {
        return orgId;
    }
    
    public short getAppId()
    {
        return appId;
    }
    
    public AppProfile[] getProfiles()
    {
        return profiles;
    }
    
    public short getPriority()
    {
        return priority;
    }
    
    public AppBinding getBinding()
    {
        return binding;
    }
    
    public AppVisibility getVisibility()
    {
        return visibility;
    }
    
    public AppName[] getNames()
    {
        return names;
    }
    
    public Locator getIconLocator()
    {
        return iconLocator;
    }
    
    public short getIconFlags()
    {
        return iconFlags;
    }
    
    public String getBasePath()
    {
        return basePath;
    }
    
    public String getClasspathExt()
    {
        return classpathExt;
    }
    
    public String getInitialClass()
    {
        return initialClass;
    }
    
    public String[] getParams()
    {
        return params;
    }

    public String toString()
    {
        return "AppEntry [appId=" + appId + ", basePath=" + basePath
                + ", binding=" + binding + ", classpathExt=" + classpathExt
                + ", controlCode=" + controlCode + ", iconFlags=" + iconFlags
                + ", iconLocator=" + iconLocator + ", initialClass="
                + initialClass + ", names=" + Arrays.toString(names)
                + ", orgId=" + orgId + ", params=" + Arrays.toString(params)
                + ", priority=" + priority + ", profiles="
                + Arrays.toString(profiles) + ", type=" + type
                + ", visibility=" + visibility + "]";
    }

    private ControlCode controlCode;
    private AppType type;
    private int orgId;
    private short appId;

    private AppProfile[] profiles;
    private short priority;
    private AppBinding binding;
    private AppVisibility visibility;
    private AppName[] names;
    private Locator iconLocator;
    private short iconFlags;
    private String basePath;
    private String classpathExt;
    private String initialClass;
    private String[] params;
}
