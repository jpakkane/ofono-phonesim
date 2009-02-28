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

#ifndef GSMITEM_H
#define GSMITEM_H

#include <QString>
#include <QStringList>

class GSMItem
{

public:
    GSMItem(const QString&, const QString&, const QStringList&, const QStringList&, const QString&);
    GSMItem();
    QString getDescription();
    QString getProfile();
    QStringList getParameterFormat();
    QStringList getResponseFormat();

private:
    QString command;
    QString profile;
    QStringList parameterFormat;
    QStringList responses;
    QString description;

};

#endif
