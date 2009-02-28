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

#ifndef SIMFILESYSTEM_H
#define SIMFILESYSTEM_H

#include "phonesim.h"

class SimFileItem;

class SimFileSystem : public QObject
{
    Q_OBJECT
public:
    SimFileSystem( SimRules *rules, SimXmlNode& e );
    ~SimFileSystem();

    // Execute an AT+CRSM command against the filesystem.
    void crsm( const QString& args );

    // Find an item with a specific id.
    SimFileItem *findItem( const QString& fileid ) const;

    // Find the parent of an item with a specific id even if the
    // item itself does not exist.  The parameter should be fully qualified.
    SimFileItem *findItemParent( const QString& fileid ) const;

    // Find an item relative to the current item and update current item.
    SimFileItem *findItemRelative( const QString& fileid );

    // Resolve a file identifier to its full path from the root directory.
    QString resolveFileId( const QString& fileid ) const;

private:
    SimRules *rules;
    SimFileItem *rootItem;
    SimFileItem *currentItem;
};

class SimFileItem : public QObject
{
    Q_OBJECT
public:
    SimFileItem( const QString& fileid, SimFileItem *parentDir );
    ~SimFileItem();

    QString fileid() const { return _fileid; }
    SimFileItem *parentDir() const { return _parentDir; }

    QByteArray contents() const { return _contents; }
    void setContents( const QByteArray& value ) { _contents = value; }

    int recordSize() const { return _recordSize; }
    void setRecordSize( int value ) { _recordSize = value; }

    bool isDirectory() const { return _isDirectory; }
    void setIsDirectory( bool value ) { _isDirectory = value; }

    QList<SimFileItem *> children() const { return _children; }

    SimFileItem *findItem( const QString& fileid );

private:
    QString _fileid;
    SimFileItem *_parentDir;
    QByteArray _contents;
    int _recordSize;
    bool _isDirectory;
    QList<SimFileItem *> _children;
};


#endif /* SIMFILESYSTEM_H */
