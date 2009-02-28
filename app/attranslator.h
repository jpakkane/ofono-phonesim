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

#ifndef ATTRANSLATOR_H
#define ATTRANSLATOR_H

#include <QString>
#include <QMap>
#include <QStringList>
#include "gsmspec.h"

class AtTranslator
{

public:
    AtTranslator(const QString& specFile);
    QString translateCommand( const QString& );
    QString translateResponse( const QString& );
    void resetSpecification(const QString& );

private:
    GSMSpec gsmSpec;
};

#endif
