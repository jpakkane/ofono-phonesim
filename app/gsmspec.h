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

#ifndef GSMSPEC_H
#define GSMSPEC_H

#include <QString>
#include <QMap>
#include <QStringList>
#include "gsmitem.h"
#include <QFile>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDir>

class GsmXmlNode
{
public:
    GsmXmlNode( const QString& tag );
    ~GsmXmlNode();

    GsmXmlNode *parent, *next, *children, *attributes;
    QString tag;
    QString contents;

    void addChild( GsmXmlNode *child );
    void addAttribute( GsmXmlNode *child );
    QString getAttribute( const QString& name );
};


class GsmXmlHandler : public QXmlDefaultHandler
{
public:
    GsmXmlHandler();
    ~GsmXmlHandler();

    bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
    bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName );
    bool characters( const QString& ch );
    bool ignorableWhitespace( const QString& ch );

    GsmXmlNode *documentElement() const;

private:
    GsmXmlNode *tree;
    GsmXmlNode *current;
};

class GSMSpec
{

public:
    GSMSpec(const QString& specFile);
    bool commandExists(const QString&);
    QString getDescription(const QString&);
    QString getProfile(const QString&);
    QStringList getParameterFormat(const QString&);
    QStringList getResponseFormat(const QString&);

    bool validateCommand(QString format, QString pars);
    bool validateResponse(QString format, QString pars);
    void resetDictionary( const QString& );

private:
    QMap<QString, GSMItem> commandMap;
    GSMItem getGSMItem( const QString& );
    void setupDictionary( const QString& );
    //GSMItem createGSMItem( QString command, QString profile, QStringList pars, QStringList resps );
};

#endif
