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
public:
    SimApplication( SimRules *rules, QObject *parent = 0 );
    ~SimApplication();

    virtual bool envelope( const QSimEnvelope& env );
    virtual bool response( const QSimTerminalResponse& resp );
    virtual QByteArray fetch( bool clear = false );

    virtual const QString getName() = 0;

public slots:
    virtual void start();
    virtual void abort();

protected slots:
    void command( const QSimCommand& cmd,
                  QObject *target, const char *slot,
                  QSimCommand::ToPduOptions options
                        = QSimCommand::NoPduOptions );
    void controlEvent( const QSimControlEvent& event );

    virtual void mainMenu() = 0;
    virtual void mainMenuSelection( int id );
    virtual void mainMenuHelpRequest( int id );
    virtual void endSession();

private:
    SimApplicationPrivate *d;
};

class DemoSimApplication : public SimApplication
{
    Q_OBJECT
public:
    DemoSimApplication( SimRules *rules, QObject *parent = 0 );
    ~DemoSimApplication();

    const QString getName();

protected slots:
    void mainMenu();
    void mainMenuSelection( int id );
    void sendSportsMenu();
    void sportsMenu( const QSimTerminalResponse& resp );
    void sendCallsMenu();
    void callsMenu( const QSimTerminalResponse& resp );
    void startSticksGame();
    void sticksGameShow();
    void sticksGameLoop( const QSimTerminalResponse& resp );
    void getInputLoop( const QSimTerminalResponse& resp );
    void sticksGamePlayAgain( const QSimTerminalResponse& resp );
    void sendToneMenu();
    void toneMenu( const QSimTerminalResponse& resp );
    void sendIconMenu();
    void iconMenu( const QSimTerminalResponse& resp );
    void sendIconSEMenu();
    void iconSEMenu( const QSimTerminalResponse& resp );
    void sendDisplayText();
    void displayTextResponse( const QSimTerminalResponse& resp );
    void sendBrowserMenu();
    void browserMenu( const QSimTerminalResponse& resp );
    void sendDTMF();
    void sendSendSSMenu();
    void sendSSMenu( const QSimTerminalResponse& resp );
    void sendCBMenu();
    void CBMenu( const QSimTerminalResponse& resp );
    void sendCFMenu();
    void CFMenu( const QSimTerminalResponse& resp );
    void sendCWMenu();
    void CWMenu( const QSimTerminalResponse& resp );
    void sendCLIPMenu();
    void CLIPMenu( const QSimTerminalResponse& resp );
    void sendCLIRMenu();
    void CLIRMenu( const QSimTerminalResponse& resp );
    void sendCoLPMenu();
    void CoLPMenu( const QSimTerminalResponse& resp );
    void sendCoLRMenu();
    void CoLRMenu( const QSimTerminalResponse& resp );

private:
    int sticksLeft;
    bool immediateResponse;
};

#endif
