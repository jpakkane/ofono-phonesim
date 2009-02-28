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

#ifndef QDRMCONTENT_H
#define QDRMCONTENT_H

#include <QObject>
#include <QDateTime>
#include <qtimestring.h>
#include <qtopiaipcadaptor.h>
#include <qtopiaipcmarshal.h>
#include <qcontent.h>

class QDrmContentPrivate;

class QTOPIA_EXPORT QDrmContent : public QObject
{
    Q_OBJECT
public:

    enum RenderState{ Started, Stopped, Paused };

    enum LicenseOption
    {
        NoLicenseOptions = 0x00,
        Activate         = 0x01,
        Reactivate       = 0x02,
        Handover         = 0x04,
        Default          = Activate | Reactivate,
    };

    Q_DECLARE_FLAGS( LicenseOptions, LicenseOption );

    explicit QDrmContent( QDrmRights::Permission permission = QDrmRights::Unrestricted,
                          LicenseOptions options = Default, QObject *parent = 0 );

    virtual ~QDrmContent();

    RenderState renderState() const;

    QDrmRights::Permission permission() const;

    void setPermission( QDrmRights::Permission permission );

    LicenseOptions licenseOptions() const;

    void setLicenseOptions( LicenseOptions options );
    void enableLicenseOptions( LicenseOptions options );
    void disableLicenseOptions( LicenseOptions options );

    QWidget *focusWidget() const;
    void setFocusWidget( QWidget *focus );

    QContent content() const;

    static bool activate( const QContent &content, QWidget *focus = 0 );
    static bool canActivate( const QContent &content );

    static QStringList supportedTypes();
    static QList< QPair< QString, QString > > httpHeaders();

public slots:
    bool requestLicense( const QContent &content );
    void releaseLicense();

    void renderStarted();
    void renderStopped();
    void renderPaused();

signals:
    void licenseGranted( const QContent &content );
    void licenseDenied( const QContent &content );
    void renderStateChanged( const QDrmContent &content );
    void rightsExpired( const QDrmContent &content );

private:
    void init( QDrmRights::Permission permission, LicenseOptions options );
    bool getLicense( const QContent &content, QDrmRights::Permission );

    QDrmContentPrivate *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QDrmContent::LicenseOptions );

#endif
