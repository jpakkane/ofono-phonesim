/****************************************************************************
**
** This file contains the SAT conformance test cases
**
** Copyright 2011 EB(Elektrobit).
**
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

#include "simapplication.h"
#include <qatutils.h>
#include <qdebug.h>
#include <QTextCodec>
#include "qsmsmessage.h"

ConformanceSimApplication::ConformanceSimApplication( SimRules *rules,
                                                        QObject *parent )
    : SimApplication( rules, parent )
{
}

ConformanceSimApplication::~ConformanceSimApplication()
{
}

const QString ConformanceSimApplication::getName()
{
    return "Conformance SIM Application";
}

#define ConformanceMenu_DisplayText 1

#define NormalMenu_1_1    1
#define NormalMenu_1_2    2
#define NormalMenu_1_3    3
#define NormalMenu_1_4    4
#define NormalMenu_1_5    5
#define NormalMenu_1_6    6
#define NormalMenu_1_7    7
#define NormalMenu_1_8    8
#define NormalMenu_1_9    9
#define NormalMenu_1_10   10
#define NormalMenu_1_11   11
#define NormalMenu_Main   12

#define IconMenu_1A     1
#define IconMenu_2A     2
#define IconMenu_3A     3
#define IconMenu_4A     4
#define IconMenu_5A     5
#define IconMenu_6A     6
#define IconMenu_Main   7

#define Display_Text_Normal              1
#define Display_Text_No_Response         2
#define Display_Text_Extension_Text      3
#define Display_Text_Sustained_Text_1    4
#define Display_Text_Sustained_Text_2    5
#define Display_Text_Sustained_Text_3    6
#define Display_Text_Icon                7
#define Display_Text_UCS2_Cyrillic       8
#define Display_Text_Variable_Time_Out   9
#define Display_Text_Attribute          10
#define Display_Text_Main               11

void ConformanceSimApplication::mainMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SetupMenu );

    item.setIdentifier( ConformanceMenu_DisplayText );
    item.setLabel( "Display Text" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, 0, 0 );
}

void ConformanceSimApplication::mainMenuSelection( int id )
{
    switch ( id ) {
        case ConformanceMenu_DisplayText:
        {
            sendDisplayTextMenu();
        }
        break;

        default:
        {
            // Don't know what this item is, so just re-display the main menu.
            endSession();
        }
        break;
    }
}

void ConformanceSimApplication::sendDisplayTextMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Display Text" );

    item.setIdentifier( Display_Text_Normal );
    item.setLabel( "Normal" );
    items += item;

    item.setIdentifier( Display_Text_No_Response );
    item.setLabel( "Support of No response from user" );
    items += item;

    item.setIdentifier( Display_Text_Extension_Text );
    item.setLabel( "Display of the extension text" );
    items += item;

    item.setIdentifier( Display_Text_Sustained_Text_1 );
    item.setLabel( "Sustained text, unpacked 8-bit data" );
    items += item;

    item.setIdentifier( Display_Text_Sustained_Text_2 );
    item.setLabel( "Sustained text, clear message after delay" );
    items += item;

    item.setIdentifier( Display_Text_Sustained_Text_3 );
    item.setLabel( "Sustained text, wait for user MMI to clear" );
    items += item;

    item.setIdentifier( Display_Text_Icon );
    item.setLabel( "Display of icon" );
    items += item;

    item.setIdentifier( Display_Text_UCS2_Cyrillic );
    item.setLabel( "UCS2 display supported in Cyrillic" );
    items += item;

    item.setIdentifier( Display_Text_Variable_Time_Out );
    item.setLabel( "Variable Time out" );
    items += item;

    item.setIdentifier( Display_Text_Attribute );
    item.setLabel( "Support of Text Attribute" );
    items += item;

    item.setIdentifier( Display_Text_Main );
    item.setLabel( "Return to main menu" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(DisplayTextMenu(QSimTerminalResponse)) );
}

void ConformanceSimApplication::sendDisplayTextNormalMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Display Text (Normal)" );

    item.setIdentifier( NormalMenu_1_1 );
    item.setLabel( "normal priority, Unpacked 8 bit data,successful" );
    items += item;

    item.setIdentifier( NormalMenu_1_2 );
    item.setLabel( "Unpacked 8 bit data,screen busy" );
    items += item;

    item.setIdentifier( NormalMenu_1_3 );
    item.setLabel( "high priority, Unpacked 8 bit data,successful" );
    items += item;

    item.setIdentifier( NormalMenu_1_4 );
    item.setLabel( "Packed, SMS default alphabet,successful" );
    items += item;

    item.setIdentifier( NormalMenu_1_5 );
    item.setLabel( "Clear message after delay,successful" );
    items += item;

    item.setIdentifier( NormalMenu_1_6 );
    item.setLabel( "Text string with 160 bytes,successful" );
    items += item;

    item.setIdentifier( NormalMenu_1_7 );
    item.setLabel( "Backward move, successful" );
    items += item;

    item.setIdentifier( NormalMenu_1_8 );
    item.setLabel( "session terminated by user" );
    items += item;

    item.setIdentifier( NormalMenu_1_9 );
    item.setLabel( "Null text string, successful" );
    items += item;

    item.setIdentifier( NormalMenu_1_10 );
    item.setLabel( "Empty text string, successful" );
    items += item;

    item.setIdentifier( NormalMenu_Main );
    item.setLabel( "Return to Display Text main menu" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(DisplayTextNormalMenu(QSimTerminalResponse)) );
}

void ConformanceSimApplication::sendDisplayTextIconMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Display Text (Icon support)" );

    item.setIdentifier( IconMenu_1A );
    item.setLabel( "basic icon, self-explanatory, successful" );
    items += item;

    item.setIdentifier( IconMenu_2A );
    item.setLabel( "display of colour icon, successful" );
    items += item;

    item.setIdentifier( IconMenu_3A );
    item.setLabel( "basic icon, not self-explanatory, successful" );
    items += item;

    item.setIdentifier( IconMenu_4A );
    item.setLabel( "basic icon, empty text string, unsuccessful" );
    items += item;

    item.setIdentifier( IconMenu_Main );
    item.setLabel( "Return to Display Text main menu" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(DisplayTextIconMenu(QSimTerminalResponse)) );
}

void ConformanceSimApplication::DisplayTextMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    /* Item selected. */
    switch ( resp.menuItem() ) {
        case Display_Text_Normal:
        {
            sendDisplayTextNormalMenu();
        }
        break;

        case Display_Text_No_Response:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "<TIME-OUT>" );
            command( cmd, this, SLOT(sendDisplayTextMenu()) );
        }
        break;

        case Display_Text_Extension_Text:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "This command instructs the ME to display a"
                    " text message and/or an icon (see 6.5.4). "
                    "It allows the SIM to define the priority"
                    " of that message, and the text string format."
                    " Two types of priority are defined:- display "
                    " normal priority text and/");
            command( cmd, this, SLOT(sendDisplayTextMenu()) );
        }
        break;

        case Display_Text_Sustained_Text_1:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Toolkit Test 1" );
            cmd.setImmediateResponse( true );
            command( cmd, this, SLOT(sendDisplayTextMenu()) );
        }
        break;

        case Display_Text_Sustained_Text_2:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( true );
            cmd.setText( "Toolkit Test 2" );
            cmd.setImmediateResponse( true );
            command( cmd, this, SLOT(sendDisplayTextMenu()) );
        }
        break;

        case Display_Text_Sustained_Text_3:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Toolkit Test 3" );
            cmd.setImmediateResponse( true );
            command( cmd, this, SLOT(sendDisplayTextMenu()) );
        }
        break;

        case Display_Text_Icon:
        {
            sendDisplayTextIconMenu();
        }
        break;

        case Display_Text_UCS2_Cyrillic:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            QTextCodec *codec = QTextCodec::codecForName( "utf8" );
            cmd.setText( codec->toUnicode( "ЗДРАВСТВУЙТЕ" ) );
            command( cmd, this, SLOT(sendDisplayTextMenu()),
                    QSimCommand::UCS2Strings );
        }
        break;

        case Display_Text_Variable_Time_Out:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "10 Second" );
            cmd.setDuration( 10000 );
            command( cmd, this, SLOT(sendDisplayTextMenu()) );
        }
        break;

        case Display_Text_Attribute:
        {
            QByteArray ba;
            ba.resize( 4 );
            ba[0] = 0x00;
            ba[1] = 0x10;
            ba[2] = 0x00;
            ba[3] = 0xB4;

            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Text Attribute 1" );
            cmd.setTextAttribute( ba );
            command( cmd, this, SLOT(sendDisplayTextMenu()) );
        }
        break;

        default:
            endSession();
        break;
    }
}

void ConformanceSimApplication::DisplayTextNormalMenu(
                    const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    /* Item selected. */
    switch ( resp.menuItem() ) {
        case NormalMenu_1_1:
        case NormalMenu_1_2:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Toolkit Test 1" );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_1_3:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setHighPriority( true );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Toolkit Test 2" );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_1_4:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Toolkit Test 3" );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()),
                    QSimCommand::PackedStrings );
        }
        break;

        case NormalMenu_1_5:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( true );
            cmd.setText( "Toolkit Test 4" );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_1_6:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "This command instructs the ME to display a"
                    " text message. It allows the SIM to define"
                    " the priority of that message, and the text"
                    " string format. Two types of prio" );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_1_7:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "<GO-BACKWARDS>" );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_1_8:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "<ABORT>" );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_1_9:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText("");
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_1_10:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( QString() );
            command( cmd, this, SLOT(sendDisplayTextNormalMenu()) );
        }
        break;

        case NormalMenu_Main:
        {
            sendDisplayTextMenu();
        }
        break;

        default:
            endSession();
        break;
    }
}

void ConformanceSimApplication::DisplayTextIconMenu(
                    const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    /* Item selected. */
    switch ( resp.menuItem() ) {
        case IconMenu_1A:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Basic Icon" );
            cmd.setIconId( 1 );
            cmd.setIconSelfExplanatory( true );
            command( cmd, this, SLOT(sendDisplayTextIconMenu()) );
        }
        break;

        case IconMenu_2A:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Colour Icon" );
            cmd.setIconId( 2 );
            cmd.setIconSelfExplanatory( true );
            command( cmd, this, SLOT(sendDisplayTextIconMenu()) );
        }
        break;

        case IconMenu_3A:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( "Basic Icon" );
            cmd.setIconId( 1 );
            command( cmd, this, SLOT(sendDisplayTextIconMenu()) );
        }
        break;

        case IconMenu_4A:
        {
            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setClearAfterDelay( false );
            cmd.setText( QString() );
            cmd.setIconId( 1 );
            command( cmd, this, SLOT(sendDisplayTextIconMenu()) );
        }
        break;

        case IconMenu_Main:
        {
            sendDisplayTextMenu();
        }
        break;

        default:
            endSession();
        break;
    }
}