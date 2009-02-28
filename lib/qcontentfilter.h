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

#ifndef QCONTENTFILTER_H__
#define QCONTENTFILTER_H__

#include <QSharedData>
#include <qcontent.h>

class QCategoryFilter;
class QContentFilterPrivate;

class QTOPIA_EXPORT QContentFilter
{
public:

    enum FilterType {
        Location,       // storage media, mount, website etc
        Role,           // Applications, Documents
        MimeType,       // image/gif etc
        Directory,      // directory name
        Category,       // category string from category subsystem
        DRM,            // drm permissions
        Property,      // synthetic attributes eg mtime, event
        FileName,
        Name,
        QtopiaType = Role,
        Synthetic = Property,
        Unknown=100     // probable error
    };

    enum Operand
    {
        NoOperand,
        And,
        Or
    };

    QContentFilter();
    QContentFilter( const QContentFilter &other );
    QContentFilter( FilterType type, const QString &argument );
    QContentFilter( QContent::Property property, const QString &value );
    QContentFilter( QContent::Role role );
    QContentFilter( const QMimeType &mime );
    QContentFilter( const QCategoryFilter &filter );
    ~QContentFilter();

    QContentFilter &operator =( const QContentFilter &other );

    QContentFilter operator ~() const;

    QContentFilter operator &( const QContentFilter &other ) const;
    QContentFilter operator |( const QContentFilter &other ) const;

    QContentFilter &operator &=( const QContentFilter &other );
    QContentFilter &operator |=( const QContentFilter &other );

    bool operator ==( const QContentFilter &other ) const;
    bool operator !=( const QContentFilter &other ) const;

    Operand operand() const;

    bool negated() const;

    bool isValid() const;

    void clear();

    QList< FilterType > types() const;

    QStringList arguments( FilterType type ) const;

    QList< QContentFilter > subFilters() const;

    bool test( const QContent &content ) const;

    QStringList argumentMatches( FilterType type, const QString &scope ) const;

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

    static QContentFilter mimeType( const QString &mimeType );
    static QContentFilter category( const QString &categoryId );
    static QContentFilter name( const QString &name );
    static QContentFilter fileName( const QString &fileName );
    static QContentFilter property( const QString &group, const QString &key, const QString &value );
    static QContentFilter property( const QString &key, const QString &value );

private:
    QSharedDataPointer< QContentFilterPrivate > d;

    static bool test( const QContent &content, QContentFilter::FilterType type, const QString &argument );
};

Q_DECLARE_USER_METATYPE(QContentFilter);
Q_DECLARE_USER_METATYPE_ENUM(QContentFilter::Operand);
Q_DECLARE_USER_METATYPE_ENUM(QContentFilter::FilterType);

QTOPIA_EXPORT QDebug operator <<( QDebug debug, const QContentFilter &filter );

#endif
