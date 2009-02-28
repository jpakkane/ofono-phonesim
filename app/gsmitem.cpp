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

#include "gsmitem.h"

GSMItem::GSMItem(const QString& comm, const QString& prof, const QStringList& farForm, const QStringList& respFormat, const QString& desc)
{
    command = comm;
    profile = prof;
    description = desc;
    parameterFormat = farForm;
    responses = respFormat;
}

GSMItem::GSMItem()
{
}

QString GSMItem::getProfile()
{
    return profile;
}


QStringList GSMItem::getParameterFormat()
{
    return parameterFormat;
}


QStringList GSMItem::getResponseFormat()
{
    return responses;
}

QString GSMItem::getDescription()
{
    return description;
}
