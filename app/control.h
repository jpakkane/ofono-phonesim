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

#ifndef CONTROL_H
#define CONTROL_H

#include <qobject.h>
#include <qdialog.h>

#include <lib/hardwaremanipulator.h>
#include <attranslator.h>

class Ui_ControlBase;

class Control: public HardwareManipulator
{
Q_OBJECT

public:
    Control(const QString& ruleFile, QWidget *parent=0);
    bool shouldShow() const;

public slots:
    void handleFromData( const QString& );
    void handleToData( const QString& );
    void resetTranslator();

private slots:
    void sendSQ();
    void sendBC();
    void sendOPS();
    void sendREG();
    void sendCBM();
    void sendSMSMessage();
    void sendMGD();
    void selectFile();
    void sendSMSDatagram();
    void sendCall();
    void atChanged();

private:
    Ui_ControlBase *ui;
    AtTranslator *translator;
};

class ControlFactory : public HardwareManipulatorFactory
{
public:
    inline virtual HardwareManipulator *create(QWidget *parent)
        { return new Control(ruleFile(), parent); }
};


#endif
