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

package javax.tv.service.selection;

import javax.tv.xlet.XletContext;

import org.bluray.ti.selection.TitleContext;
import org.bluray.ti.selection.TitleContextImpl;

public class ServiceContextFactoryImpl extends ServiceContextFactory {
    public ServiceContext createServiceContext()
            throws InsufficientResourcesException, SecurityException
    {
        throw new Error("Not implemented");
    }
    
    public ServiceContext getServiceContext(XletContext context)
            throws SecurityException, ServiceContextException
    {
        SecurityManager sec = System.getSecurityManager();
        if (sec != null)
            sec.checkPermission(new ServiceContextPermission("access", "own"));
        
        return serviceContext;
    }

    public ServiceContext[] getServiceContexts()
    {
        SecurityManager sec = System.getSecurityManager();
        if (sec != null)
            sec.checkPermission(new ServiceContextPermission("access", "own"));
        
        ServiceContext[] contexts = new ServiceContext[1];
        contexts[0] = serviceContext;
        return contexts;
    }

    private TitleContext serviceContext = new TitleContextImpl();
}
