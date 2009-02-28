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

#ifndef QCONTENTSET_H__
#define QCONTENTSET_H__

#include <QMultiHash>
#include <qcontent.h>
#include <qcontentfilter.h>

#include <QAbstractListModel>
#include <qcontentsortcriteria.h>

class DirectoryScanner;
class QContentSetModel;
class QContentSetEngine;

typedef QContentSetEngine QContentSetPrivate;

class QTOPIA_EXPORT QContentSet : public QObject
{
    Q_OBJECT
public:

    enum UpdateMode
    {
        Asynchronous,
        Synchronous
    };

    explicit QContentSet( QObject *parent = 0 );
    explicit QContentSet( const QContentFilter &, QObject *parent = 0 );
    QContentSet( const QContentFilter &, const QStringList &, QObject *parent = 0 );
    QContentSet( QContentFilter::FilterType, const QString&, QObject *parent = 0 );
    QContentSet( QContentFilter::FilterType, const QString&, const QStringList &, QObject *parent = 0 );
    QContentSet( const QContentSet &, QObject *parent = 0 );

    QContentSet( UpdateMode mode, QObject *parent = 0 );
    QContentSet( const QContentFilter &, UpdateMode mode, QObject *parent = 0 );
    QContentSet( const QContentFilter &, const QContentSortCriteria &, UpdateMode mode, QObject *parent = 0 );

    virtual ~QContentSet();

    UpdateMode updateMode() const;

    enum Priority { LowPriority, NormalPriority, HighPriority };
    static void scan( const QString &path, Priority priority=NormalPriority );
    static void findDocuments(QContentSet* folder, const QString &mimefilter=QString());


    bool contains( const QContent &cl ) const;

    void add(const QContent&);
    void remove(const QContent&);
    void installContent();
    void uninstallContent();
    void clear();

    QContentList items() const;
    QContentIdList itemIds() const;

    bool isEmpty() const;
    void appendFrom( QContentSet& other );
    int count() const;
    QContent findExecutable( const QString& exec ) const;
    QContent findFileName( const QString& res ) const;
    QStringList types() const;

    QContentSet &operator=(const QContentSet&);

    void addCriteria( QContentFilter::FilterType kind, const QString &filter, QContentFilter::Operand operand );
    void addCriteria(const QContentFilter& filters, QContentFilter::Operand operand );
    void setCriteria( QContentFilter::FilterType kind, const QString &filter);
    void setCriteria(const QContentFilter& filters);
    void clearFilter();
    QContentFilter filter() const;

    void setSortOrder( const QStringList &sortOrder );
    QStringList sortOrder() const;

    void setSortCriteria( const QContentSortCriteria &criteria );
    QContentSortCriteria sortCriteria() const;

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

    static int count( const QContentFilter &filter );

signals:
    void contentAboutToBeInserted( int start, int end );
    void contentInserted();
    void contentAboutToBeRemoved( int start, int end );
    void contentRemoved();
    void contentChanged( int start, int end );
    void aboutToSort();
    void sorted();
    void changed(const QContentIdList &, QContent::ChangeType);
    void changed();

protected:
    void timerEvent(QTimerEvent *e);

private:
    friend class QContent;
    friend class ContentLinkPrivate;
    friend class QContentSetModel;
    friend class DirectoryScanner;
    friend class QtopiaSql;

    void init();

    QContentSetPrivate *d;
};

class QContentSetModelPrivate;

class QTOPIA_EXPORT QContentSetModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit QContentSetModel( const QContentSet *, QObject *parent=0 );
    virtual ~QContentSetModel();

    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
    QContent content( uint ) const;
    QContentId contentId( uint ) const;
    QContent content( const QModelIndex & index ) const;
    QContentId contentId( const QModelIndex & index ) const;

    void setSelectPermission( QDrmRights::Permission permission );
    QDrmRights::Permission selectPermission() const;

    void setMandatoryPermissions( QDrmRights::Permissions permissions );
    QDrmRights::Permissions mandatoryPermissions() const;

    bool updateInProgress() const;

signals:
    void updateStarted();
    void updateFinished();

private slots:
    void beginInsertContent( int start, int end );
    void endInsertContent();
    void beginRemoveContent( int start, int end );
    void endRemoveContent();
    void contentChanged( int start, int end );
    void clearContentSet();
    void doReset();
    void emitLayoutAboutToBeChanged();
    void emitLayoutChanged();
private:
    int rowForContentId(QContentId contentId);

    const QContentSet *contentSet;
    QContentSetModelPrivate *d;

    friend class QContentSet;
};

Q_DECLARE_USER_METATYPE(QContentSet);
Q_DECLARE_USER_METATYPE_ENUM(QContentSet::UpdateMode);

QTOPIA_EXPORT QDataStream &operator <<( QDataStream &ds, const QContentSet &set );
QTOPIA_EXPORT QDataStream &operator >>( QDataStream &ds, QContentSet &set );

#endif

