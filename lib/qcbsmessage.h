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

#ifndef QCBSMESSAGE_H
#define QCBSMESSAGE_H

#include <qstring.h>
#include <qdatastream.h>
#include <qtopiaipcmarshal.h>

class QCBSMessagePrivate;

class QTOPIAPHONE_EXPORT QCBSMessage
{
public:
    QCBSMessage();
    QCBSMessage(const QCBSMessage &);
    ~QCBSMessage();

    enum GeographicalScope
    {
        CellWide            = 0,
        PLMNWide            = 1,
        LocationAreaWide    = 2,
        CellWide2           = 3
    };

    enum Language
    {
        German      = 0,
        English     = 1,
        Italian     = 2,
        French      = 3,
        Spanish     = 4,
        Dutch       = 5,
        Swedish     = 6,
        Danish      = 7,
        Portuguese  = 8,
        Finnish     = 9,
        Norwegian   = 10,
        Greek       = 11,
        Turkish     = 12,
        Hungarian   = 13,
        Polish      = 14,
        Unspecified = 15
    };

    QCBSMessage& operator=(const QCBSMessage &);

    uint messageCode() const;
    void setMessageCode( uint code );

    QCBSMessage::GeographicalScope scope() const;
    void setScope( QCBSMessage::GeographicalScope scope );

    uint updateNumber() const;
    void setUpdateNumber( uint num );

    uint channel() const;
    void setChannel( uint chan );

    QCBSMessage::Language language() const;
    void setLanguage( QCBSMessage::Language lang );

    uint page() const;
    void setPage( uint page );

    uint numPages() const;
    void setNumPages( uint npages );

    void setText(const QString &);
    QString text() const;

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

    bool operator==( const QCBSMessage& other ) const;
    bool operator!=( const QCBSMessage& other ) const;

    void print();

    QByteArray toPdu() const;
    static QCBSMessage fromPdu( const QByteArray& pdu );

private:
    QCBSMessagePrivate *d;
};

Q_DECLARE_USER_METATYPE(QCBSMessage)

#endif
