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

#include "control.h"
#include "ui_controlbase.h"
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

#define TWO_BYTE_MAX 65535
#define FOUR_CHAR 4
#define HEX_BASE 16

Control::Control(const QString& ruleFile, QWidget *parent)
        : HardwareManipulator(parent)
{
    QFileInfo info( ruleFile );
    QString specFile = info.absolutePath() + "/GSMSpecification.xml";
    if (!QFile::exists(specFile))
        specFile = QDir::currentPath() + "/GSMSpecification.xml";
    translator = new AtTranslator(specFile);
    ui = new Ui_ControlBase;
    ui->setupUi(this);
    connect(ui->hsSignalQuality, SIGNAL(valueChanged(int)), this, SLOT(sendSQ()));
    connect(ui->hsBatteryCharge, SIGNAL(valueChanged(int)), this, SLOT(sendBC()));
    connect(ui->pbSelectOperator, SIGNAL(clicked()), this, SLOT(sendOPS()));
    connect(ui->pbRegistration, SIGNAL(clicked()), this, SLOT(sendREG()));
    connect(ui->pbSendCellBroadcast, SIGNAL(clicked()), this, SLOT(sendCBM()));
    connect(ui->pbSendSMSMessage, SIGNAL(clicked()), this, SLOT(sendSMSMessage()));
    connect(ui->pbFile, SIGNAL(clicked()), this, SLOT(selectFile()));
    connect(ui->pbSendSMSDatagram, SIGNAL(clicked()), this, SLOT(sendSMSDatagram()));
    connect(ui->pbIncomingCall, SIGNAL(clicked()), this, SLOT(sendCall()));
    connect(ui->openSpecButton, SIGNAL(clicked()), this, SLOT(resetTranslator()));
    connect(ui->atCheckBox, SIGNAL(clicked()), this, SLOT(atChanged()));
}

bool Control::shouldShow() const
{
    return true;
}

void Control::sendSQ()
{
    emit variableChanged("SQ",QString::number(ui->hsSignalQuality->value())+",99");
    emit unsolicitedCommand("+CSQ: "+QString::number(ui->hsSignalQuality->value())+",99");
}


void Control::sendBC()
{
    emit variableChanged("BC","1,"+QString::number(ui->hsSignalQuality->value()));
    emit unsolicitedCommand("+CBC: 0,"+QString::number(ui->hsBatteryCharge->value()));
}

void Control::sendOPS()
{
    emit variableChanged("OP", ui->leOperatorName->text());
    emit unsolicitedCommand("+CREG: 5");
}

void Control::sendREG()
{
    QString commandString = "+CREG: "+QString::number(ui->cbRegistrationStatus->currentIndex());

    if ( ui->chkLocationInfo->checkState() == Qt::Checked ) {
        bool ok;

        int LAC = convertString(ui->leLAC->text(),TWO_BYTE_MAX,FOUR_CHAR,HEX_BASE, &ok);
        if (!ok) {
            QMessageBox::warning(this, tr("Invalid LAC"),
                                 tr("Location Area Code must be 4 hex digits long"),
                                 "OK");
            return;
        }

        int cellID = convertString(ui->leCellID->text(),TWO_BYTE_MAX,FOUR_CHAR,HEX_BASE, &ok);
        if (!ok) {
            QMessageBox::warning(this, tr("Invalid Cell ID"),
                                 tr("Cell ID must be 4 hex digits long"),
                                 "OK");
            return;
        }

        commandString.append("," + QString::number(LAC) + "," + QString::number(cellID));
    }

    emit unsolicitedCommand(commandString);
}

void Control::sendCBM()
{
    QString pdu = constructCBMessage(ui->leMessageCode->text(),ui->cbGeographicalScope->currentIndex(),
                       ui->leUpdateNumber->text(),ui->leChannel->text(),ui->leScheme->text(),
                       ui->cbLanguage->currentIndex(),ui->leNumPages->text(),ui->lePage->text(),
                       ui->teContent->toPlainText());

    emit unsolicitedCommand(QString("+CBM: ")+QString::number(pdu.length()/2)+'\r'+'\n'+ pdu);
}

void Control::sendSMSMessage()
{
    int originalCount = HardwareManipulator::getSMSList().count();
    constructSMSMessage(ui->leMessageSender->text(), ui->leSMSServiceCenter->text(), ui->teSMSText->toPlainText());

    int count = HardwareManipulator::getSMSList().count();
    if ( count > originalCount )
        emit unsolicitedCommand("+CMTI: \"SM\","+QString::number( HardwareManipulator::getSMSList().count()));
}

void Control::sendMGD()
{
    emit command("AT+CMGD=1");
}

void Control::selectFile()
{
    ui->leFile->setText(QFileDialog::getOpenFileName(this, "Select File", "/home", "Files (*.*)"));
}

void Control::sendSMSDatagram()
{

    QString portStr = ui->lePort->text();
    if ( portStr.contains(QRegExp("\\D")) ) {
       QMessageBox::warning(this, tr("Invalid Port"),
        tr("Port number can contain only digits" ),
        "OK");
        return;
    }
    int port = portStr.toInt();

    QString sender = ui->leDatagramSender->text();

    //obtain data from either file or text edit
    QFile file(ui->leFile->text());
    QByteArray data;
    if ( ui->chkAppData->checkState() ==Qt::Checked ) {
        if ( !file.open(QIODevice::ReadOnly) ) {
            QMessageBox::warning(this, tr("Invalid File"),
            tr("File could not be opened"),
            "OK");
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
            QMessageBox::warning(this, tr("Invalid ContentType"),
                                 tr("Please check the Content Type"),
                                    "OK");
            return;
        }
        contentType = contentTypeStr.toUtf8();
    }

    //construct and place SMS datagram in SMSList
    int originalCount = HardwareManipulator::getSMSList().count();
    constructSMSDatagram(port, sender, data, contentType);

    //if datagram successfully placed in SMSList emit CMTI
    int count = HardwareManipulator::getSMSList().count();
    if ( count > originalCount )
        emit unsolicitedCommand("+CMTI: \"SM\","+QString::number( HardwareManipulator::getSMSList().count()));
}

void Control::sendCall()
{
    emit startIncomingCall( ui->leCaller->text() );
}

void Control::handleFromData( const QString& cmd )
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

void Control::handleToData( const QString& cmd )
{
    QStringList dataList = cmd.split("\n");
    QString dataItem;
    foreach( dataItem, dataList ){
        if( dataItem != "" ){
            ui->atViewer->append( dataItem + " : " + translator->translateCommand(dataItem) );
        }
    }
}

void Control::resetTranslator()
{
    QString fileName = QFileDialog::getOpenFileName(this,
     tr("Open Specification File"), QDir::homePath(), tr("Specification files (*.xml)"));
    if(fileName != 0)
        translator->resetSpecification(fileName);
}

void Control::atChanged()
{
    ui->atGroupBox->setVisible( ui->atCheckBox->isChecked() );
}

