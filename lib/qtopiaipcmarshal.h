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

#ifndef __QTOPIAIPCMARSHAL__H__
#define __QTOPIAIPCMARSHAL__H__

#include <qtopiaglobal.h>

#include <qvariant.h>
#include <qmetatype.h>
#include <qdatastream.h>
#include <qatomic.h>
#include <quuid.h>

#if defined(QTOPIA_DBUS_IPC)
#include <qtdbus/qdbusmetatype.h>
#include <qtdbus/qdbusargument.h>
#endif

template <typename T>
struct QMetaTypeRegister
{
    static int registerType() { return 1; }
};

#if QT_VERSION < 0x040400
#define _QATOMIC_ONCE() \
    static QAtomic once; \
    if ( once.exchange(1) ) \
        return 1
#else
#define _QATOMIC_ONCE() \
    static QAtomicInt once; \
    if ( once.fetchAndStoreOrdered(1) ) \
        return 1
#endif

#if defined(QTOPIA_DBUS_IPC)
#define Q_DECLARE_USER_METATYPE_NO_OPERATORS(TYPE) \
    Q_DECLARE_METATYPE(TYPE) \
    template<> \
    struct QMetaTypeRegister< TYPE > \
    { \
        static int registerType() \
        { \
            _QATOMIC_ONCE(); \
            int id = qMetaTypeId( reinterpret_cast<TYPE *>(0) ); \
            if ( id >= static_cast<int>(QMetaType::User) ) {\
                qRegisterMetaTypeStreamOperators< TYPE >( #TYPE ); \
                qDBusRegisterMetaType< TYPE >(); \
            } \
            return 1; \
        } \
        static int __init_variable__; \
    };
#else
#define Q_DECLARE_USER_METATYPE_NO_OPERATORS(TYPE) \
    Q_DECLARE_METATYPE(TYPE) \
    template<> \
    struct QMetaTypeRegister< TYPE > \
    { \
        static int registerType() \
        { \
            _QATOMIC_ONCE(); \
            int id = qMetaTypeId( reinterpret_cast<TYPE *>(0) ); \
            if ( id >= static_cast<int>(QMetaType::User) ) \
                qRegisterMetaTypeStreamOperators< TYPE >( #TYPE ); \
            return 1; \
        } \
        static int __init_variable__; \
    };
#endif

#if defined(QTOPIA_DBUS_IPC)
#define Q_DECLARE_USER_METATYPE(TYPE) \
    Q_DECLARE_USER_METATYPE_NO_OPERATORS(TYPE) \
    QTOPIABASE_EXPORT QDataStream &operator<<(QDataStream &stream, const TYPE &var); \
    QTOPIABASE_EXPORT QDataStream &operator>>( QDataStream &stream, TYPE &var ); \
    QTOPIABASE_EXPORT QDBusArgument &operator<<(QDBusArgument &stream, const TYPE &var); \
    QTOPIABASE_EXPORT const QDBusArgument &operator>>(const QDBusArgument &stream, TYPE &var );
#else
#define Q_DECLARE_USER_METATYPE(TYPE) \
    Q_DECLARE_USER_METATYPE_NO_OPERATORS(TYPE) \
    QTOPIABASE_EXPORT QDataStream &operator<<(QDataStream &stream, const TYPE &var); \
    QTOPIABASE_EXPORT QDataStream &operator>>( QDataStream &stream, TYPE &var );
#endif

#if defined(QTOPIA_DBUS_IPC)
#define Q_DECLARE_USER_METATYPE_TYPEDEF(TAG,TYPE)       \
    template <typename T> \
    struct QMetaTypeRegister##TAG \
    { \
        static int registerType() { return 1; } \
    }; \
    template<> struct QMetaTypeRegister##TAG< TYPE > { \
        static int registerType() { \
            _QATOMIC_ONCE(); \
            int id = qRegisterMetaType< TYPE >( #TYPE ); \
            qRegisterMetaTypeStreamOperators< TYPE >( #TYPE ); \
            void (*mf)(QDBusArgument &, const TYPE *) = qDBusMarshallHelper<TYPE>; \
            void (*df)(const QDBusArgument &, TYPE *) = qDBusDemarshallHelper<TYPE>;\
            QDBusMetaType::registerMarshallOperators(id, \
                reinterpret_cast<QDBusMetaType::MarshallFunction>(mf), \
                reinterpret_cast<QDBusMetaType::DemarshallFunction>(df)); \
            return 1; \
        } \
        static int __init_variable__; \
    };
#else
#define Q_DECLARE_USER_METATYPE_TYPEDEF(TAG,TYPE)       \
    template <typename T> \
    struct QMetaTypeRegister##TAG \
    { \
        static int registerType() { return 1; } \
    }; \
    template<> struct QMetaTypeRegister##TAG< TYPE > { \
        static int registerType() { \
            _QATOMIC_ONCE(); \
            qRegisterMetaType< TYPE >( #TYPE ); \
            qRegisterMetaTypeStreamOperators< TYPE >( #TYPE ); \
            return 1; \
        } \
        static int __init_variable__; \
    };
#endif

#define Q_DECLARE_USER_METATYPE_ENUM(TYPE)      \
    Q_DECLARE_USER_METATYPE(TYPE)

#define Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(TYPE) \
    int QMetaTypeRegister< TYPE >::__init_variable__ = \
        QMetaTypeRegister< TYPE >::registerType();

#if defined(QTOPIA_DBUS_IPC)
#define Q_IMPLEMENT_USER_METATYPE(TYPE) \
    QDataStream &operator<<(QDataStream &stream, const TYPE &var) \
    { \
        var.serialize(stream); \
        return stream; \
    } \
    QDataStream &operator>>( QDataStream &stream, TYPE &var ) \
    { \
        var.deserialize(stream); \
        return stream; \
    } \
    QDBusArgument &operator<<(QDBusArgument &stream, const TYPE &var) \
    { \
        stream.beginStructure(); \
        var.serialize(stream); \
        stream.endStructure(); \
        return stream; \
    } \
    const QDBusArgument &operator>>(const QDBusArgument &stream, TYPE &var ) \
    { \
        stream.beginStructure(); \
        var.deserialize(stream); \
        stream.endStructure(); \
        return stream; \
    } \
    Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(TYPE)
#else
#define Q_IMPLEMENT_USER_METATYPE(TYPE) \
    QDataStream &operator<<(QDataStream &stream, const TYPE &var) \
    { \
        var.serialize(stream); \
        return stream; \
    } \
    \
    QDataStream &operator>>( QDataStream &stream, TYPE &var ) \
    { \
        var.deserialize(stream); \
        return stream; \
    } \
    Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(TYPE)
#endif

#define Q_IMPLEMENT_USER_METATYPE_TYPEDEF(TAG,TYPE)     \
    int QMetaTypeRegister##TAG< TYPE >::__init_variable__ = \
        QMetaTypeRegister##TAG< TYPE >::registerType();

#if defined(QTOPIA_DBUS_IPC)
#define Q_IMPLEMENT_USER_METATYPE_ENUM(TYPE)    \
    QTOPIABASE_EXPORT QDataStream& operator<<( QDataStream& stream, const TYPE &v ) \
    { \
        stream << static_cast<qint32>(v); \
        return stream; \
    } \
    QTOPIABASE_EXPORT QDataStream& operator>>( QDataStream& stream, TYPE& v ) \
    { \
        qint32 _v; \
        stream >> _v; \
        v = static_cast<TYPE>(_v); \
        return stream; \
    } \
    QTOPIABASE_EXPORT QDBusArgument &operator<<(QDBusArgument &a, TYPE v) \
    { \
        a << static_cast<qint32>(v); \
        return a; \
    } \
    QTOPIABASE_EXPORT const QDBusArgument &operator>>(const QDBusArgument &a, TYPE &v) \
    { \
        qint32 _v; \
        a >> _v; \
        v = static_cast<TYPE>(_v); \
        return a; \
    } \
    Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(TYPE)
#else
#define Q_IMPLEMENT_USER_METATYPE_ENUM(TYPE)    \
    QDataStream& operator<<( QDataStream& stream, const TYPE &v ) \
    { \
        stream << static_cast<qint32>(v); \
        return stream; \
    } \
    QDataStream& operator>>( QDataStream& stream, TYPE& v ) \
    { \
        qint32 _v; \
        stream >> _v; \
        v = static_cast<TYPE>(_v); \
        return stream; \
    } \
    Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(TYPE)
#endif

#define Q_REGISTER_USER_METATYPE(TYPE)  \
    QMetaTypeRegister< TYPE >::registerType()

Q_DECLARE_USER_METATYPE_NO_OPERATORS(QUuid)

// Special variant class that can perform QDataStream operations
// without the QVariant header information.
class QtopiaIpcAdaptorVariant : public QVariant
{
    public:
        QtopiaIpcAdaptorVariant() : QVariant() {}
        explicit QtopiaIpcAdaptorVariant( const QVariant& value ) : QVariant( value ) {}

        void load( QDataStream& stream, int typeOrMetaType )
        {
            clear();
            create( typeOrMetaType, 0 );
            d.is_null = false;
            QMetaType::load
                ( stream, d.type, const_cast<void *>(constData()) );
        }
        void save( QDataStream& stream ) const
        {
            QMetaType::save( stream, d.type, constData() );
        }
};

#endif
