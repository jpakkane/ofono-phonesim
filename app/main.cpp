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

#include <lib/server.h>
#ifndef PHONESIM_TARGET
    #include "control.h"
    #include <qapplication.h>
#else
    #include <qcoreapplication.h>
#endif
#include <qstring.h>
#include <qdebug.h>
#include <stdlib.h>

static void usage( const QString& progname )
{
    qWarning() << "Usage: "<< progname << " [-p port] [-gui] filename";
    exit(-1);
}

int main(int argc, char **argv)
{
#ifndef PHONESIM_TARGET
    QApplication app(argc, argv);
#else
    QCoreApplication app(argc, argv);
#endif
    QString filename;
    int port = 12345;
    int index;
    bool with_gui = false;

    // Parse the command-line.
    index = 1;
    for (index = 1; index < argc; index++) {
        if (strcmp(argv[index],"-p") == 0) {
            index++;
            if (index >= argc) {
                usage( app.applicationName() );
            } else {
                port = atoi(argv[index]);
            }
        } else if (strcmp(argv[index],"-gui") == 0) {
            // turn on gui option
            with_gui = true;
        } else if ( strcmp(argv[index],"-h") == 0
                || strcmp(argv[index],"-help") == 0 ) {
            usage( argv[0] );
        } else {
            // must be filename.  SHOULD be last argument.
            if (index != argc-1) {
                usage( app.applicationName() );
            }
            filename = argv[index];
        }

    }

    PhoneSimServer *pss = new PhoneSimServer(filename, port, 0);

#ifndef PHONESIM_TARGET
    if (with_gui)
        pss->setHardwareManipulator(new ControlFactory);
#else
    Q_UNUSED(pss);
#endif

    return app.exec();
}

