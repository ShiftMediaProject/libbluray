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

package org.videolan;

import java.awt.Font;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.LinkedList;

import javax.xml.stream.FactoryConfigurationError;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

public class FontIndex {
    public FontIndex()
    {
        
    }
    
    public void parse(String file) throws BDJException
    {
        FileInputStream inStream;
        try {
            inStream = new FileInputStream(file);
        } catch (FileNotFoundException e) {
            throw new BDJException("Not found");
        }
        
        try {
            XMLStreamReader reader = XMLInputFactory.newInstance().createXMLStreamReader(inStream);
            
            parse(reader);
            
        } catch (XMLStreamException e) {
            e.printStackTrace();
            throw new BDJException("Parse error");
        } catch (FactoryConfigurationError e) {
            throw new BDJException("Couldn't create xml reader");
        }
    }
    
    public LinkedList<FontIndexData> getFontIndexData()
    {
        return fontData;
    }
    
    private void parse(XMLStreamReader reader) throws XMLStreamException
    {
        while (reader.hasNext()) {
            int type = reader.next();
            
            switch (type) {
            case XMLStreamReader.START_ELEMENT:
                parseStartElement(reader.getLocalName().toLowerCase());
                
                if (inDocument) {
                    for (int i = 0; i < reader.getAttributeCount(); i++)
                        parseAttribute(reader.getAttributeLocalName(i).toLowerCase(), reader.getAttributeLocalName(i));
                    
                    parseValue(reader);
                }
                break;
            case XMLStreamReader.END_ELEMENT:
                parseEndElement(reader.getLocalName().toLowerCase());
                break;
            }
        }
    }
    
    private void parseStartElement(String elemName) throws XMLStreamException
    {
        if (elemName.equals("fontdirectory"))
            inDocument = true;
        else if (elemName.equals("font"))
            current = new FontIndexData();
        else
            state = State.parseElement(elemName);
    }
    
    private void parseEndElement(String elemName) 
    {
        if (elemName.equals("fontdirectory"))
            inDocument = false;
        
        if (inDocument) {
            if (elemName.equals("font")) {
                fontData.add(current);
            }
        
            state = State.None;
        }
    }
    
    private void parseAttribute(String attrName, String attrValue) throws XMLStreamException
    {
        if (state == State.size) {
            if (attrName.equals("min"))
                current.minSize = Integer.parseInt(attrValue);
            else if (attrName.equals("max"))
                current.maxSize = Integer.parseInt(attrValue);
            else
                throw new XMLStreamException("invalid attribute name: " + attrName);
        } else {
            throw new XMLStreamException("invalid attribute for state");
        }
    }
    
    private void parseValue(XMLStreamReader reader) throws XMLStreamException 
    {
        switch (state) {
        case name:
            current.name = reader.getElementText();
            break;
        case fontformat:
            current.format = reader.getElementText();
            break;
        case filename:
            current.filename = reader.getElementText();
            break;
        case style:
            parseStyle(reader.getElementText());
            break;
        }
    }
    
    private void parseStyle(String style) throws XMLStreamException
    {
        if (style.equals("PLAIN"))
            current.style = Font.PLAIN;
        else if (style.equals("BOLD"))
            current.style = Font.BOLD;
        else if (style.equals("ITALIC"))
            current.style = Font.ITALIC;

        throw new XMLStreamException();
    }
    
    private enum State
    {
        None,
        name,
        fontformat,
        filename,
        size,
        style;
        
        public static State parseElement(String name) throws XMLStreamException {
            String str = name.toLowerCase();
            
            for (State state : values()) {
                if (str.equals(state.name()))
                    return state;
            }
            
            throw new XMLStreamException();
        }
    }
    
    private boolean inDocument = false;
    private State state = State.None;
    private FontIndexData current = null;
    private LinkedList<FontIndexData> fontData = new LinkedList<FontIndexData>();
    
}
