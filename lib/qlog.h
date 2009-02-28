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
#ifndef QCATEGORIZEDLOG_H
#define QCATEGORIZEDLOG_H

#include <qdebug.h>
#include <qtopiaglobal.h>

#ifndef QT_NO_LOG_STREAM

template<class T> inline int qLog_opt(const T &) { return 1; }

enum QLogUncategorized { _QLog=true }; // So "uncategorized" qLog() works.

/* Catch compile time enums (at most one symbol) */
template<> inline int qLog_opt<int>(const int & v) { return v; }

class QTOPIABASE_EXPORT QLogBase {
public:
    static QDebug log(const char*);
};

# define QLOG_DISABLE(dbgcat) \
    class dbgcat##_QLog : public QLogBase { \
    public: \
        static inline bool enabled() { return 0; }\
    };
# define QLOG_ENABLE(dbgcat) \
    class dbgcat##_QLog : public QLogBase { \
    public: \
        static inline bool enabled() { return 1; }\
    };
# define QLOG_OPTION_VOLATILE(dbgcat,expr) \
    class dbgcat##_QLog : public QLogBase { \
    public: \
        static inline bool enabled() { return expr; }\
    };
# define QLOG_OPTION(dbgcat,expr) \
    class dbgcat##_QLog : public QLogBase { \
    public: \
        static inline bool enabled() { static char mem=0; return (mem ? mem : (mem=(expr)?3:2))&1; } \
    };

# define qLog(dbgcat) if(!dbgcat##_QLog::enabled()); else dbgcat##_QLog::log(#dbgcat)
# define qLogEnabled(dbgcat) (dbgcat##_QLog::enabled())
#else
# define QLOG_DISABLE(dbgcat)
# define QLOG_ENABLE(dbgcat)
# define QLOG_OPTION(dbgcat,expr)
# define QLOG_OPTION_VOLATILE(dbgcat,expr)
# define qLog(dbgcat) if(1); else QNoDebug()
# define qLogEnabled(dbgcat) false
#endif

#endif
