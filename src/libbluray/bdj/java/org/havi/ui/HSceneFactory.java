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

package org.havi.ui;

import org.videolan.GUIManager;

public class HSceneFactory extends Object {
    private HSceneFactory()
    {

    }

    public static HSceneFactory getInstance()
    {
        return instance;
    }

    public HSceneTemplate getBestSceneTemplate(HSceneTemplate template)
    {
        throw new Error("Not implemented");
    }

    public HScene getBestScene(HSceneTemplate template)
    {
        throw new Error("Not implemented");
    }

    public HSceneTemplate resizeScene(HScene scene, HSceneTemplate template)
            throws IllegalStateException
    {
        throw new Error("Not implemented");
    }

    public HScene getDefaultHScene(HScreen screen)
    {
        synchronized(defaultHSceneLock) {
            if (defaultHScene == null) {
                defaultHScene = new HScene();
                GUIManager.getInstance().add(defaultHScene);
            }
        }
        
        return defaultHScene;
    }

    public HScene getDefaultHScene()
    {
        synchronized(defaultHSceneLock) {
            if (defaultHScene == null) {
                defaultHScene = new HScene();
                GUIManager.getInstance().add(defaultHScene);
            }
        }
        
        return defaultHScene;
    }

    public HScene getFullScreenScene(HGraphicsDevice device)
    {
        throw new Error("Not implemented");
    }

    public void dispose(HScene scene)
    {
        throw new Error("Not implemented");
    }

    private HScene defaultHScene = null;
    private Object defaultHSceneLock = new Object();
    private static final HSceneFactory instance = new HSceneFactory();
}
