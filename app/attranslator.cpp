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

#include "attranslator.h"

AtTranslator::AtTranslator(const QString& specFile)
    : gsmSpec(specFile)
{
}

QString AtTranslator::translateCommand( const QString& data )
{
    QString command;
    QString parameters;

    if( data.contains("?") ){
        command = data.left( data.indexOf("?")+1 );
    }else if( data.contains("=") ){
        command = data.left( data.indexOf("=")+1 );
        parameters = data.right( data.indexOf("=")+1 );
    }else{
        command = data;
    }
    QString answer = gsmSpec.getProfile(command);
    if(!gsmSpec.validateCommand(command, parameters) )
        answer = answer.append("\n").append("Parameters not valid, expected: ").append(gsmSpec.getParameterFormat(command).join("||"));
    return answer;
}

QString AtTranslator::translateResponse( const QString& data )
{
    Q_UNUSED( data );
    return "todo";
}

void AtTranslator::resetSpecification(const QString& filePath)
{
    gsmSpec.resetDictionary(filePath);
}








