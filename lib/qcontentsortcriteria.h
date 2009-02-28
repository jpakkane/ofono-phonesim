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
#ifndef QCONTENTSORT_H
#define QCONTENTSORT_H
#include <qcontentfilter.h>
#include <QtGlobal>

class QContentSortCriteriaPrivate;

class QTOPIA_EXPORT QContentSortCriteria
{
public:

    enum Attribute
    {
        Name,
        MimeType,
        Property,
        FileName,
        LastUpdated
    };

    QContentSortCriteria();
    QContentSortCriteria( Attribute attribute, Qt::SortOrder order = Qt::AscendingOrder );
    QContentSortCriteria( Attribute attribute, const QString &scope, Qt::SortOrder order = Qt::AscendingOrder );
    QContentSortCriteria( QContent::Property property, Qt::SortOrder order = Qt::AscendingOrder );
    QContentSortCriteria( const QContentSortCriteria &other );

    ~QContentSortCriteria();

    QContentSortCriteria &operator =( const QContentSortCriteria &other );

    bool operator ==( const QContentSortCriteria &other ) const;
    bool operator !=( const QContentSortCriteria &other ) const;

    void addSort( Attribute attribute, Qt::SortOrder order = Qt::AscendingOrder );
    void addSort( Attribute attribute, const QString &scope, Qt::SortOrder order = Qt::AscendingOrder );
    void addSort( QContent::Property property, Qt::SortOrder order = Qt::AscendingOrder );

    void clear();

    int sortCount() const;

    Attribute attribute( int index ) const;
    QString scope( int index ) const;
    Qt::SortOrder order( int index ) const;

    int compare( const QContent &left, const QContent &right ) const;

    bool lessThan( const QContent &left, const QContent &right ) const;
    bool greaterThan( const QContent &left, const QContent &right ) const;

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

private:
    QSharedDataPointer< QContentSortCriteriaPrivate > d;
};

Q_DECLARE_USER_METATYPE(QContentSortCriteria);
Q_DECLARE_USER_METATYPE_ENUM(QContentSortCriteria::Attribute);

#endif
