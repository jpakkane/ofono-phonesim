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

#ifndef QATUTILS_P_H
#define QATUTILS_P_H

#include <qtopiaglobal.h>

#include <qobject.h>
#include <qstring.h>
#include <qbytearray.h>

class QAtResultParser;
class QTextCodec;

class QTOPIACOMM_EXPORT QAtUtils
{
private:
    QAtUtils() {}

public:

    static QString quote( const QString& str );
    static QString quote( const QString& str, QTextCodec *codec );
    static QString decode( const QString& str, QTextCodec *codec );
    static QTextCodec *codec( const QString& gsmCharset );
    static QString toHex( const QByteArray& binary );
    static QByteArray fromHex( const QString& hex );
    static QString decodeNumber( const QString& value, uint type );
    static QString decodeNumber( QAtResultParser& parser );
    static QString encodeNumber( const QString& value, bool keepPlus = false );
    static QString nextString( const QString& buf, uint& posn );
    static uint parseNumber( const QString& str, uint& posn );
    static void skipField( const QString& str, uint& posn );
    static QString stripNumber( const QString& number );
    static bool octalEscapes();
    static void setOctalEscapes( bool value );

};

#endif // QATUTILS_P_H
