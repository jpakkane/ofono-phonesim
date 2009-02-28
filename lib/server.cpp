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

#include "server.h"
#include "phonesim.h"
#include "hardwaremanipulator.h"
#include <qdebug.h>

static int phonenumber = 555000;

PhoneSimServer::PhoneSimServer(const QString &f, quint16 port, QObject *parent)
    : QTcpServer(parent), fact(0)
{
    listen( QHostAddress::Any, port );
    filename = f;
    // Parse the "peers.xml" file.
    QFileInfo info( filename );
    QString peerFile = info.absolutePath() + "/peers.xml";
}

PhoneSimServer::~PhoneSimServer()
{
    setHardwareManipulator(0);
}

void PhoneSimServer::setHardwareManipulator(HardwareManipulatorFactory *f)
{
    delete fact;
    fact = f;
    if (f)
        f->setRuleFile(filename);
}

void PhoneSimServer::incomingConnection(int s)
{
  SimRules *sr = new SimRules(s, this, filename, fact);
    if ( QFile::exists( peerFile ) )
        sr->loadPeers( peerFile );
    sr->setPhoneNumber(QString::number(phonenumber));
    phonenumber++;
    currentRules = sr;
}
