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

#ifndef HARDWAREMANIPULATOR_H
#define HARDWAREMANIPULATOR_H

#include <QObject>

#include "qsmsmessagelist.h"

class QSMSMessage;
class QCBSMessage;
class QVMMessage;
class SimRules;
struct CallInfo;
class HardwareManipulator : public QObject
{
Q_OBJECT

public:
    HardwareManipulator(SimRules *sr, QObject *parent=0);
    QSMSMessageList & getSMSList();
    bool getSimPresent();
    QStringList getSimAppsNameList();

public slots:
    virtual void handleFromData( const QString& );
    virtual void handleToData( const QString& );
    virtual void setPhoneNumber( const QString& );
    virtual void constructSMSMessage(const int type, const QString &sender, const QString &serviceCenter, const QString &text);
    virtual void sendCBS( const QCBSMessage& m );
    virtual void sendSMS( const QSMSMessage& m );
    virtual void sendVMNotify( int type, int count, const QList<QVMMessage> &received, const QList<QVMMessage> &deleted, const QString &mailbox );
    virtual void sendUSSD( bool cancel, bool response, const QString &content );
    virtual void setSimPresent( bool present );
    virtual void simAppStart( int appIndex );
    virtual void simAppAbort();
    virtual void handleNewApp();
    virtual void callManagement( QList<CallInfo> *info );

signals:
    void unsolicitedCommand(const QString &cmd);
    void command(const QString &cmd);
    void variableChanged(const QString &n, const QString &v);
    void switchTo(const QString &cmd);
    void startIncomingCall(const QString &number, const QString &called_number, const QString &name);
    void stateChangedToAlerting();
    void stateChangedToConnected();
    void stateChangedToHangup( int callId );

protected:
    virtual void constructCBMessage(const QString &messageCode, int geographicalScope, const QString &updateNumber, const QString &channel,
    int language, const QString &content);
    virtual void constructSMSDatagram(int src, int dst, const QString &sender,  const QByteArray &data, const QByteArray &contentType);

    virtual void warning(const QString &title, const QString &message);

    virtual int convertString(const QString &number, int maxValue, int numChar, int base, bool *ok);

private:
    QSMSMessageList SMSList;
    SimRules *rules;
    bool simPresent;
};

class HardwareManipulatorFactory
{
public:
    virtual ~HardwareManipulatorFactory() {};
    inline virtual HardwareManipulator *create(SimRules *sr, QObject *p)
	    { return new HardwareManipulator(sr, p); }

    QString ruleFile() const { return ruleFilename; }
    void setRuleFile(const QString& filename) { ruleFilename = filename; }

private:
    QString ruleFilename;
};

class QVMMessage {
public:
    QVMMessage(int _id, const QString &from, bool pr) : id(_id),
        sender(from), priority(pr), lengthSecs(50), retentionDays(2) {}

    int id;
    QString sender;
    bool priority;
    int lengthSecs;
    int retentionDays;
};

#endif
