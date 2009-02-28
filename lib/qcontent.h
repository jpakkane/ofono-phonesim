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

#ifndef QCONTENT_H__
#define QCONTENT_H__

#include <qtopiaglobal.h>

#include <QIcon>
#include <QFileInfo>
#include <QSharedDataPointer>
#include <QIODevice>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <qtopiaipcadaptor.h>
#include <qtopiaipcmarshal.h>
#include <inheritedshareddata.h>
#include <qdrmrights.h>
#include <QVariant>

class QContentSet;
class QContentSetModel;
class QtopiaApplication;
class QContentFilter;
class QContentEngine;
class QContentStore;

typedef QContentEngine QContentPrivate;

class QMimeType;

typedef quint32 QtopiaDatabaseId;
// sqlite 3 and mysql both use 8 bytes ints as records
typedef QPair<QtopiaDatabaseId, quint64> QContentId;
typedef QList<QContentId> QContentIdList;

class QTOPIA_EXPORT QContent
{
public:

    static const QContentId InvalidId;

    QContent();
    QContent( QContentId );
    QContent( const QString &fileName, bool store=true );
    QContent( const QFileInfo &fi, bool store=true );
    QContent( const QContent &copy );
    virtual ~QContent();

    bool isValid(bool force=false) const;

    QContentId id() const;
    QString name() const;
    void setName(const QString& docname);
    QString untranslatedName() const;
    QString type() const;
    void setType(const QString& doctype);
    qint64 size() const;     // can be -1 if invalid

    QIcon icon() const;
    QIcon icon( QDrmRights::Permission permission ) const;
    void setIcon(const QString& iconpath);

    static QContentId install( const QFileInfo & );
    static void uninstall( QContentId );
    static void installBatch( const QList<QFileInfo> & );
    static void uninstallBatch( const QList<QContentId> & );
    static void clearErrors();
    static QContentId execToContent( const QString& );

    enum ChangeType { Added, Removed, Updated };
    bool commit(){ ChangeType ctype=Updated; return commit(ctype); }

    enum DrmState {
        Unprotected,
        Protected
    };

    DrmState drmState() const;

    enum Role {
        UnknownUsage,
        Document,
        Data,
        Application,
        Folder
    };

    typedef Role UsageMode;

    Role role() const;
    UsageMode usageMode() const;

    void setRole( Role role );

    QString fileName() const;
    QString comment() const;
    void setComment( const QString &commment );
    QString errorString() const;
    bool error() const;
    QDrmRights::Permissions permissions( bool force = true ) const;
    QDrmRights rights( QDrmRights::Permission permisssion ) const;
    QString file() const;
    void setFile( const QString& filename );
    QString linkFile() const;
    void setLinkFile( const QString& filename );
    bool fileKnown() const;
    bool linkFileKnown() const {return !linkFile().isEmpty();}
    QList< QDrmRights::Permission > mimeTypePermissions() const;
    QStringList mimeTypeIcons() const;
    QStringList mimeTypes() const;
    void setMimeTypes( const QStringList &mimeTypes );
    void setMimeTypes( const QStringList &mimeTypes, const QStringList& mimeTypeIcons, const QList< QDrmRights::Permission >& permissions);
    void execute() const;
    void execute(const QStringList& args) const;
    QString iconName() const;

    bool setMedia( const QString &media );
    QString media() const;

    QStringList categories() const;
    void setCategories( const QStringList &v );

    bool operator==(const QContent &other) const;

    QContent &operator=(const QContent &other);

    bool isPreloaded() const;
    QString executableName() const;
    void setExecutableName(const QString &exec);

    bool isDocument() const;

    enum Property
    {
        Album,
        Artist,
        Author,
        Composer,
        ContentUrl,
        Copyright,
        CopyrightUrl,
        Description,
        Genre,
        InformationUrl,
        PublisherUrl,
        RightsIssuerUrl,
        Track,
        Version
    };

    void setProperty(const QString& key, const QString& value, const QString &group=QString());
    void setProperty( Property key, const QString &value );
    QString property(const QString& key, const QString &group=QString()) const;
    QString property( Property key ) const;

    void removeFiles();
    void removeLinkFile();

    QIODevice *open(QIODevice::OpenMode);
    QIODevice *open() const;
    bool save(const QByteArray &data);
    bool load(QByteArray &data) const;
    bool copyContent(const QContent &from);
    bool copyTo(const QString &newPath);
    bool moveTo(const QString &newPath);

    QDateTime lastUpdated() const;

    static QString propertyKey( Property property );

    explicit QContent( QContentEngine *engine );

    bool isDetached() const;

    bool isNull() const;

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);

    enum DocumentSystemConnection
    {
        DocumentSystemDirect,
        DocumentSystemClient
    };

    static DocumentSystemConnection documentSystemConnection();
    static bool setDocumentSystemConnection( DocumentSystemConnection connection );

protected:
    bool commit(ChangeType &change);

private:
    InheritedSharedDataPointer<QContentPrivate> d;

    static void updateSets(QContentId id, QContent::ChangeType c);

    friend class QtopiaApplication;
    friend class QDrmContent;
    friend class QContentStore;
    friend QDebug &operator<<(QDebug &debug, const QContent &content);
};

template <> inline bool qIsDetached<QContent>(QContent &t) { return t.isDetached(); }

typedef QList<QContent> QContentList;

Q_DECLARE_METATYPE(QContentIdList);
Q_DECLARE_METATYPE(QContentList);

Q_DECLARE_USER_METATYPE(QContent);
Q_DECLARE_USER_METATYPE_NO_OPERATORS(QContentId);
Q_DECLARE_USER_METATYPE_TYPEDEF(QContentId,QContentId);
Q_DECLARE_USER_METATYPE_TYPEDEF(QContentIdList,QContentIdList);
Q_DECLARE_USER_METATYPE_TYPEDEF(QContentList,QContentList);
Q_DECLARE_USER_METATYPE_ENUM(QContent::ChangeType);
Q_DECLARE_USER_METATYPE_ENUM(QContent::Role);
Q_DECLARE_USER_METATYPE_ENUM(QContent::DrmState);

uint QTOPIA_EXPORT qHash(const QContentId &id);


#endif
