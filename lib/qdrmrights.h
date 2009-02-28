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

#ifndef QDRMRIGHTS_H
#define QDRMRIGHTS_H

#include <qtopiaglobal.h>
#include <qtopiaipcmarshal.h>
#include <QSharedData>
#include <QStringList>
#include <QFlags>
#include <QPair>

class QDrmRightsPrivate;
class QDrmRightsConstraintPrivate;


class QTOPIA_EXPORT QDrmRights
{
public:
    enum Status
    {
        Invalid,
        Valid,
        ValidInFuture
    };

    enum Permission
    {
        NoPermissions     = 0x0000,
        Play              = 0x0001,
        Display           = 0x0002,
        Execute           = 0x0004,
        Print             = 0x0008,
        Export            = 0x0010,
        Distribute        = 0x0020,
        Preview           = 0x0040,
        Automated         = 0x0080,
        BrowseContents    = 0x0100,
        Unrestricted      = 0x7F00 | Play | Display | Execute | Print | Export | Distribute | Preview | Automated | BrowseContents,
        InvalidPermission = 0x8000
    };

    Q_DECLARE_FLAGS( Permissions, Permission );

    class QTOPIA_EXPORT Constraint
    {
    public:
        Constraint();
        Constraint( const QString &name, const QVariant &value );
        Constraint( const QString &name, const QVariant &value, const QList< QPair< QString, QVariant > > &attributes );
        Constraint( const Constraint &other );
        ~Constraint();

        Constraint &operator =( const Constraint &other );

        QString name() const;
        QVariant value() const;

        int attributeCount() const;
        QString attributeName( int index ) const;
        QVariant attributeValue( int index ) const;

        template <typename Stream> void serialize(Stream &stream) const;
        template <typename Stream> void deserialize(Stream &stream);

    private:
        QSharedDataPointer< QDrmRightsConstraintPrivate > d;
    };

    typedef QList< Constraint > ConstraintList;

    QDrmRights();
    QDrmRights( Permission permission, Status status, const ConstraintList &constraints = ConstraintList() );
    QDrmRights( const QDrmRights &other );
    ~QDrmRights();

    QDrmRights &operator =( const QDrmRights &other );

    Permission permission() const;

    Status status() const;

    ConstraintList constraints() const;

    static QString toString( Permission permission );
    static QString toString( Permission permission, Status status );

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

private:
    QSharedDataPointer< QDrmRightsPrivate > d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDrmRights::Permissions);
Q_DECLARE_USER_METATYPE_ENUM(QDrmRights::Permission);
Q_DECLARE_USER_METATYPE_ENUM(QDrmRights::Permissions);
Q_DECLARE_USER_METATYPE_ENUM(QDrmRights::Status);
Q_DECLARE_USER_METATYPE(QDrmRights);
Q_DECLARE_USER_METATYPE(QDrmRights::Constraint);
Q_DECLARE_USER_METATYPE_TYPEDEF(QDrmRightsConstraintList,QDrmRights::ConstraintList);

#endif
