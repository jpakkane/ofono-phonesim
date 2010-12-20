/****************************************************************************
**
** This file is part of the Qt Extended Opensource Package.
**
** Copyright (C) 2009 Trolltech ASA.
**
** Contact: Qt Extended Information (info@qtextended.org)
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation and appearing
** in the file LICENSE.GPL included in the packaging of this file.
**
** Please review the following information to ensure GNU General Public
** Licensing requirements will be met:
**     http://www.fsf.org/licensing/licenses/info/GPLv2.html.
**
**
****************************************************************************/

#include "simfilesystem.h"
#include <qatutils.h>
#include <qdebug.h>

// Known files, their parent directories, and names.
// This list comes from 3GPP TS 11.11 and 3GPP TS 51.011.
typedef struct
{
    const char *fileid;
    const char *parent;
    const char *name;
    uint32_t access;
    enum file_type type;
} SimFileInfo;
static SimFileInfo const knownFiles[] =
{
    {"3F00",        0,          "MF",          0,         FILE_TYPE_TRANSPARENT},
    {"7F20",        "3F00",     "DFgsm",       0,         FILE_TYPE_TRANSPARENT},
    {"7F10",        "3F00",     "DFtelecom",   0,         FILE_TYPE_TRANSPARENT},

    // Order according to sections in 31.102
    {"6F05",        "7F20",     "EFli",        0x01ff44,  FILE_TYPE_TRANSPARENT},
    {"6F07",        "7F20",     "EFimsi",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F60",        "7F20",     "EFplmnwact",  0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F31",        "7F20",     "EFhpplmn",    0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F37",        "7F20",     "EFacmmax",    0x12ff44,  FILE_TYPE_TRANSPARENT},
    {"6F38",        "7F20",     "EFust",       0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F39",        "7F20",     "EFacm",       0x111f44,  FILE_TYPE_CYCLIC},
    {"6F3E",        "7F20",     "EFgid1",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F3F",        "7F20",     "EFgid2",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F46",        "7F20",     "EFspn",       0x04ff44,  FILE_TYPE_TRANSPARENT},
    {"6F41",        "7F20",     "EFpuct",      0x12ff44,  FILE_TYPE_TRANSPARENT},
    {"6F45",        "7F20",     "EFcbmi",      0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F78",        "7F20",     "EFacc",       0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F7B",        "7F20",     "EFfplmn",     0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F7E",        "7F20",     "EFloci",      0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6FAD",        "7F20",     "EFad",        0x04ff44,  FILE_TYPE_TRANSPARENT},
    {"6F48",        "7F20",     "EFcbmid",     0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FB7",        "7F20",     "EFecc",       0x04ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F50",        "7F20",     "EFcbmir",     0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F73",        "7F20",     "EFpsloci",    0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F3B",        "7F10",     "EFfdn",       0x12ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F3C",        "7F10",     "EFsms",       0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F40",        "7F10",     "EFmsisdn",    0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F42",        "7F10",     "EFsmsp",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F43",        "7F10",     "EFsmss",      0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F49",        "7F10",     "EFsdn",       0x14ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F4B",        "7F10",     "EFext2",      0x12ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F4C",        "7F10",     "EFext3",      0x14ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F47",        "7F10",     "EFsmsr",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F80",        "7F20",     "EFici",       0x111f44,  FILE_TYPE_CYCLIC},
    {"6F81",        "7F20",     "EFoci",       0x111f44,  FILE_TYPE_CYCLIC},
    {"6F82",        "7F20",     "EFict",       0x121f44,  FILE_TYPE_CYCLIC},
    {"6F83",        "7F20",     "EFoct",       0x121f44,  FILE_TYPE_CYCLIC},
    {"6F4E",        "7F10",     "EFext5",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F4F",        "7F10",     "EFccp2",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FB5",        "7F20",     "EFemlpp",     0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FB6",        "7F20",     "EFaaem",      0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F4D",        "7F10",     "EFbdn",       0x12ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F55",        "7F10",     "EFext4",      0x12ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F58",        "7F10",     "EFcmi",       0x14ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F56",        "7F20",     "EFest",       0x12ff44,  FILE_TYPE_TRANSPARENT},
    {"6F2C",        "7F20",     "EFdck",       0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F32",        "7F20",     "EFcnl",       0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F61",        "7F20",     "EFoplmnwact", 0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F62",        "7F20",     "EFhplmnwact", 0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FC5",        "7F20",     "EFpnn",       0x04ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FC6",        "7F20",     "EFopl",       0x04ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FC7",        "7F20",     "EFmbdn",      0x11ff44,  FILE_TYPE_LINEAR_FIXED}, 
    {"6FC8",        "7F20",     "EFext6",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FC9",        "7F20",     "EFmbi",       0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FCA",        "7F20",     "EFmwis",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FCB",        "7F20",     "EFcfis",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FCC",        "7F20",     "EFext7",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FCD",        "7F20",     "EFspdi",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FCE",        "7F20",     "EFmmsn",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FCF",        "7F20",     "EFext8",      0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FD0",        "7F20",     "EFmmsicp",    0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FD1",        "7F20",     "EFmmsup",     0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6FD2",        "7F20",     "EFmmsucp",    0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6FB1",        "7F20",     "EFvgcs",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FB2",        "7F20",     "EFvgcss",     0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6FB3",        "7F20",     "EFvbs",       0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FB4",        "7F20",     "EFvbss",      0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6FD9",        "7F20",     "EFehplmn",    0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FDB",        "7F20",     "EFehplmnpi",  0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6FDE",        "7F20",     "EFspni",      0x04ff44,  FILE_TYPE_TRANSPARENT},
    {"6FDF",        "7F20",     "EFpnni",      0x04ff44,  FILE_TYPE_LINEAR_FIXED},

    {"2FE2",        "3F00",     "EFiccid",     0x04ff44,  FILE_TYPE_TRANSPARENT},
    {"2F05",        "3F00",     "EFpl",        0x11ff44,  FILE_TYPE_TRANSPARENT},

    {"5F50",        "7F10",     "DFgraphics",  0,         FILE_TYPE_TRANSPARENT},
    {"4F20",        "5F50",     "EFimg",       0x14ff44,  FILE_TYPE_LINEAR_FIXED},
    {"4F01",        "5F50",     "EFimg1",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"4F02",        "5F50",     "EFimg2",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"4F03",        "5F50",     "EFimg3",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"4F04",        "5F50",     "EFimg4",      0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"4F05",        "5F50",     "EFimg5",      0x14ff44,  FILE_TYPE_TRANSPARENT},

    // CPHS 4.2
    {"6F13",        "7F20",     "EFcphs_cff",  0x11ff44,  FILE_TYPE_TRANSPARENT},
    {"6F16",        "7F20",     "EFcphs_info", 0x14ff44,  FILE_TYPE_TRANSPARENT},
    {"6F17",        "7F20",     "EFcphs_mbdn", 0x11ff44,  FILE_TYPE_LINEAR_FIXED},
    {"6F11",        "7F20",     "EFcphs_mwis", 0x11ff44,  FILE_TYPE_TRANSPARENT},
    {0,             0,          0,             0,         FILE_TYPE_TRANSPARENT}
};

SimFileSystem::SimFileSystem( SimRules *rules, SimXmlNode& e )
    : QObject( rules )
{
    this->rules = rules;
    rootItem = new SimFileItem( "3F00", 0 );
    currentItem = rootItem;

    // Create all of the standard directories.
    const SimFileInfo *info = knownFiles;
    SimFileItem *dirItem = 0;
    while ( info->fileid ) {
        QString fileid = info->fileid;
        if ( fileid.startsWith( "7F" ) ) {
            dirItem = new SimFileItem( fileid, rootItem );
        } else if ( fileid.startsWith( "5F" ) ) {
            new SimFileItem( fileid, dirItem );
        }
        ++info;
    }

    // Load the file definitions.
    SimXmlNode *child = e.children;
    while ( child ) {
        if ( child->tag == "file" ) {
            QString name = child->getAttribute( "name" );
            QByteArray data = QAtUtils::fromHex( child->contents );
            QString fileid = resolveFileId( name );
            int access = findItemAccess( name );
            enum file_type type = findItemFileType( name );
            SimFileItem *parent = findItemParent( fileid );
            if ( parent ) {
                SimFileItem *item;
                item = findItem( fileid.right(4) );
                if ( !item )
                    item = new SimFileItem( fileid.right(4), parent, access, type );
                else
                    qDebug() << "File" << name << "defined multiple times";
                item->setContents( data );
                QString size = child->getAttribute( "recordsize" );
                if ( !size.isEmpty() )
                    item->setRecordSize( size.toInt() );
            } else {
                qDebug() << "Could not find parent for" << name;
            }
        } else {
            qDebug() << "Unknown filesystem command <" << child->tag << ">";
        }
        child = child->next;
    }

    /* Select DFgsm initially */
    currentItem = findItem("7F20");
}

SimFileSystem::~SimFileSystem()
{
    delete rootItem;
}

void SimFileSystem::crsm( const QString& args )
{
    // Extract the arguments to the command.
    uint posn = 0;
    uint command = QAtUtils::parseNumber( args, posn );
    QString fileid =
        QString::number( QAtUtils::parseNumber( args, posn ), 16 ).toUpper();
    uint p1 = QAtUtils::parseNumber( args, posn );
    uint p2 = QAtUtils::parseNumber( args, posn );
    uint p3 = QAtUtils::parseNumber( args, posn );
    QByteArray data = QAtUtils::fromHex( args.mid( (int)posn ) );

    // Determine how to execute the command.
    bool ok = true;
    SimFileItem *item;
    QString response;
    int sw1 = 0;
    int sw2 = 0;
    int offset, length;
    QByteArray contents;
    switch ( command ) {

        case 176:       // READ BINARY
        {
            offset = (int)((p1 << 8) + p2);
            length = (int)p3;
            item = findItem( fileid );
            if ( !item ) {
                sw1 = 0x94;
                sw2 = 0x04;
            } else if ( item->recordSize() > 0) {
                sw1 = 0x94;
                sw2 = 0x08;
            } else if ( !item->checkAccess( FILE_OP_READ, true ) ) {
                sw1 = 0x98;
                sw2 = 0x04;
            } else {
                contents = item->contents();
                if ( ( offset + length ) > contents.size() ) {
                    sw1 = 0x94;
                    sw2 = 0x02;
                } else if ( length ) {
                    sw1 = 0x9f;
                    sw2 = length;
                    response =
                        QAtUtils::toHex( contents.mid( offset, length ) );
                } else {
                    sw1 = 0x67;
                    sw2 = contents.size() - offset;
                }
            }
            if ( item )
                currentItem = item;
        }
        break;

        case 178:       // READ RECORD
        {
            offset = (int)(p1 - 1);
            length = (int)p3;
            item = findItem( fileid );
            if ( !item ) {
                sw1 = 0x94;
                sw2 = 0x04;
            } else if ( p2 != 0x04 ) {
                // Only absolute reads are supported.
                sw1 = 0x6b;
                sw2 = 0x00;
            } else if ( !item->checkAccess( FILE_OP_READ, true ) ) {
                sw1 = 0x98;
                sw2 = 0x04;
            } else if ( item->recordSize() <= 0 ) {
                sw1 = 0x94;
                sw2 = 0x08;
            } else {
                offset *= item->recordSize();
                contents = item->contents();
                if ( length < 1 || length > item->recordSize() ) {
                    sw1 = 0x67;
                    sw2 = item->recordSize();
                } else if ( offset >= contents.size() || offset < 0 ) {
                    sw1 = 0x94;
                    sw2 = 0x02;
                } else {
                    sw1 = 0x9F;
                    sw2 = length;
                    response =
                        QAtUtils::toHex( contents.mid( offset, length ) );
                }
            }
            if ( item )
                currentItem = item;
        }
        break;

        case 192:       // GET RESPONSE
        {
            item = findItem( fileid );
            if ( !item ) {
                sw1 = 0x94;
                sw2 = 0x04;
                break;
            }
            currentItem = item;
        }
        // Fall through to the next case.

        case 242:       // STATUS
        {
            // Format the status response according to GSM 51.011, 9.2.1.
            char status[15];
            status[0] = 0x00;           // RFU
            status[1] = 0x00;
            int size = currentItem->contents().size();
            status[2] = (char)(size >> 8);
            status[3] = (char)size;
            status[4] = fileid.left(2).toInt(0, 16);
            status[5] = fileid.right(2).toInt(0, 16);
            if ( currentItem == rootItem ) {
                status[6] = 0x01;
            } if ( currentItem->isDirectory() ) {
                status[6] = 0x02;
            } else {
                status[6] = 0x04;
            }
            status[7] = 0x00;           // RFU

            int access = currentItem->access();
            status[8] = (access >> 16) & 0xff;
            status[9] = (access >> 8) & 0xff;
            status[10] = (access >> 0) & 0xff;

            // File Status, TS 11.11, Section 9.3
            status[11] = 0x01;

            status[12] = 2;             // Size of data that follows.
            if ( currentItem->isDirectory() ) {
                status[13] = 0x00;
                status[14] = 0x00;
            } else if ( currentItem->recordSize() > 0 ) {
                status[13] = (char)(currentItem->type() );
                status[14] = (char)( currentItem->recordSize() );
            } else {
                status[13] = 0x00;
                status[14] = 0x00;
            }
            sw1 = 0x90;
            sw2 = 0x00;
            response = QAtUtils::toHex( QByteArray( status, 15 ) );
        }
        break;

        case 214:       // UPDATE BINARY
        {
            offset = (int)((p1 << 8) + p2);
            length = (int)p3;
            item = findItem( fileid );
            if ( !item ) {
                sw1 = 0x94;
                sw2 = 0x04;
            } else if ( !item->checkAccess( FILE_OP_UPDATE, true ) ) {
                sw1 = 0x98;
                sw2 = 0x04;
            } else if ( item->recordSize() > 0 || data.size() != length ) {
                sw1 = 0x94;
                sw2 = 0x08;
            } else {
                contents = item->contents();
                if ( ( offset + length ) > contents.size() ) {
                    sw1 = 0x94;
                    sw2 = 0x02;
                } else if ( length ) {
                    sw1 = 0x90;
                    sw2 = 0x00;
                    item->setContents(
                            contents.left( offset ) +
                            data +
                            contents.mid( offset + length ) );
                } else {
                    sw1 = 0x67;
                    sw2 = contents.size() - offset;
                }
            }
            if ( item )
                currentItem = item;
        }
        break;

        case 220:       // UPDATE RECORD
        {
            offset = (int)(p1 - 1);
            length = (int)p3;
            item = findItem( fileid );
            if ( !item ) {
                sw1 = 0x94;
                sw2 = 0x04;
            } else if ( !length ) {
                sw1 = 0x67;
                sw2 = item->recordSize();
            } else if ( !item->checkAccess( FILE_OP_UPDATE, true ) ) {
                sw1 = 0x98;
                sw2 = 0x04;
            } else if ( p2 != 0x04 ) {
                // Only absolute writes are supported.
                sw1 = 0x6b;
                sw2 = 0x00;
            } else if ( item->recordSize() != length ||
                    data.size() != length ) {
                sw1 = 0x94;
                sw2 = 0x08;
            } else {
                offset *= item->recordSize();
                contents = item->contents();
                if ( offset >= contents.size() || offset < 0 ) {
                    sw1 = 0x94;
                    sw2 = 0x02;
                } else {
                    sw1 = 0x90;
                    sw2 = 0x00;
                    item->setContents(
                            contents.left( offset ) +
                            data +
                            contents.mid( offset + length ) );
                }
            }
            if ( item )
                currentItem = item;
        }
        break;

        default: ok = false; break;
    }

    // Send the response information.
    if ( sw1 != 0 ) {
        QString resp;
        resp = "+CRSM: " + QString::number(sw1) + "," + QString::number(sw2);
        if ( !response.isEmpty() )
            resp += "," + response;
        rules->respond( resp );
    }
    if ( ok )
        rules->respond( "OK" );
    else
        rules->respond( "ERROR" );
}

SimFileItem *SimFileSystem::findItem( const QString& fileid ) const
{
    return rootItem->findItem( fileid );
}

SimFileItem *SimFileSystem::findItemParent( const QString& fileid ) const
{
    QString parent = fileid.left( fileid.length() - 4 );
    if ( parent.isEmpty() )
        return rootItem;
    else
        return findItem( parent.right(4) );
}

SimFileItem *SimFileSystem::findItemRelative( const QString& fileid )
{
    SimFileItem *item;
    if ( fileid == "3F00" ) {
        // We can always find the root directory no matter where we are.
        item = rootItem;
    } else if ( currentItem->fileid() == fileid ) {
        // We can always find where we currently are.
        item = currentItem;
    } else if ( currentItem->parentDir() &&
                currentItem->parentDir()->fileid() == fileid ) {
        // We can always find our parent directory.
        item = currentItem->parentDir();
    } else {
        // Search the current item's immediate children.
        QList<SimFileItem *> children = currentItem->children();
        item = 0;
        foreach( SimFileItem *temp, children ) {
            if ( temp->fileid() == fileid ) {
                item = temp;
                break;
            }
        }
        if ( !item )
            return 0;
    }
    currentItem = item;
    return item;
}

QString SimFileSystem::resolveFileId( const QString& _fileid ) const
{
    QString fileid = _fileid;

    // Convert alphabetic names into their numeric equivalents.
    const SimFileInfo *info = knownFiles;
    while ( info->fileid ) {
        if ( fileid == info->name ) {
            fileid = info->fileid;
            break;
        }
        ++info;
    }

    // Determine if the fileid is already a full path.
    if ( fileid.startsWith( "3F" ) ||       // MF root directory.
         fileid.startsWith( "7F" ) ||       // First-level DF directory.
         fileid.startsWith( "2F" ) )        // First-level EF file.
        return fileid;

    // Extract the first component and search for a path back to the root.
    QString newId = fileid;
    QString first = fileid.left(4);
    for(;;) {
        info = knownFiles;
        while ( info->fileid ) {
            if ( first == info->fileid && info->parent ) {
                first = info->parent;
                newId = first + newId;
                if ( first.startsWith( "3F" ) || first.startsWith( "7F" ) )
                    return newId;
                break;
            }
            ++info;
        }
        if ( !info->fileid ) {
            // We could not find a suitable parent directory, so bail out.
            return newId;
        }
    }
}

int SimFileSystem::findItemAccess( const QString& _fileid ) const
{
    QString fileid = _fileid;

    const SimFileInfo *info = knownFiles;
    while ( info->fileid ) {
        if ( fileid == info->name || fileid == info->fileid )
            return info->access;
        ++info;
    }

    return 0;
}

enum file_type SimFileSystem::findItemFileType( const QString& _fileid ) const
{
    QString fileid = _fileid;

    const SimFileInfo *info = knownFiles;
    while ( info->fileid ) {
        if ( fileid == info->name || fileid == info->fileid )
            return info->type;
        ++info;
    }

    return FILE_TYPE_INVALID;
}
SimFileItem::SimFileItem( const QString& fileid, SimFileItem *parentDir,
        int access, enum file_type type)
    : QObject( parentDir )
{
    _fileid = fileid;
    _parentDir = parentDir;
    _recordSize = 0;
    _access = access;
    _isDirectory = false;
    _type = type;
    if ( parentDir ) {
        parentDir->_isDirectory = true;
        parentDir->_children.append( this );
    }
}

SimFileItem::~SimFileItem()
{
}

SimFileItem *SimFileItem::findItem( const QString& fileid )
{
    if ( fileid == _fileid )
        return this;
    foreach ( SimFileItem *item, _children ) {
        SimFileItem *temp = item->findItem( fileid );
        if ( temp )
            return temp;
    }
    return 0;
}

bool SimFileItem::checkAccess( enum file_op op, bool havepin ) const
{
    switch ( (access() >> op) & 0xf ) {
        case FILE_ACCESS_ALWAYS:
            return true;

        case FILE_ACCESS_CHV1:
        case FILE_ACCESS_CHV2:
            return havepin;

        case FILE_ACCESS_RESERVED:
        case FILE_ACCESS_ADM:
        case FILE_ACCESS_NEVER:
        default:
            return false;
    }
}
