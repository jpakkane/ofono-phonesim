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

#ifndef PHONESIMSERVER_H
#define PHONESIMSERVER_H

#include <qtcpserver.h>
#include <qtcpsocket.h>
#include <qpointer.h>

#include "phonesim.h"

class PhoneTestServer;
class HardwareManipulatorFactory;

class PhoneSimServer : public QTcpServer
{
public:
    PhoneSimServer(const QString &, quint16 port, QObject *parent = 0);
    ~PhoneSimServer();

    void setHardwareManipulator(HardwareManipulatorFactory *f);

    SimRules *rules() const { return currentRules; }

protected:
    void incomingConnection(int s);

private:
    QString filename;
    QString peerFile;

    HardwareManipulatorFactory *fact;
    QPointer<SimRules> currentRules;
};

#endif

