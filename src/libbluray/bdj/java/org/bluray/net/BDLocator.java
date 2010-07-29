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

package org.bluray.net;

import org.davic.net.Locator;
import org.davic.net.InvalidLocatorException;
import org.videolan.BDJUtil;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.InputMismatchException;
import java.util.Scanner;
import java.util.logging.Logger;

public class BDLocator extends Locator {

    public BDLocator(String url) throws InvalidLocatorException
    {
        super(url);
        
        logger.info("Parsing locator " + url);

        if (!url.startsWith("bd://"))
            throw new InvalidLocatorException("Locator must start with bd:// (" + url + ")");

        Scanner scan = new Scanner(url.substring(5));

        scan.useDelimiter("[\\.:&]");

        try {
            while (scan.hasNext()) {
                String name = scan.next();
                
                if (scan.hasNext()) {
                    if (name.equals("JAR")) {
                        scan.useDelimiter("[:]");
                        String temp = scan.next();
                        jar = Integer.parseInt(temp.substring(0, 5));
                        isJarItem = true;
                        
                        if (temp.length() > 5)
                            pathSegments = temp.substring(5);
                    } else if (name.equals("SOUND")) {
                        sound = scan.nextInt(16);
                        isSoundItem = true;
                    } else if (name.equals("PLAYLIST")) {
                        playList = scan.nextInt();
                        isPlayItem = true;
                        
                        parsePlaylist(scan);
                    } else if (name.length() == 32)
                        disc = name;
                    else if (name.length() <= 4)
                        titleNum = Integer.parseInt(name, 16);
                    else
                        throw new InvalidLocatorException("Invalid identifier (" + url + ")");

                } else {
                    throw new InvalidLocatorException("Missing value (" + url + ")");
                }
            }
        } catch (InputMismatchException ex) {
            throw new InvalidLocatorException("Failed to parse value (" + url + ")");
        } catch (NumberFormatException ex) {
            throw new InvalidLocatorException("Failed to parse value (" + url + ")");
        }

        
    }

    public BDLocator(String disc, int titleNum, int playList) throws InvalidLocatorException
    {
        super("");
        
        this.disc = disc;
        this.titleNum = titleNum;
        this.playList = playList;
    }

    public BDLocator(String disc, int titleNum, int jar, int sound) throws InvalidLocatorException
    {
        super("");
        
        this.disc = disc;
        this.titleNum = titleNum;
        this.jar = jar;
        this.sound = sound;
    }

    public BDLocator(String disc, int titleNum, int playList, int playItem, int mark, String[] componentTags)
            throws InvalidLocatorException
    {
        super("");
        
        this.disc = disc;
        this.titleNum = titleNum;
        this.playList = playList;
        this.playItem = playItem;
        this.mark = mark;
        this.componentTags = componentTags.clone();
        
        for (String comp : componentTags) {
            if (comp.startsWith("A1:"))
                primaryAudioNum = Integer.parseInt(comp.substring(3));
            else if (comp.startsWith("A2:"))
                secondaryAudioNum = Integer.parseInt(comp.substring(3));
            else if (comp.startsWith("V1:"))
                primaryVideoNum = Integer.parseInt(comp.substring(3));
            else if (comp.startsWith("V2:"))
                secondaryVideoNum = Integer.parseInt(comp.substring(3));
            else if (comp.startsWith("P:"))
                textStreamNum = Integer.parseInt(comp.substring(2));
            else
                throw new InvalidLocatorException("Invalid component");
        }
    }

    public String[] getComponentTags()
    {
        return componentTags;
    }

    public String getPathSegments()
    {
        return pathSegments;
    }

    public String getDiscId()
    {
        return disc;
    }

    public int getPrimaryAudioStreamNumber()
    {
        return primaryAudioNum;
    }

    public int getSecondaryAudioStreamNumber()
    {
        return secondaryAudioNum;
    }

    public int getPrimaryVideoStreamNumber()
    {
        return primaryVideoNum;
    }

    public int getSecondaryVideoStreamNumber()
    {
        return secondaryVideoNum;
    }

    public int getJarFileId()
    {
        return jar;
    }

    public int getMarkId()
    {
        return mark;
    }

    public int getPGTextStreamNumber()
    {
        return textStreamNum;
    }

    public int getPlayItemId()
    {
        return playItem;
    }

    public int getPlayListId()
    {
        return playList;
    }

    public int getSoundId()
    {
        return sound;
    }

    public int getTitleNumber()
    {
        return titleNum;
    }

    public boolean isJarFileItem()
    {
        return isJarItem;
    }

    public boolean isPlayListItem()
    {
        return isPlayItem;
    }

    public boolean isSoundItem()
    {
        return isSoundItem;
    }

    public String toExternalForm()
    {
        StringBuilder str = new StringBuilder();

        str.append("bd://");
        
        if (disc != null && disc != "")
            str.append(disc + ".");

            
        if (titleNum != -1)
            str.append(Integer.toString(titleNum, 16) + ".");
        
        if (isJarItem) {
            str.append("JAR:");
            str.append(BDJUtil.makeFiveDigitStr(jar));
            
            if (pathSegments != null)
                str.append(pathSegments);
            else if (sound != -1) {
                str.append(".SOUND:");
                str.append(Integer.toString(sound, 16));
            }
        } else if (isPlayItem) {
            str.append("PLAYLIST:");
            str.append(BDJUtil.makeFiveDigitStr(playList));
            
            if (playItem != -1) {
                str.append(".ITEM:");
                str.append(BDJUtil.makeFiveDigitStr(playItem));
            }
            if (mark != -1) {
                str.append(".MARK:");
                str.append(BDJUtil.makeFiveDigitStr(mark));
            }
            
            if (componentTags != null) {
                for (int i = 0; i < componentTags.length; i++) {
                    if (i != 0)
                        str.append("&");
                
                    str.append(componentTags[i]);
                }
            }
        } else if (isSoundItem) {
            str.append("SOUND:");
            str.append(Integer.toString(sound, 16));
        }
        
        return str.toString();
    }

    public String toString()
    {
        return toExternalForm();
    }

    public boolean equals(Object obj)
    {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        BDLocator other = (BDLocator) obj;
        if (!Arrays.equals(componentTags, other.componentTags))
            return false;
        if (disc == null) {
            if (other.disc != null)
                return false;
        } else if (!disc.equals(other.disc))
            return false;
        if (isJarItem != other.isJarItem)
            return false;
        if (isPlayItem != other.isPlayItem)
            return false;
        if (isSoundItem != other.isSoundItem)
            return false;
        if (jar != other.jar)
            return false;
        if (mark != other.mark)
            return false;
        if (pathSegments == null) {
            if (other.pathSegments != null)
                return false;
        } else if (!pathSegments.equals(other.pathSegments))
            return false;
        if (playItem != other.playItem)
            return false;
        if (playList != other.playList)
            return false;
        if (primaryAudioNum != other.primaryAudioNum)
            return false;
        if (primaryVideoNum != other.primaryVideoNum)
            return false;
        if (secondaryAudioNum != other.secondaryAudioNum)
            return false;
        if (secondaryVideoNum != other.secondaryVideoNum)
            return false;
        if (sound != other.sound)
            return false;
        if (textStreamNum != other.textStreamNum)
            return false;
        if (titleNum != other.titleNum)
            return false;
        return true;
    }

    public int hashCode()
    {
        final int prime = 31;
        int result = 1;
        result = prime * result + Arrays.hashCode(componentTags);
        result = prime * result + ((disc == null) ? 0 : disc.hashCode());
        result = prime * result + (isJarItem ? 1231 : 1237);
        result = prime * result + (isPlayItem ? 1231 : 1237);
        result = prime * result + (isSoundItem ? 1231 : 1237);
        result = prime * result + jar;
        result = prime * result + mark;
        result = prime * result
                + ((pathSegments == null) ? 0 : pathSegments.hashCode());
        result = prime * result + playItem;
        result = prime * result + playList;
        result = prime * result + primaryAudioNum;
        result = prime * result + primaryVideoNum;
        result = prime * result + secondaryAudioNum;
        result = prime * result + secondaryVideoNum;
        result = prime * result + sound;
        result = prime * result + textStreamNum;
        result = prime * result + titleNum;
        return result;
    }
    
    private void parsePlaylist(Scanner scan) throws InvalidLocatorException
    {
        ArrayList<String> components = new ArrayList<String>(4);
        
        try {
            while (scan.hasNext()) {
                String name = scan.next();

                if (scan.hasNext()) {
                    if (name.equals("A1")) {
                        String a1 = scan.next();
                        primaryAudioNum = Integer.parseInt(a1);
                        components.add(name + ":" + a1);
                    } else if (name.equals("A2")) {
                        String a2 = scan.next();
                        secondaryAudioNum = Integer.parseInt(a2);
                        components.add(name + ":" + a2);
                    }
                    else if (name.equals("V1")) {
                        String v1 = scan.next();
                        primaryVideoNum = Integer.parseInt(v1);
                        components.add(name + ":" + v1);
                    }
                    else if (name.equals("V2")) {
                        String v2 = scan.next();
                        secondaryVideoNum = Integer.parseInt(v2);
                        components.add(name + ":" + v2);
                    }
                    else if (name.equals("P")) {
                        String p = scan.next();
                        textStreamNum = Integer.parseInt(p);
                        components.add(name + ":" + p);
                    }
                    else if (name.equals("MARK"))
                        mark = scan.nextInt();
                    else if (name.equals("ITEM"))
                        playItem = scan.nextInt();
                    else if (name.length() == 32)
                        disc = name;
                    else if (name.length() <= 4)
                        titleNum = Integer.parseInt(name, 16);
                    else
                        throw new InvalidLocatorException("Invalid identifier");

                } else {
                    throw new InvalidLocatorException("Missing value");
                }
            }
        } catch (InputMismatchException ex) {
            throw new InvalidLocatorException("Failed to parse value");
        }
    }

    public static final int NOTLOCATED = -1;

    protected String pathSegments = null;
    protected String[] componentTags = null;
    protected String disc = null;

    protected int primaryAudioNum = -1;
    protected int secondaryAudioNum = -1;
    protected int primaryVideoNum = -1;
    protected int secondaryVideoNum = -1;
    protected int textStreamNum = -1;
    protected int jar = -1;
    protected int mark = -1;
    protected int playItem = -1;
    protected int playList = -1;
    protected int sound = -1;
    protected int titleNum = -1;

    protected boolean isJarItem = false;
    protected boolean isPlayItem = false;
    protected boolean isSoundItem = false;
    
    private static final Logger logger = Logger.getLogger(BDLocator.class.getName());
}
