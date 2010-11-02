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

#include "control.h"
#include <qpushbutton.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <QtGui/qmessagebox.h>
#include <qfiledialog.h>
#include <Qt>
#include <qbuffer.h>
#include <qtimer.h>
#include <qevent.h>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QtGui/QHeaderView>

#define TWO_BYTE_MAX 65535
#define FOUR_CHAR 4
#define FOUR_BYTE_MAX 0x7FFFFFFF
#define EIGHT_CHAR 8
#define HEX_BASE 16

#define SERVICE "org.ofono.phonesim"

ControlWidget::ControlWidget(const QString &ruleFile, Control *parent)
    : QWidget(), p(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QFileInfo info( ruleFile );
    QString specFile = info.absolutePath() + "/GSMSpecification.xml";
    if (!QFile::exists(specFile))
        specFile = QDir::currentPath() + "/GSMSpecification.xml";
    translator = new AtTranslator(specFile);

    ui = new Ui_ControlBase;
    ui->setupUi(this);

    script = new Script(this, ui);

    if (!bus.registerService(SERVICE)) {
        qWarning() << bus.lastError().message();
        exit(-1);
    }

    bus.registerObject("/", script, QDBusConnection::ExportAllSlots);

    connect(ui->hsSignalQuality, SIGNAL(valueChanged(int)), this, SLOT(sendSQ()));
    connect(ui->hsBatteryCharge, SIGNAL(valueChanged(int)), this, SLOT(sendBC()));
    connect(ui->hsBatteryCharging, SIGNAL(stateChanged(int)), this, SLOT(chargingChanged(int)));
    connect(ui->pbSelectOperator, SIGNAL(clicked()), this, SLOT(sendOPS()));
    connect(ui->pbRegistration, SIGNAL(clicked()), this, SLOT(sendREG()));
    connect(ui->pbSendCellBroadcast, SIGNAL(clicked()), this, SLOT(sendCBM()));
    connect(ui->pbSendSMSMessage, SIGNAL(clicked()), this, SLOT(sendSMSMessage()));
    connect(ui->pbFile, SIGNAL(clicked()), this, SLOT(selectFile()));
    connect(ui->pbSendSMSDatagram, SIGNAL(clicked()), this, SLOT(sendSMSDatagram()));
    connect(ui->pbIncomingCall, SIGNAL(clicked()), this, SLOT(sendCall()));
    connect(ui->openSpecButton, SIGNAL(clicked()), this, SLOT(resetTranslator()));
    connect(ui->atCheckBox, SIGNAL(clicked()), this, SLOT(atChanged()));
    connect(ui->pbAddMessage, SIGNAL(clicked()), this, SLOT(addVoicemail()));
    connect(ui->pbRemoveMessage, SIGNAL(clicked()), this, SLOT(delVoicemail()));
    connect(ui->pbNotifyUDH, SIGNAL(clicked()), this, SLOT(sendVMNotify()));
    connect(ui->pbNotifyUDHEnhanced, SIGNAL(clicked()), this, SLOT(sendEVMNotify()));
    connect(ui->pbSendUSSD, SIGNAL(clicked()), this, SLOT(sendUSSD()));
    connect(ui->pbCancelUSSD, SIGNAL(clicked()), this, SLOT(cancelUSSD()));
    connect(ui->cbSimInserted, SIGNAL(clicked()), this, SLOT(simInsertRemove()));
    connect(ui->pbStart, SIGNAL(clicked()), this, SLOT(simAppStart()));
    connect(ui->pbAbort, SIGNAL(clicked()), this, SLOT(simAppAbort()));

    QStringList headers;
    headers << "Sender" << "Priority" << "Notification Status";
    ui->twMessageList->setHorizontalHeaderLabels( headers );
    ui->twMessageList->verticalHeader()->hide();

    handleNewApp();

    show();
}

void ControlWidget::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

ControlWidget::~ControlWidget()
{
    delete script;
    delete ui;
    delete translator;
}

Control::Control(const QString& ruleFile, SimRules *sr, QObject *parent)
        : HardwareManipulator(sr, parent),
        widget(new ControlWidget(ruleFile, this))
{
    QList<QByteArray> proxySignals;
    proxySignals
        << SIGNAL(unsolicitedCommand(QString))
        << SIGNAL(command(QString))
        << SIGNAL(variableChanged(QString,QString))
        << SIGNAL(switchTo(QString))
        << SIGNAL(startIncomingCall(QString));

    foreach (QByteArray sig, proxySignals)
        connect(widget, sig, this, sig);
}

Control::~Control()
{
    delete widget;
}

void Control::setPhoneNumber( const QString &number )
{
    widget->setWindowTitle("Phonesim - Number: " + number);
}

void Control::warning( const QString &title, const QString &message )
{
    QMessageBox::warning(widget, title, message, "OK");
}

void ControlWidget::sendSQ()
{
    emit variableChanged("SQ",QString::number(ui->hsSignalQuality->value())+",99");
    emit unsolicitedCommand("+CSQ: "+QString::number(ui->hsSignalQuality->value())+",99");
}


void ControlWidget::sendBC()
{
    bool charging = ui->hsBatteryCharging->checkState() == Qt::Checked;
    emit variableChanged("BC",QString::number(charging)+","+QString::number(ui->hsBatteryCharge->value()));
    emit unsolicitedCommand("+CBC: "+QString::number(charging)+","+QString::number(ui->hsBatteryCharge->value()));
}

void ControlWidget::chargingChanged(int state)
{
    bool charging = state  == Qt::Checked;
    ui->hsBatteryCharge->setEnabled(!charging);
    emit variableChanged("BC",QString::number(charging)+","+QString::number(ui->hsBatteryCharge->value()));
    emit unsolicitedCommand("+CBC: "+QString::number(charging)+","+QString::number(ui->hsBatteryCharge->value()));
}

void ControlWidget::sendOPS()
{
    emit variableChanged("OP", ui->leOperatorName->text());
    emit unsolicitedCommand("+CREG: 5");
}

void ControlWidget::sendREG()
{
    QString commandString = "+CREG: "+QString::number(ui->cbRegistrationStatus->currentIndex());

    if ( ui->chkLocationInfo->checkState() == Qt::Checked ) {
        bool ok;

        p->convertString(ui->leLAC->text(),TWO_BYTE_MAX,FOUR_CHAR,HEX_BASE, &ok);
        if (!ok) {
            p->warning(tr("Invalid LAC"),
                    tr("Location Area Code must be 4 hex digits long"));
            return;
        }

        p->convertString(ui->leCellID->text(),FOUR_BYTE_MAX,EIGHT_CHAR,HEX_BASE, &ok);
        if (!ok) {
            p->warning(tr("Invalid Cell ID"),
                    tr("Cell ID must be 8 hex digits long"));
            return;
        }

        commandString.append(",\"" + ui->leLAC->text() + "\",\"" + ui->leCellID->text() + "\"");
    }

    emit unsolicitedCommand(commandString);
}

void ControlWidget::sendCBM()
{
    QString pdu = p->constructCBMessage(ui->leMessageCode->text(),ui->cbGeographicalScope->currentIndex(),
                       ui->leUpdateNumber->text(),ui->leChannel->text(),ui->leScheme->text(),
                       ui->cbLanguage->currentIndex(),ui->leNumPages->text(),ui->lePage->text(),
                       ui->teContent->toPlainText());

    emit unsolicitedCommand(QString("+CBM: ")+QString::number(pdu.length()/2)+'\r'+'\n'+ pdu);
}

void ControlWidget::sendSMSMessage()
{
    if (ui->leSMSClass->text().isEmpty()) {
        p->warning(tr("Invalid Sender"),
                tr("Sender must not be empty"));
        return;
    }

    if (ui->leSMSServiceCenter->text().isEmpty() || ui->leSMSServiceCenter->text().contains(QRegExp("\\D"))) {
        p->warning(tr("Invalid Service Center"),
                tr("Service Center must not be empty and contain "
                   "only digits"));
        return;
    }
    p->constructSMSMessage(ui->leSMSClass->text().toInt(), ui->leMessageSender->text(), ui->leSMSServiceCenter->text(), ui->teSMSText->toPlainText());
}

void ControlWidget::sendMGD()
{
    emit command("AT+CMGD=1");
}

void ControlWidget::selectFile()
{
    ui->leFile->setText(QFileDialog::getOpenFileName(this, "Select File", "/home", "Files (*.*)"));
}

void ControlWidget::sendSMSDatagram()
{
    QString dstPortStr = ui->leDstPort->text();
    if ( dstPortStr.contains(QRegExp("\\D")) ) {
        p->warning(tr("Invalid Port"), tr("Port number can contain only digits" ));
        return;
    }
    int dst = dstPortStr.toInt();

    QString srcPortStr = ui->leSrcPort->text();
    if ( srcPortStr.contains(QRegExp("\\D")) ) {
        p->warning(tr("Invalid Port"), tr("Port number can contain only digits" ));
        return;
    }
    int src = srcPortStr.toInt();

    QString sender = ui->leDatagramSender->text();

    //obtain data from either file or text edit
    QFile file(ui->leFile->text());
    QByteArray data;
    if ( ui->chkAppData->checkState() ==Qt::Checked ) {
        if ( !file.open(QIODevice::ReadOnly) ) {
            p->warning(tr("Invalid File"), tr("File could not be opened"));
            return;
        }
        data = file.readAll();
    } else {
       data = ui->teAppData->toPlainText().toUtf8();
    }

    //obtain Content-Type if required
    QByteArray contentType;
    if ( ui->chkContentType->checkState() == Qt::Checked ) {
        QString contentTypeStr = ui->leContentType->text();
        if ( contentTypeStr.length() == 0 ) {
            p->warning( tr("Invalid ContentType"),
                     tr("Please check the Content Type") );
            return;
        }
        contentType = contentTypeStr.toUtf8();
    }

    //construct and place SMS datagram in SMSList
    p->constructSMSDatagram(src, dst, sender, data, contentType);
}

void ControlWidget::sendCall()
{
    emit startIncomingCall( ui->leCaller->text() );
}

void ControlWidget::handleFromData( const QString& cmd )
{
    ui->atViewer->append(cmd);
    /*QStringList dataList = cmd.split("\n");
    QString dataItem;
    foreach( dataItem, dataList ){
        if( dataItem != "" ){
            ui->atViewer->append(translator->translate(dataItem));
            ui->atViewer->append(dataItem);
        }
    }*/
}

void Control::handleFromData( const QString& cmd )
{
    widget->handleFromData(cmd);
}

void ControlWidget::handleToData( const QString& cmd )
{
    QStringList dataList = cmd.split("\n");
    QString dataItem;
    foreach( dataItem, dataList ){
        if( dataItem != "" ){
            ui->atViewer->append( dataItem + " : " + translator->translateCommand(dataItem) );
        }
    }

    if ( cmd.startsWith("AT+CUSD=") ) {
        int comma = cmd.indexOf( QChar(',') );
        if ( comma >= 0 ) {
            QString content = cmd.mid(comma + 1);
            content.remove( QChar('"') );
            ui->lblResponse->setText( content );
        }

        if ( cmd.startsWith("AT+CUSD=2") )
            ui->lblResponse->setText( "" );
    }
}

void Control::handleToData( const QString& cmd )
{
    widget->handleToData(cmd);
}

void ControlWidget::resetTranslator()
{
    QString fileName = QFileDialog::getOpenFileName(this,
     tr("Open Specification File"), QDir::homePath(), tr("Specification files (*.xml)"));
    if(fileName != 0)
        translator->resetSpecification(fileName);
}

void ControlWidget::atChanged()
{
    ui->atGroupBox->setVisible( ui->atCheckBox->isChecked() );
}

void ControlWidget::sendVMNotify( int type )
{
    QList<QVMMessage> received;
    QList<QVMMessage> deleted;

    for ( QList<VoicemailItem>::iterator i = mailbox.begin();
            i != mailbox.end(); ++i ) {
        QVMMessage msg( i->id, i->sender.text(), i->priority.text() == "Urgent" );

        if ( i->notifyReceived ) {
            received.append( msg );

            i->notifyReceived = false;
            i->pending.setText( "" );
        }

        if ( i->notifyDeleted ) {
            int position = i - mailbox.begin();

            deleted.append( msg );

            ui->twMessageList->takeItem( position, 0 );
            ui->twMessageList->takeItem( position, 1 );
            ui->twMessageList->takeItem( position, 2 );
            ui->twMessageList->model()->removeRow( position );

            mailbox.erase( i );
        }
    }

    return p->sendVMNotify( type, mailbox.size(), received, deleted,
            ui->leServiceDiallingNumber->text() );
}

void ControlWidget::sendEVMNotify()
{
    return sendVMNotify( 1 );
}

void ControlWidget::addVoicemail()
{
    int position = ui->twMessageList->rowCount();

    mailbox.append( VoicemailItem(
            ui->leSenderNumber->text(),
            ui->chkPriority->isChecked() ) );
    ui->leSenderNumber->setText( "" );
    ui->chkPriority->setChecked( false );

    ui->twMessageList->model()->insertRow( position );
    ui->twMessageList->setItem( position, 0, &mailbox.last().sender );
    ui->twMessageList->setItem( position, 1, &mailbox.last().priority );
    ui->twMessageList->setItem( position, 2, &mailbox.last().pending );
}

void ControlWidget::delVoicemail()
{
    foreach ( QTableWidgetSelectionRange range,
            ui->twMessageList->selectedRanges() ) {
        for ( int i = range.topRow(); i <= range.bottomRow(); i ++ ) {
            VoicemailItem &item = mailbox[i];

            item.notifyDeleted = true;
            item.pending.setText( "DELETE" );
        }
    }
}

int ControlWidget::VoicemailItem::nextId;

ControlWidget::VoicemailItem::VoicemailItem( const QString &from, bool urgent )
    : sender( from ), priority( urgent ? "Urgent" : "" ), pending( "NEW" )
{
    notifyReceived = true;
    notifyDeleted = false;

    id = nextId++;
}

void ControlWidget::sendUSSD()
{
    bool ask = ui->chkAskResponse->isChecked();
    QString text = ui->leUSSDString->text();

    p->sendUSSD( false, ask, text );

    ui->lblResponse->setText( "" );
    ui->leUSSDString->setText( "" );
}

void ControlWidget::cancelUSSD()
{
    p->sendUSSD( true, false, "" );

    ui->lblResponse->setText( "" );
    ui->leUSSDString->setText( "" );
}

void ControlWidget::simInsertRemove()
{
    p->setSimPresent( ui->cbSimInserted->isChecked() );
}

void ControlWidget::handleNewApp()
{
    ui->lblApplicationName->setText( "Current application: " +
            p->getSimAppName() );
}

void Control::handleNewApp()
{
    widget->handleNewApp();
}

void ControlWidget::simAppStart()
{
    p->simAppStart();
}

void ControlWidget::simAppAbort()
{
    p->simAppAbort();
}

Script::Script(QObject *obj, Ui_ControlBase *ui) : QDBusAbstractAdaptor(obj)
{
    /* Export tabs to be accessed by script */
    QScriptValue qsTab = engine.newQObject(ui->tab);
    engine.globalObject().setProperty("tabRegistration", qsTab);

    QScriptValue qsTab2 = engine.newQObject(ui->tab_2);
    engine.globalObject().setProperty("tabCBM", qsTab2);

    QScriptValue qsTab3 = engine.newQObject(ui->tab_3);
    engine.globalObject().setProperty("tabSMS", qsTab3);

    QScriptValue qsTab4 = engine.newQObject(ui->tab_4);
    engine.globalObject().setProperty("tabVoiceMail", qsTab4);

    QScriptValue qsTab5 = engine.newQObject(ui->tab_5);
    engine.globalObject().setProperty("tabUSSD", qsTab5);

    QScriptValue qsTab6 = engine.newQObject(ui->tab_6);
    engine.globalObject().setProperty("tabSIM", qsTab6);
}

void Script::SetPath(const QString &path, const QDBusMessage &msg)
{
    QDir dir(path);
    if (!dir.exists()) {
        QDBusMessage reply = msg.createErrorReply(SERVICE "Error.PathNotFound", "Path doesn't exist");
        QDBusConnection::sessionBus().send(reply);
        return;
    }

    dirPath = path;
}

QString Script::GetPath()
{
    return dirPath;
}

QString Script::Run(const QString &name, const QDBusMessage &msg)
{
    QString fileName;

    if (dirPath.endsWith('/'))
        fileName = dirPath + name;
    else
        fileName = dirPath + "/" + name;

    QFile scriptFile(fileName);

    if (!scriptFile.open(QIODevice::ReadOnly)) {
        QDBusMessage reply = msg.createErrorReply(SERVICE ".Error.FileNotFound", "Script file doesn't exist");
        QDBusConnection::sessionBus().send(reply);
        return QString();
    }

    QTextStream stream(&scriptFile);
    stream.setCodec("UTF-8");
    QString contents = stream.readAll();
    scriptFile.close();

    QScriptValue qsScript = engine.evaluate(contents);
    if (qsScript.isError()) {
        QString info = fileName + ", line " + qsScript.property("lineNumber").toString() + ", " + qsScript.toString();
        QDBusMessage reply = msg.createErrorReply(SERVICE "Error.ScriptExecError", info);
        QDBusConnection::sessionBus().send(reply);
        return QString();
    }

    /*
     * If this QScriptValue is an object, calling this function has side effects on the script engine,
     * since the engine will call the object's toString() function (and possibly valueOf()) in an attempt
     * to convert the object to a primitive value (possibly resulting in an uncaught script exception).
     */
    if (qsScript.isObject() || qsScript.isUndefined())
        return QString();

    return qsScript.toString();
}
