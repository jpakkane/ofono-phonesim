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
#ifndef QMIMETYPE_H
#define QMIMETYPE_H

#include <qtopiaglobal.h>
#include <qstringlist.h>
#include <qicon.h>
#include <qlist.h>
#include <QMutex>

#include <qdrmcontent.h>
#include <qcontentset.h>

class QStringList;
class QMimeTypeData;

class QTOPIA_EXPORT QMimeType
{
public:
    QMimeType();
    explicit QMimeType( const QString& ext_or_id );
    explicit QMimeType( const QContent& );
    QMimeType( const QMimeType &other );

    QMimeType &operator =( const QMimeType &other );

    bool operator ==( const QMimeType &other ) const;

    enum IconType
    {
        Default,
        DrmValid,
        DrmInvalid
    };

    QString id() const;
    QString description() const;
    QIcon icon( IconType iconType = Default ) const;

    QString extension() const;
    QStringList extensions() const;
    QContentList applications() const;
    QContent application() const;

    QDrmRights::Permission permission() const;
    QList<QDrmRights::Permission> permissions() const;

    static void updateApplications();

    bool isNull() const;

    static QMimeType fromId( const QString &mimeId );
    static QMimeType fromExtension( const QString &extension );
    static QMimeType fromFileName( const QString &fileName );

    static void addAssociation(const QString& mimeType, const QString& application, const QString& icon, QDrmRights::Permission permission);
    static void removeAssociation(const QString& mimeType, const QString& application);
    static QContentList applicationsFor(const QContent&);
    static QContentList applicationsFor(const QMimeType&);
    static QContent defaultApplicationFor(const QContent&);
    static QContent defaultApplicationFor(const QMimeType&);
    static void setDefaultApplicationFor(const QString&, const QContent&);

private:
    static void clear();
    static void loadExtensions();
    static void loadExtensions(const QString&);
    void init( const QString& ext_or_id );
    static QMimeTypeData data(const QString& id);
    QString mimeId;
    static QMutex staticsGuardMutex;

    friend class QMimeTypeData;
    friend class QtopiaApplication;
};

#endif
