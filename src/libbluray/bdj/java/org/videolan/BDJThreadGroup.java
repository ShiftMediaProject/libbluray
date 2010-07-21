package org.videolan;

import javax.tv.xlet.XletContext;

public class BDJThreadGroup extends ThreadGroup {

    public BDJThreadGroup(String name, XletContext context)
    {
        super(name);
        this.context = context;
    }

    public XletContext getContext()
    {
        return context;
    }
    
    private XletContext context;
}
