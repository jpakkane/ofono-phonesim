/****************************************************************************
**
** Copyright (C) 2000-2007 TROLLTECH ASA. All rights reserved.
**
** This file is part of the Opensource Edition of the Qtopia Toolkit.
**
** This software is licensed under the terms of the GNU General Public
** License (GPL) version 2.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "gsmspec.h"
#include <qdebug.h>

GSMSpec::GSMSpec(const QString& specFile)
{
    setupDictionary(specFile);
}


QString GSMSpec::getProfile(const QString& command)
{
    if(commandExists(command)){
        GSMItem gi = commandMap.value(command);
        return gi.getProfile();
    }else{
        return "";
    }
}


QStringList GSMSpec::getParameterFormat(const QString& command)
{
    if(commandExists(command)){
        GSMItem gi = commandMap.value(command);
        return gi.getParameterFormat();
    }else{
        return QStringList("format not available");
    }
}


QStringList GSMSpec::getResponseFormat(const QString& command)
{
    if(commandExists(command)){
        GSMItem gi = commandMap.value(command);
        return gi.getParameterFormat();
    }else{
        return QStringList();
    }
}


bool GSMSpec::commandExists(const QString& command)
{
    return commandMap.contains(command);
}


bool GSMSpec::validateCommand(QString format, QString pars)
{
    Q_UNUSED(pars);
    QStringList formatList = format.split(",");
    QStringList parsList = format.split(",");

    if( formatList.size() == 0 && parsList.size() == 0 ){
        return true;
    }else if( true ){
    }
    return true; // TODO: Make useful
}


bool GSMSpec::validateResponse(QString format, QString pars)
{
    Q_UNUSED(pars);
    QStringList formatList = format.split(",");
    QStringList parsList = format.split(",");

    if( formatList.size() == 0 && parsList.size() == 0 ){
        return true;
    }else if( true ){
    }
    return true; // TODO: Make useful
}

void GSMSpec::resetDictionary(const QString& filePath)
{
    if( QFile::exists(filePath) ){
        setupDictionary(filePath);
    }
}

void GSMSpec::setupDictionary(const QString& filePath)
{

    commandMap.clear();
    QFile file(filePath);
    QString line;
    QStringList okList = QStringList("OK");

    GsmXmlHandler *handler = new GsmXmlHandler();
    QXmlSimpleReader *xmlReader = new QXmlSimpleReader();
    xmlReader->setContentHandler( handler );

    QXmlInputSource source( &file );

    if( !xmlReader->parse(source) ){
        qWarning() << "Failed to parse GSM xml file" ;
        file.close();
        return;
    }
    file.close();

    GsmXmlNode *specNode = handler->documentElement()->children;
    QString command, profile, format, response, description;

    while ( specNode != 0 ) {
        if ( specNode->tag == "spec" ) {

            GsmXmlNode *specData = specNode->children;
            while ( specData != 0 ) {

                if( specData->tag == "command" ) {
                    command = specData->contents;
                }else if( specData->tag == "profile" ) {
                    profile = specData->contents;
                }else if( specData->tag == "format" ) {
                    format = specData->contents;
                }else if( specData->tag == "response" ) {
                    response = specData->contents;
                }else if( specData->tag == "description" ) {
                    description = specData->contents;
                }else{
                    // not a valid entry
                    break;
                }
                specData = specData->next;

            }
            // Add a phone simulator peer entry.
            commandMap.insert( command, GSMItem(command, profile, format.split(";"), response.split(";"), description) );
        }
        specNode = specNode->next;
    }
    // Clean up the XML reader objects.
    delete xmlReader;
    delete handler;

}

GsmXmlNode::GsmXmlNode( const QString& _tag )
{
    parent = 0;
    next = 0;
    children = 0;
    attributes = 0;
    tag = _tag;
}


GsmXmlNode::~GsmXmlNode()
{
    GsmXmlNode *temp1, *temp2;
    temp1 = children;
    while ( temp1 ) {
        temp2 = temp1->next;
        delete temp1;
        temp1 = temp2;
    }
    temp1 = attributes;
    while ( temp1 ) {
        temp2 = temp1->next;
        delete temp1;
        temp1 = temp2;
    }
}


void GsmXmlNode::addChild( GsmXmlNode *child )
{
    GsmXmlNode *current = children;
    GsmXmlNode *prev = 0;
    while ( current ) {
        prev = current;
        current = current->next;
    }
    if ( prev ) {
        prev->next = child;
    } else {
        children = child;
    }
    child->next = 0;
    child->parent = this;
}


void GsmXmlNode::addAttribute( GsmXmlNode *child )
{
    GsmXmlNode *current = attributes;
    GsmXmlNode *prev = 0;
    while ( current ) {
        prev = current;
        current = current->next;
    }
    if ( prev ) {
        prev->next = child;
    } else {
        attributes = child;
    }
    child->next = 0;
    child->parent = this;
}


QString GsmXmlNode::getAttribute( const QString& name )
{
    GsmXmlNode *current = attributes;
    while ( current ) {
        if ( current->tag == name )
            return current->contents;
        current = current->next;
    }
    return QString();
}


GsmXmlHandler::GsmXmlHandler()
{
    tree = new GsmXmlNode( QString() );
    current = tree;
}


GsmXmlHandler::~GsmXmlHandler()
{
    delete tree;
}


bool GsmXmlHandler::startElement( const QString&, const QString& localName, const QString&, const QXmlAttributes& atts )
{
    GsmXmlNode *node = new GsmXmlNode( localName );
    GsmXmlNode *attr;
    int index;
    current->addChild( node );
    for ( index = 0; index < atts.length(); ++index ) {
        attr = new GsmXmlNode( atts.localName( index ) );
        attr->contents = atts.value( index );
        node->addAttribute( attr );
    }
    current = node;
    return true;
}


bool GsmXmlHandler::endElement( const QString&, const QString& , const QString&)
{
    current = current->parent;
    return true;
}


bool GsmXmlHandler::characters( const QString& ch )
{
    current->contents += ch;
    return true;
}


bool GsmXmlHandler::ignorableWhitespace( const QString& ch )
{
    current->contents += ch;
    return true;
}


GsmXmlNode *GsmXmlHandler::documentElement() const
{
    if ( tree->children && tree->children->tag == "gsm" ) {
        return tree->children;
    } else {
        return tree;
    }
}
