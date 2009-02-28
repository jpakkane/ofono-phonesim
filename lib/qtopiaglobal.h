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
#ifndef QPE_GLOBAL_H
#define QPE_GLOBAL_H

#include <qglobal.h>
#include <qplugin.h>

// The _EXPORT macros...

#if defined(QT_VISIBILITY_AVAILABLE)
#   define QTOPIA_VISIBILITY __attribute__((visibility("default")))
#else
#   define QTOPIA_VISIBILITY
#endif


#ifndef QTOPIABASE_EXPORT
#   define QTOPIABASE_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIA_EXPORT
#   define QTOPIA_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAPIM_EXPORT
#   define QTOPIAPIM_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAMAIL_EXPORT
#   define QTOPIAMAIL_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIA_PLUGIN_EXPORT
#   define QTOPIA_PLUGIN_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIACOMM_EXPORT
#   define QTOPIACOMM_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAPHONE_EXPORT
#   define QTOPIAPHONE_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAPHONEMODEM_EXPORT
#   define QTOPIAPHONEMODEM_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIASECURITY_EXPORT
#   define QTOPIASECURITY_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAHW_EXPORT
#   define QTOPIAHW_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAAUDIO_EXPORT
#   define QTOPIAAUDIO_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAMEDIA_EXPORT
#   define QTOPIAMEDIA_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAWAP_EXPORT
#   define QTOPIAWAP_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIASMIL_EXPORT
#   define QTOPIASMIL_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAOMADRM_EXPORT
#   define QTOPIAOMADRM_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIAPRINTING_EXPORT
#   define QTOPIAPRINTING_EXPORT QTOPIA_VISIBILITY
#endif
#ifndef QTOPIATEST_EXPORT
#   define QTOPIATEST_EXPORT QTOPIA_VISIBILITY
#endif

// This macro exports symbols only when building a test-enabled build.
// Use this to make private classes available for test.

#ifdef QTOPIA_TEST_EXTRA_SYMBOLS
#   ifndef QTOPIA_AUTOTEST_EXPORT
#       define QTOPIA_AUTOTEST_EXPORT QTOPIA_VISIBILITY
#   endif
#else
#   ifndef QTOPIA_AUTOTEST_EXPORT
#       define QTOPIA_AUTOTEST_EXPORT
#   endif
#endif

// This is the magic that lets Qt and Qtopia plugins be compiled for singleexec

#ifndef SINGLE_EXEC
#define QTOPIA_EXPORT_PLUGIN(IMPLEMENTATION) Q_EXPORT_PLUGIN(IMPLEMENTATION)
#define QTOPIA_EXPORT_QT_PLUGIN(IMPLEMENTATION) Q_EXPORT_PLUGIN(IMPLEMENTATION)
#else
class QObject;
typedef QObject *(*qtopiaPluginCreateFunc_t)();
extern void registerPlugin(const char *name, const char *type, qtopiaPluginCreateFunc_t createFunc);
#define QTOPIA_EXPORT_PLUGIN(IMPLEMENTATION) \
    static QObject *create_ ## IMPLEMENTATION() \
        { return new IMPLEMENTATION(); } \
    static qtopiaPluginCreateFunc_t append_ ## IMPLEMENTATION() \
        { registerPlugin(QTOPIA_TARGET, QTOPIA_PLUGIN_TYPE, create_ ##IMPLEMENTATION); \
            return create_ ##IMPLEMENTATION; } \
    static qtopiaPluginCreateFunc_t dummy_ ## IMPLEMENTATION = \
        append_ ## IMPLEMENTATION();
#define QTOPIA_EXPORT_QT_PLUGIN(IMPLEMENTATION)\
    Q_EXPORT_PLUGIN(IMPLEMENTATION)\
    Q_IMPORT_PLUGIN(IMPLEMENTATION)
#endif


#endif
