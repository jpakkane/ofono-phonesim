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

#ifndef SIMAPPLICATION_H
#define SIMAPPLICATION_H

#include "phonesim.h"
#include <qsimcommand.h>
#include <qsimterminalresponse.h>
#include <qsimenvelope.h>
#include <qsimcontrolevent.h>

class SimApplicationPrivate;

class SimApplication : public QObject
{
    Q_OBJECT
    friend class SimRules;
public:
    SimApplication( QObject *parent = 0 );
    ~SimApplication();

    void command( const QSimCommand& cmd,
                  QObject *target, const char *slot,
                  QSimCommand::ToPduOptions options
                        = QSimCommand::NoPduOptions );

public slots:
    void controlEvent( const QSimControlEvent& event );
    virtual void start();
    virtual void abort();

protected slots:
    virtual void mainMenu() = 0;
    virtual void mainMenuSelection( int id );
    virtual void mainMenuHelpRequest( int id );

private:
    SimApplicationPrivate *d;

    void setSimRules( SimRules *rules );
    bool execute( const QString& cmd );
    void response( const QSimTerminalResponse& resp );
};

class DemoSimApplication : public SimApplication
{
    Q_OBJECT
public:
    DemoSimApplication( QObject *parent = 0 );
    ~DemoSimApplication();

protected slots:
    void mainMenu();
    void mainMenuSelection( int id );
    void sendSportsMenu();
    void sportsMenu( const QSimTerminalResponse& resp );
    void startSticksGame();
    void sticksGameShow();
    void sticksGameLoop( const QSimTerminalResponse& resp );
    void sticksGamePlayAgain( const QSimTerminalResponse& resp );
    void sendToneMenu();
    void toneMenu( const QSimTerminalResponse& resp );
    void sendIconMenu();
    void iconMenu( const QSimTerminalResponse& resp );
    void sendIconSEMenu();
    void iconSEMenu( const QSimTerminalResponse& resp );

private:
    int sticksLeft;
};

#endif /* SIMAPPLICATION_H */
