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

#ifndef _TIMESTRING_H_
#define _TIMESTRING_H_

#include <QDateTime>
#include <QString>

#include <qtopiaglobal.h>

class QTOPIA_EXPORT QTimeString
{
public:
    enum Length { Short, Medium, Long };
    static QString nameOfWeekDay( int day, Length len = Medium);
    static QString nameOfMonth( int month, Length len = Medium);

    static QString numberDateString( const QDate &d, Length len = Medium );

    static QString currentFormat();
    static QStringList formatOptions();

    static QString localH( int hour );
    static QString localHM( const QTime &t, Length len = Medium );
    static QString localHMS( const QTime &t, Length len = Medium );
    static QString localHMDayOfWeek( const QDateTime &dt, Length len = Medium);
    static QString localHMSDayOfWeek( const QDateTime &dt, Length len = Medium);
    static QString localMD( const QDate &d, Length len = Medium );
    static QString localYMD( const QDate &d, Length len = Medium );
    static QString localYMDHMS( const QDateTime &d, Length len = Medium );
    static QString localDayOfWeek( const QDate &d, Length len = Medium );

    static bool currentAMPM();
private:
    friend class QtopiaApplication;
    static void updateFormats();

};

#endif
