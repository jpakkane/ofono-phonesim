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

#ifndef QSMSMESSAGELIST_H
#define QSMSMESSAGELIST_H

#include <QList>
#include <QByteArray>

class QSMSMessageList
{
public:
    enum SMSStatus{
        REC_UNREAD  =0,
        REC_READ    =1,
        STO_UNSENT  =2,
        STO_SENT    =3
    };

    QSMSMessageList();
    ~QSMSMessageList();

    void appendSMS( const QByteArray & );
    void deleteSMS( int );  //note SMS's are not actually physically deleted. A flag is set

    int count() const; //returns the total number, even those that are 'deleted'

    SMSStatus getStatus( int ) const;
    void  setStatus( const SMSStatus &, int );

    bool getDeletedFlag( int ) const;
    void setDeletedFlag( bool, int );

    QByteArray & readSMS( int );//returns and sets the status of an SMS
    QByteArray & operator[]( int );//only returns an SMS, does not set status

private:
    QList<QByteArray> SMSList;
    QList<SMSStatus> statusList;
    QList<bool> deletedFlagList;
};

#endif
