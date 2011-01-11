/****************************************************************************
**
** This file contains the SAT conformance test cases
**
** Copyright 2011 EB(Elektrobit).
**
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

#include "simapplication.h"
#include <qatutils.h>
#include <qdebug.h>
#include <QTextCodec>
#include "qsmsmessage.h"

ConformanceSimApplication::ConformanceSimApplication( SimRules *rules,
                                                        QObject *parent )
    : SimApplication( rules, parent )
{
}

ConformanceSimApplication::~ConformanceSimApplication()
{
}

const QString ConformanceSimApplication::getName()
{
    return "Conformance SIM Application";
}

void ConformanceSimApplication::mainMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SetupMenu );

    cmd.setMenuItems( items );

    command( cmd, 0, 0 );
}

void ConformanceSimApplication::mainMenuSelection( int id )
{
    switch ( id ) {
        default:
        {
            // Don't know what this item is, so just re-display the main menu.
            endSession();
        }
        break;
    }
}
