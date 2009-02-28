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

#include "qsmsmessagelist.h"
#include <qdebug.h>

QSMSMessageList::QSMSMessageList()
{
SMSList.clear();
statusList.clear();
deletedFlagList.clear();
}

QSMSMessageList::~QSMSMessageList()
{
}

void QSMSMessageList::appendSMS( const QByteArray &m )
{
    SMSList.append(m);
    statusList.append(QSMSMessageList::REC_UNREAD);
    deletedFlagList.append(false);
}


void QSMSMessageList::deleteSMS( int i )
{
    deletedFlagList[i] = true;
}

int QSMSMessageList::count() const
{
    return SMSList.count();
}

QSMSMessageList::SMSStatus QSMSMessageList::getStatus( int i ) const
{
   return statusList[i];
}

void QSMSMessageList::setStatus( const SMSStatus &s, int i )
{
    statusList[i] = s;
}


bool QSMSMessageList::getDeletedFlag( int i ) const
{
    return deletedFlagList[i];
}

void QSMSMessageList::setDeletedFlag( bool b, int i )
{
    deletedFlagList[i] = b;
}

QByteArray & QSMSMessageList::readSMS( int i )
{
   if ( statusList[i] == QSMSMessageList::REC_UNREAD ) {
        statusList[i] = QSMSMessageList::REC_READ;
  }

   return SMSList[i];
}

QByteArray & QSMSMessageList::operator[]( int i )
{
    return SMSList[i];

}



