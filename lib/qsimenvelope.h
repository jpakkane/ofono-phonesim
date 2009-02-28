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
#ifndef QSIMENVELOPE_H
#define QSIMENVELOPE_H

#include <qsimcommand.h>

class QSimEnvelopePrivate;

class QTOPIAPHONE_EXPORT QSimEnvelope
{
public:
    QSimEnvelope();
    QSimEnvelope( const QSimEnvelope& value );
    ~QSimEnvelope();

    enum Type
    {
        NoEnvelope              = -1,
        SMSPPDownload           = 0xD1,
        CellBroadcastDownload   = 0xD2,
        MenuSelection           = 0xD3,
        CallControl             = 0xD4,
        MOSMSControl            = 0xD5,
        EventDownload           = 0xD6,
        TimerExpiration         = 0xD7
    };

    enum Event
    {
        NoEvent                 = -1,
        MTCall                  = 0,
        CallConnected           = 1,
        CallDisconnected        = 2,
        LocationStatus          = 3,
        UserActivity            = 4,
        IdleScreenAvailable     = 5,
        CardReaderStatus        = 6,
        LanguageSelection       = 7,
        BrowserTermination      = 8,
        DataAvailable           = 9,
        ChannelStatus           = 10
    };

    QSimEnvelope::Type type() const;
    void setType( QSimEnvelope::Type value );

    QSimCommand::Device sourceDevice() const;
    void setSourceDevice( QSimCommand::Device value );

    QSimCommand::Device destinationDevice() const;
    void setDestinationDevice( QSimCommand::Device value );

    uint menuItem() const;
    void setMenuItem( uint value );

    bool requestHelp() const;
    void setRequestHelp( bool value );

    QSimEnvelope::Event event() const;
    void setEvent( QSimEnvelope::Event value );

    QByteArray extensionData() const;
    void setExtensionData( QByteArray value );

    QByteArray extensionField( int tag ) const;
    void addExtensionField( int tag, const QByteArray& value );

    static QSimEnvelope fromPdu( const QByteArray& pdu );
    QByteArray toPdu() const;

    QSimEnvelope& operator=( const QSimEnvelope & );
    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

private:
    QSimEnvelopePrivate *d;
};

Q_DECLARE_USER_METATYPE(QSimEnvelope)

#endif // QSIMENVELOPE_H
