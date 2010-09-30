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

#ifndef CONTROL_H
#define CONTROL_H

#include <hardwaremanipulator.h>
#include <QtDBus/QtDBus>
#include <QtScript>
#include "ui_controlbase.h"
#include "attranslator.h"

class Control;

class Script: public QDBusAbstractAdaptor
{
Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.phonesim.Script")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.phonesim.Script\">\n"
"    <method name=\"SetPath\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"    </method>\n"
"    <method name=\"GetPath\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"path\"/>\n"
"    </method>\n"
"    <method name=\"Run\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"name\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"result\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")

public:
    Script(QObject *obj, Ui_ControlBase *ui);

public slots:
    void SetPath(const QString &path, const QDBusMessage &msg);
    QString GetPath();
    QString Run(const QString &name, const QDBusMessage &msg);

private:
    QString dirPath;
    QScriptEngine engine;
};

class ControlWidget : public QWidget
{
Q_OBJECT
public:
    ControlWidget( const QString&, Control*);
    ~ControlWidget();

    void handleFromData( const QString& );
    void handleToData( const QString& );
    void handleNewApp();

private slots:
    void sendSQ();
    void sendBC();
    void chargingChanged(int state);
    void sendOPS();
    void sendREG();
    void sendCBM();
    void sendSMSMessage();
    void sendMGD();
    void selectFile();
    void sendSMSDatagram();
    void sendCall();
    void atChanged();
    void resetTranslator();
    void addVoicemail();
    void delVoicemail();
    void sendVMNotify( int type = 0 );
    void sendEVMNotify();
    void sendUSSD();
    void cancelUSSD();
    void simInsertRemove();
    void simAppStart();
    void simAppAbort();

signals:
    void unsolicitedCommand(const QString &);
    void command(const QString &);
    void variableChanged(const QString &, const QString &);
    void switchTo(const QString &);
    void startIncomingCall(const QString &);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui_ControlBase *ui;
    Script *script;
    Control *p;
    AtTranslator *translator;

    class VoicemailItem {
    public:
        VoicemailItem( const QString &sender, bool urgent );

        QTableWidgetItem sender;
        QTableWidgetItem priority;
        QTableWidgetItem pending;

        bool notifyReceived;
        bool notifyDeleted;
        int id;

        static int nextId;
    };
    QList<VoicemailItem> mailbox;
};

class Control: public HardwareManipulator
{
Q_OBJECT

public:
    Control(const QString& ruleFile, SimRules *sr, QObject *parent=0);
    virtual ~Control();

public slots:
    void handleFromData( const QString& );
    void handleToData( const QString& );
    void setPhoneNumber( const QString& );
    void handleNewApp();

protected:
    virtual void warning( const QString&, const QString& );

private:
    ControlWidget *widget;
    friend class ControlWidget;
};

class ControlFactory : public HardwareManipulatorFactory
{
public:
    inline virtual HardwareManipulator *create(SimRules *sr, QObject *parent)
        { return new Control(ruleFile(), sr, parent); }
};


#endif
