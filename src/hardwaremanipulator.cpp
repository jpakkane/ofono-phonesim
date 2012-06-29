/****************************************************************************
**
** This file is part of the Qt Extended Opensource Package.
**
** Copyright (C) 2009 Trolltech ASA.
**
** Contact: Qt Extended Information (info@qtextended.org)
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation and appearing
** in the file LICENSE.GPL included in the packaging of this file.
**
** Please review the following information to ensure GNU General Public
** Licensing requirements will be met:
**     http://www.fsf.org/licensing/licenses/info/GPLv2.html.
**
**
****************************************************************************/

#include "hardwaremanipulator.h"
#include <Qt>
#include <qdebug.h>
#include <qbuffer.h>
#include <qtimer.h>
#include <qsmsmessage.h>
#include <qcbsmessage.h>
#include <qgsmcodec.h>
#include <qwsppdu.h>
#include <qatutils.h>
#include <phonesim.h>
#include <simapplication.h>

#define NIBBLE_MAX 15
#define TWO_BYTE_MAX 65535
#define THREE_BYTE_MAX 16777215
#define PORT_MAX 999999;//arbitrary value
#define FOUR_CHAR 4
#define SIX_CHAR 6
#define ONE_CHAR 1
#define HEX_BASE 16

HardwareManipulator::HardwareManipulator(SimRules *sr, QObject *parent)
        : QObject(parent), rules(sr)
{
    simPresent = true;
}


QSMSMessageList & HardwareManipulator::getSMSList()
{
    return SMSList;
}

void HardwareManipulator::warning( const QString &title, const QString &message)
{
    qWarning() << title << ":" << message;
}

void HardwareManipulator::setPhoneNumber( const QString& )
{
}

QString PS_toHex( const QByteArray& binary );

void HardwareManipulator::constructCBMessage(const QString &messageCode, int geographicalScope, const QString &updateNumber,
    const QString &channel, int language, const QString &content)
{

    bool ok;
    uint mc = convertString(messageCode,1023,3,HEX_BASE, &ok);
    if ( !ok ) {
        warning(tr("Invalid Message Code"),
                tr("Message code 3 hex digits long and no larger than 3FF"));
        return;
    }

    QCBSMessage::GeographicalScope gs = (QCBSMessage::GeographicalScope)geographicalScope;

    uint un = convertString(updateNumber,NIBBLE_MAX,ONE_CHAR,HEX_BASE,&ok);
    if ( !ok ) {
        warning(tr("Invalid Update Number"),
                tr("Update number must be 1 hex digit long"
                   "and no larger than F"));
        return;
    }


    uint ch = convertString(channel, TWO_BYTE_MAX,FOUR_CHAR,HEX_BASE,&ok);
    if ( !ok ) {
        warning(tr("Invalid Channel,"),
                tr("Channel  must be 4 hex digits long "
                   "and no larger than FFFF"));
        return;
    }

    QCBSMessage::Language lang = (QCBSMessage::Language)language;

    QCBSMessage m;
    m.setMessageCode(mc);
    m.setScope(gs);
    m.setUpdateNumber(un);
    m.setChannel(ch);
    m.setLanguage(lang);
    m.setText(content);

    sendCBS(m);
}

void HardwareManipulator::sendCBS( const QCBSMessage &m )
{
    uint numPages, spaceLeftInLast;
    m.computeSize( numPages, spaceLeftInLast );

    if ( numPages > 15) {
            warning(tr("Text too long"),
                    tr("The maximum number of pages (15) is reached"
                       " - Text is truncated"));
    }

    if( numPages >1 ) {
        QList<QCBSMessage> list = m.split();
        for( int i =0; i < list.count(); i++ ) {
            QByteArray pdu = list[i].toPdu();
             emit unsolicitedCommand(QString("+CBM: ")+QString::number(pdu.length())+'\r'+'\n'+ PS_toHex(pdu));
         }
    } else {
        QByteArray pdu = m.toPdu();
        emit unsolicitedCommand(QString("+CBM: ")+QString::number(pdu.length())+'\r'+'\n'+ PS_toHex(pdu));
    }
}

void HardwareManipulator::constructSMSMessage( const int type, const QString &sender, const QString &serviceCenter, const QString &text )
{
    QSMSMessage m;
    m.setMessageClass(type);
    m.setSender(sender);
    m.setServiceCenter(serviceCenter);
    m.setText(text);
    m.setTimestamp(QDateTime::currentDateTime());
    sendSMS(m);

}

void HardwareManipulator::sendSMS( const QSMSMessage &m )
{
    if( m.shouldSplit() ) {
        QList<QSMSMessage> list = m.split();

        for( int i =0; i < list.count(); i++ ) {
            QByteArray pdu = list[i].toPdu();
            uint pdulen = pdu.length() - QSMSMessage::pduAddressLength( pdu );
            SMSList.appendSMS( pdu, pdulen );
            emit unsolicitedCommand("+CMTI: \"SM\","+QString::number( getSMSList().count()));
        }
    } else {
        QByteArray pdu = m.toPdu();
        uint pdulen = pdu.length() - QSMSMessage::pduAddressLength( pdu );
        SMSList.appendSMS( pdu, pdulen );
        emit unsolicitedCommand("+CMTI: \"SM\","+QString::number( getSMSList().count()));
    }
}

void HardwareManipulator::constructSMSDatagram(int src, int dst,
                                               const QString &sender,
                                               const QByteArray &data,
                                               const QByteArray &contentType)
{
    QWspPush pdu;
    pdu.setIdentifier(0);
    pdu.setPduType(6);

    pdu.setData(data.data(),data.length());

    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QWspPduEncoder encoder(&buffer);

    if ( contentType.length() != 0 ) {
        pdu.addHeader("Content-Type", contentType);
        encoder.encodePush(pdu);
    } else {
        pdu.writeData(&buffer);
    }
    QByteArray appData = buffer.buffer();
    buffer.close();

    QSMSMessage m;
    m.setDestinationPort(dst);
    m.setSourcePort(src);
    m.setSender(sender);
    m.setApplicationData(appData);
    m.setTimestamp( QDateTime::currentDateTime() );

    sendSMS(m);
}

int HardwareManipulator::convertString(const QString &number, int maxValue, int numChar, int base, bool *ok)
{
    bool b;
    int num = number.toInt(&b, base);

    *ok = true;
    if ( !b || num < 0 || num > maxValue || number.size() != numChar ) {
        *ok = false;
    }
    return num;
}

void HardwareManipulator::handleFromData( const QString& /*cmd*/ )
{
}

void HardwareManipulator::handleToData( const QString& /*cmd*/ )
{
}

void HardwareManipulator::sendVMNotify( int type, int count, const QList<QVMMessage> &received, const QList<QVMMessage> &deleted, const QString &mailbox )
{
    QSMSMessage m;
    QByteArray mwiUdh;
    int scheme = 0xc0;

    if ( count )
        scheme |= 0x08;

    if ( type == 0 ) {
        mwiUdh.append( 0x01 );     // Special SMS message indication
        mwiUdh.append( 0x02 );     // IE length
        mwiUdh.append( (char) 0 ); // Message type: Voice
        mwiUdh.append( count );    // Message count
    } else if ( type == 1 ) {
        if ( received.size() ) {
            int lengthOctet = mwiUdh.size() + 1;

            mwiUdh.append( 0x23 );     // Enhanced Voice Mail Notification
            mwiUdh.append( 0x02 );     // IE length (Filled in later)
            mwiUdh.append( (char) 0 ); // Parameters
            QSMSMessage::appendAddress( mwiUdh, mailbox, false );
            mwiUdh.append( count );    // Number of Voice Messages
            mwiUdh.append( received.size() ); // Number of VM Notifications

            foreach ( QVMMessage msg, received ) {
                mwiUdh.append( msg.id & 0xff );
                mwiUdh.append( msg.id >> 8 ); // Endianness doesn't matter here
                mwiUdh.append( qMin( msg.lengthSecs, 255 ) );
                mwiUdh.append( qMin( msg.retentionDays, 31 ) |
                        ( msg.priority ? 0x40 : 0 ) );
                QSMSMessage::appendAddress( mwiUdh, msg.sender, false );
            }

	    mwiUdh[lengthOctet] = mwiUdh.size() - ( lengthOctet + 1 );
        }

        if ( deleted.size() || !received.size() ) {
            int lengthOctet = mwiUdh.size() + 1;

            mwiUdh.append( 0x23 );  // Enhanced Voice Mail Notification
            mwiUdh.append( 0x02 );  // IE length (Filled in later)
            mwiUdh.append( 0x01 );  // Parameters
            QSMSMessage::appendAddress( mwiUdh, mailbox, false );
            mwiUdh.append( count ); // Number of Voice Messages
            mwiUdh.append( deleted.size() ); // Number of VM Notifications

            foreach ( QVMMessage msg, deleted ) {
                mwiUdh.append( msg.id & 0xff );
                mwiUdh.append( msg.id >> 8 ); // Endianness doesn't matter here
                mwiUdh.append( qMin( msg.lengthSecs, 255 ) );
                mwiUdh.append( qMin( msg.retentionDays, 31 ) |
                        ( msg.priority ? 0x40 : 0 ) );
                QSMSMessage::appendAddress( mwiUdh, msg.sender, false );
            }

	    mwiUdh[lengthOctet] = mwiUdh.size() - ( lengthOctet + 1 );
        }
    }

    m.setDataCodingScheme( scheme );
    m.setSender( mailbox );
    m.setTimestamp( QDateTime::currentDateTime() );

    m.setHeaders( mwiUdh );

    sendSMS( m );
}

void HardwareManipulator::sendUSSD( bool cancel, bool response,
		const QString &content )
{
    /* TODO: if rules->variable("USD") == "0" then return */
    if (cancel)
        emit unsolicitedCommand( "+CUSD: 2" );
    else {
        QTextCodec *codec = QAtUtils::codec( "gsm-noloss" );
        bool gsmSafe;
        QString request;

        // Check the body for non-GSM characters.
        gsmSafe = codec->canEncode( content );

        // Use the default alphabet if everything is GSM-compatible.
        if ( gsmSafe ) {
            QTextCodec *codec = QAtUtils::codec( rules->variable("SCS") );
            request= QAtUtils::quote( content, codec );
        } else {
            // Encode the text using the 16-bit UCS2 alphabet.
            uint u;
            QByteArray binary;
            uint len = content.length();

            for ( u = 0; u < len; u++ ) {
                binary += (unsigned char)(content[u].unicode() >> 8);
                binary += (unsigned char)(content[u].unicode() & 0xFF);
            }
            request = PS_toHex( binary );
        }
        emit unsolicitedCommand( "+CUSD: " +
                    QString::number( response ? 1 : 0 ) + ",\"" +
                    request + "\"," + QString::number( gsmSafe ? 0 : 0x48 ));
    }
}

bool HardwareManipulator::getSimPresent()
{
    return simPresent;
}

void HardwareManipulator::setSimPresent( bool present )
{
    simPresent = present;
}

QStringList HardwareManipulator::getSimAppsNameList()
{
    const QList<SimApplication *> simApps = rules->getSimApps();
    QStringList nameList;

    for ( int i = 0; i  < simApps.count(); i++ )
        nameList.append( simApps.at( i )->getName() );

    return nameList;
}

void HardwareManipulator::handleNewApp()
{
}

void HardwareManipulator::simAppStart( int appIndex )
{
    const QList<SimApplication *> simApps = rules->getSimApps();

    rules->setSimApplication( simApps.at( appIndex ) );
    simApps.at( appIndex )->start();
}

void HardwareManipulator::simAppAbort()
{
    SimApplication *app = rules->simApplication();

    if (app)
        return app->abort();
}

void HardwareManipulator::callManagement( QList<CallInfo> *info )
{
}
