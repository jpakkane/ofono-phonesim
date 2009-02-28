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
#ifndef QSIMCONTROLEVENT_H
#define QSIMCONTROLEVENT_H

#include <qtopiaipcmarshal.h>

class QSimControlEventPrivate;

class QTOPIAPHONE_EXPORT QSimControlEvent
{
public:
    QSimControlEvent();
    QSimControlEvent( const QSimControlEvent& value );
    ~QSimControlEvent();

    enum Type
    {
        Call  = 0,
        Sms   = 1
    };

    enum Result
    {
        Allowed                     = 0,
        NotAllowed                  = 1,
        AllowedWithModifications    = 2
    };

    QSimControlEvent::Type type() const;
    void setType( QSimControlEvent::Type value );

    QSimControlEvent::Result result() const;
    void setResult( QSimControlEvent::Result value );

    QString text() const;
    void setText( const QString& value );

    QByteArray extensionData() const;
    void setExtensionData( QByteArray value );

    QByteArray extensionField( int tag ) const;
    void addExtensionField( int tag, const QByteArray& value );

    static QSimControlEvent fromPdu( QSimControlEvent::Type type, const QByteArray& pdu );
    QByteArray toPdu() const;

    QSimControlEvent& operator=( const QSimControlEvent & );
    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

private:
    QSimControlEventPrivate *d;
};

Q_DECLARE_USER_METATYPE(QSimControlEvent)

#endif // QSIMCONTROLEVENT_H
