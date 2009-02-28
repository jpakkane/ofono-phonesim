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
#include <qlog.h>

#ifndef QTOPIA_LOG_H
#define QTOPIA_LOG_H

#include <qtopiaglobal.h>

QTOPIABASE_EXPORT bool qtopiaLogRequested( const char* );
QTOPIABASE_EXPORT bool qtopiaLogEnabled( const char* );
QTOPIABASE_EXPORT bool qtopiaLogOptional( const char* );

#define QTOPIA_LOG_OPTION(x) QLOG_OPTION(x,qtopiaLogRequested(#x))

#include <qtopialog-config.h>

#endif
