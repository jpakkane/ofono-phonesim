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

#include "simapplication.h"
#include <qatutils.h>
#include <qdebug.h>
#include <QTextCodec>
#include "qsmsmessage.h"

class SimApplicationPrivate
{
public:
    SimApplicationPrivate()
    {
        expectedType = QSimCommand::NoCommand;
        target = 0;
        slot = 0;
        inResponse = false;
    }

    SimRules   *rules;
    QSimCommand::Type expectedType;
    QByteArray  currentCommand;
    QObject    *target;
    const char *slot;
    bool inResponse;
};

SimApplication::SimApplication( SimRules *rules, QObject *parent )
    : QObject( parent )
{
    d = new SimApplicationPrivate();
    d->rules = rules;
}

SimApplication::~SimApplication()
{
    delete d;
}

/*!
    Sends a proactive SIM command, \a cmd, to the ME, and instructs
    SimApplication to invoke \a slot on \a target when the response
    arrives back.  The \a options provides extra information about
    how the command should be transmitted to the ME in PDU form.

    For \c SetupMenu commands, \a target and \a slot should be null.
    When the user responds to the main menu, mainMenuSelection() or
    mainMenuHelpRequest() will be invoked.

    \sa mainMenuSelection(), mainMenuHelpRequest()
*/
void SimApplication::command( const QSimCommand& cmd,
                              QObject *target, const char *slot,
                              QSimCommand::ToPduOptions options )
{
    // Record the command details, together with the type of
    // TERMINAL RESPONSE or ENVELOPE that we expect in answer.
    d->currentCommand = cmd.toPdu( options );
    d->expectedType = cmd.type();
    d->target = target;
    d->slot = slot;

    // Send an unsolicited notification to indicate that a new
    // proactive SIM command is available.  If we are already in
    // the middle of processing a TERMINAL RESPONSE or ENVELOPE,
    // then delay the unsolicited notification until later.
    if ( d->rules && !d->inResponse ) {
        d->rules->proactiveCommandNotify( d->currentCommand );
    }
}

/*!
    Sends a call control \a event to the ME.
*/
void SimApplication::controlEvent( const QSimControlEvent& event )
{
    if ( d->rules )
        d->rules->callControlEventNotify( event );
}

/*!
    Starts the SIM application.  The default implementation calls mainMenu().

    \sa abort(), mainMenu()
*/
void SimApplication::start()
{
    mainMenu();
}

/*!
    Aborts the SIM application and forces it to return to the main menu.
    The default implementation clears the pending command for FETCH
    and then calls mainMenu().

    This function is called whenever a \c{TERMINAL PROFILE} command is
    received from the ME.

    \sa start(), mainMenu()
*/
void SimApplication::abort()
{
    d->expectedType = QSimCommand::NoCommand;
    d->currentCommand = QByteArray();
    d->target = 0;
    d->slot = 0;
    endSession();
}

/*!
    \fn void SimApplication::mainMenu()

    Builds and sends the main menu to the ME using the command() method.

    \sa mainMenuSelection(), mainMenuHelpRequest()
*/

/*!
    Indicates that the main menu item with the identifier \a id
    has been selected.  The default implementation re-sends
    the main menu.

    \sa mainMenu(), mainMenuHelpRequest()
*/
void SimApplication::mainMenuSelection( int id )
{
    Q_UNUSED(id);
    endSession();
}

/*!
    Indicates that help has been requested for the main menu item
    with the identifier \a id.  The default implementation
    re-sends the main menu.

    \sa mainMenu(), mainMenuSelection()
*/
void SimApplication::mainMenuHelpRequest( int id )
{
    Q_UNUSED(id);
    endSession();
}

bool SimApplication::envelope( const QSimEnvelope& env )
{
    /* Process a menu selection ENVELOPE message.  We turn it into a
     * QSimTerminalResponse to make it easier to process.  */
    if ( env.type() == QSimEnvelope::EventDownload )
        return true;

    if ( env.type() != QSimEnvelope::MenuSelection )
        /* Not supported */
        return false;

    if ( d->expectedType != QSimCommand::SetupMenu )
        /* Envelope sent for the wrong type of command. */
        return false;

    d->expectedType = QSimCommand::NoCommand;
    d->currentCommand = QByteArray();
    d->target = 0;
    d->slot = 0;
    if ( env.requestHelp() )
        mainMenuHelpRequest( env.menuItem() );
    else
        mainMenuSelection( env.menuItem() );

    return true;
}

QByteArray SimApplication::fetch( bool clear )
{
    QByteArray resp = d->currentCommand;

    if ( clear )
        d->currentCommand = QByteArray();

    return resp;
}

bool SimApplication::response( const QSimTerminalResponse& resp )
{
    if ( resp.command().type() != QSimCommand::NoCommand &&
         resp.command().type() != d->expectedType )
        return false;

    // Save the target information.
    QObject *target = d->target;
    const char *slot = d->slot;

    // Clear the command details, in preparation for a new command.
    if ( resp.command().type() != QSimCommand::SetupMenu ) {
        d->expectedType = QSimCommand::NoCommand;
        d->currentCommand = QByteArray();
    }
    d->target = 0;
    d->slot = 0;

    // Process the response.
    d->inResponse = true;
    if ( target && slot ) {
        // Invoke the slot and pass "resp" to it.
        QByteArray name( slot + 1 );
        name = QMetaObject::normalizedSignature( name.constData() );
        int index = target->metaObject()->indexOfMethod( name.constData() );
        if ( index != -1 ) {
            void *args[2];
            args[0] = 0;
            args[1] = (void *)&resp;
            target->qt_metacall
                ( QMetaObject::InvokeMetaMethod, index, args );
        }
    }
    d->inResponse = false;

    // Answer the AT+CSIM command and send notification of the new command.
    if ( !d->rules )
        return false;////
    if ( d->currentCommand.isEmpty() || resp.command().type() == QSimCommand::SetupMenu ) {
        // No new command, so respond with a simple OK.
        d->rules->respond( "+CSIM: 4,9000\\n\\nOK" );
    } else {
        // There is a new command, so send back 91XX to the TERMINAL RESPONSE
        // or ENVELOPE request to indicate that we have another command to
        // be fetched.  Then send the unsolicited "*TCMD" notification.
        QByteArray data;
        data += (char)0x91;
        data += (char)(d->currentCommand.size());
        d->rules->respond( "+CSIM: " + QString::number( data.size() * 2 ) + "," +
                           QAtUtils::toHex( data ) + "\\n\\nOK" );
        d->rules->unsolicited
            ( "*TCMD: " + QString::number( d->currentCommand.size() ) );
    }

    return true;
}

void SimApplication::endSession()
{
    d->expectedType = QSimCommand::SetupMenu;
    d->rules->unsolicited("*TEND");
}

DemoSimApplication::DemoSimApplication( SimRules *rules, QObject *parent )
    : SimApplication( rules, parent ), smsDestNumber( "12345" ),
    smsText( "Hello" )
{
}

DemoSimApplication::~DemoSimApplication()
{
}

const QString DemoSimApplication::getName()
{
    return "Demo SIM Application";
}

#define MainMenu_News       1
#define MainMenu_Sports     2
#define MainMenu_Calls      3
#define MainMenu_SticksGame 4
#define MainMenu_Tones      5
#define MainMenu_Icons      6
#define MainMenu_IconsSE    7
#define MainMenu_Finance    8
#define MainMenu_Browser    9
#define MainMenu_DTMF       10
#define MainMenu_SendSS     11
#define MainMenu_Language   12
#define MainMenu_SendUSSD   13
#define MainMenu_SendSMS    14
#define MainMenu_Polling    15
#define MainMenu_Timers     16
#define MainMenu_Refresh    17
#define MainMenu_LocalInfo  18

#define SportsMenu_Chess        1
#define SportsMenu_Painting     2
#define SportsMenu_Snakes       3
#define SportsMenu_Main         4

#define CallsMenu_Normal        1
#define CallsMenu_Disconnect    2
#define CallsMenu_Hold          3

#define SendSSMenu_CB       1
#define SendSSMenu_CF       2
#define SendSSMenu_CW       3
#define SendSSMenu_CLIP     4
#define SendSSMenu_CLIR     5
#define SendSSMenu_CoLP     6
#define SendSSMenu_CoLR     7
#define SendSSMenu_CNAP     8

#define CBMenu_Activation       1
#define CBMenu_Interrogation    2
#define CBMenu_Deactivation     3

#define CFMenu_Registration     1
#define CFMenu_Activation       2
#define CFMenu_Interrogation    3
#define CFMenu_Deactivation     4
#define CFMenu_Erasure          5

#define CWMenu_Activation       1
#define CWMenu_Interrogation    2
#define CWMenu_Deactivation     3

#define CLIPMenu_Activation       1
#define CLIPMenu_Interrogation    2
#define CLIPMenu_Deactivation     3

#define CLIRMenu_Activation       1
#define CLIRMenu_Interrogation    2
#define CLIRMenu_Deactivation     3

#define CoLPMenu_Activation       1
#define CoLPMenu_Interrogation    2
#define CoLPMenu_Deactivation     3

#define CoLRMenu_Activation       1
#define CoLRMenu_Interrogation    2
#define CoLRMenu_Deactivation     3

#define CNAPMenu_Activation       1
#define CNAPMenu_Interrogation    2
#define CNAPMenu_Deactivation     3

#define Language_Specific       1
#define Language_Non_Specific   2
#define Language_Main           3

#define SendUSSD_7Bit       1
#define SendUSSD_8Bit       2
#define SendUSSD_UCS2       3
#define SendUSSD_Error      4
#define SendUSSD_Main       5

#define LocalInfoMenu_Time  1
#define LocalInfoMenu_Lang  2

enum SendSMSMenuItems {
	SendSMS_Unpacked = 1,
	SendSMS_Packed,
	SendSMS_SetDestination,
	SendSMS_SetContents,
};

enum PollingMenuItems {
	Polling_Off = 1,
	Polling_30s,
};

enum TimersMenuItems {
	Timers_Start = 1,
	Timers_Stop,
	Timers_Sleep,
	Timers_Query,
};

void DemoSimApplication::mainMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SetupMenu );

    item.setIdentifier( MainMenu_News );
    item.setLabel( "News" );
    items += item;

    item.setIdentifier( MainMenu_Sports );
    item.setLabel( "Sports" );
    items += item;

    item.setIdentifier( MainMenu_Calls );
    item.setLabel( "Calls" );
    items += item;

    item.setIdentifier( MainMenu_SticksGame );
    item.setLabel( "Sticks Game" );
    items += item;

    item.setIdentifier( MainMenu_Tones );
    item.setLabel( "Tones" );
    items += item;

    item.setIdentifier( MainMenu_Icons );
    item.setLabel( "Icons (not self-explanatory)" );
    items += item;

    item.setIdentifier( MainMenu_IconsSE );
    item.setLabel( "Icons (self-explanatory)" );
    items += item;

    item.setIdentifier( MainMenu_Finance );
    item.setLabel( "Finance" );
    items += item;

    item.setIdentifier( MainMenu_Browser );
    item.setLabel( "Web Browser" );
    items += item;

    item.setIdentifier( MainMenu_DTMF );
    item.setLabel( "DialTones" );
    items += item;

    item.setIdentifier( MainMenu_SendSS );
    item.setLabel( "Send SS" );
    items += item;

    item.setIdentifier( MainMenu_Language );
    item.setLabel( "Language Notification" );
    items += item;

    item.setIdentifier( MainMenu_SendUSSD );
    item.setLabel( "Send USSD" );
    items += item;

    item.setIdentifier( MainMenu_SendSMS );
    item.setLabel( "Send SMS request" );
    items += item;

    item.setIdentifier( MainMenu_Polling );
    item.setLabel( "SIM Polling" );
    items += item;

    item.setIdentifier( MainMenu_Timers );
    item.setLabel( "Timers" );
    items += item;

    item.setIdentifier( MainMenu_Refresh );
    item.setLabel( "SIM Refresh" );
    items += item;

    item.setIdentifier( MainMenu_LocalInfo );
    item.setLabel( "Provide Local Information" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, 0, 0 );
}

void DemoSimApplication::sendDisplayText()
{
    // Display a text string and then go back to the main menu once the
    // text is accepted by the user.
    QSimCommand cmd;
    cmd.setType( QSimCommand::DisplayText );
    cmd.setDestinationDevice( QSimCommand::Display );
    cmd.setClearAfterDelay(false);
    cmd.setImmediateResponse(true);
    cmd.setHighPriority(false);
    immediateResponse = true;
    cmd.setText( "Police today arrested a man on suspicion "
            "of making phone calls while intoxicated.  Witnesses claimed "
            "that they heard the man exclaim \"I washent dwinkn!\" as "
            "officers escorted him away." );
    command( cmd, this, SLOT(displayTextResponse(QSimTerminalResponse)) );
}

void DemoSimApplication::mainMenuSelection( int id )
{
    QSimCommand cmd;

    switch ( id ) {

        case MainMenu_News:
        {
            QTimer::singleShot( 0, this, SLOT(sendDisplayText()) );
        }
        break;

        case MainMenu_Sports:
        {
            sendSportsMenu();
        }
        break;

        case MainMenu_Calls:
        {
            sendCallsMenu();
        }
        break;

        case MainMenu_SticksGame:
        {
            startSticksGame();
        }
        break;

        case MainMenu_Tones:
        {
            sendToneMenu();
        }
        break;

        case MainMenu_Icons:
        {
            sendIconMenu();
        }
        break;

        case MainMenu_IconsSE:
        {
            sendIconSEMenu();
        }
        break;

        case MainMenu_Finance:
        {
            cmd.setType( QSimCommand::GetInput );
            cmd.setText( "Enter code" );
            cmd.setWantDigits( true );
            cmd.setMinimumLength( 3 );
            cmd.setHasHelp( true );
            command( cmd, this, SLOT(getInputLoop(QSimTerminalResponse)) );
        }
        break;

        case MainMenu_Browser:
        {
            sendBrowserMenu();
        }
        break;

        case MainMenu_DTMF:
        {
            sendDTMF();
        }
        break;

        case MainMenu_SendSS:
        {
            sendSendSSMenu();
        }
        break;

        case MainMenu_Language:
        {
            sendLanguageMenu();
        }
        break;

        case MainMenu_SendUSSD:
        {
            sendUSSDMenu();
        }
        break;

        case MainMenu_SendSMS:
        {
            sendSMSMenu();
        }
        break;

        case MainMenu_Polling:
        {
            sendPollingMenu();
        }
        break;

        case MainMenu_Timers:
        {
            sendTimersMenu();
        }
        break;

        case MainMenu_Refresh:
        {
            sendRefreshMenu();
        }
        break;

        case MainMenu_LocalInfo:
        {
            sendLocalInfoMenu();
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

void DemoSimApplication::sendSportsMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Sports" );

    item.setIdentifier( SportsMenu_Chess );
    item.setLabel( "Chess" );
    items += item;

    item.setIdentifier( SportsMenu_Painting );
    item.setLabel( "Finger Painting" );
    items += item;

    item.setIdentifier( SportsMenu_Snakes );
    item.setLabel( "Snakes and Ladders" );
    items += item;

    item.setIdentifier( SportsMenu_Main );
    item.setLabel( "Return to main menu" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(sportsMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::sportsMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        // Item selected.
        switch ( resp.menuItem() ) {

            case SportsMenu_Chess:
            {
                cmd.setType( QSimCommand::DisplayText );
                cmd.setDestinationDevice( QSimCommand::Display );
                cmd.setText( "Kasparov 3, Deep Blue 4" );
                command( cmd, this, SLOT(sendSportsMenu()) );
            }
            break;

            case SportsMenu_Painting:
            {
                cmd.setType( QSimCommand::DisplayText );
                cmd.setDestinationDevice( QSimCommand::Display );
                cmd.setText( "Little Johnny 4, Little Sally 6" );
                command( cmd, this, SLOT(sendSportsMenu()) );
            }
            break;

            case SportsMenu_Snakes:
            {
                cmd.setType( QSimCommand::DisplayText );
                cmd.setDestinationDevice( QSimCommand::Display );
                cmd.setText( "Little Johnny 0, Little Sally 2" );
                cmd.setClearAfterDelay( true );
                command( cmd, this, SLOT(sendSportsMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else {
        // Unknown response - just go back to the main menu.
        endSession();
    }
}

void DemoSimApplication::sendCallsMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Setup Call Menu" );

    item.setIdentifier( CallsMenu_Normal );
    item.setLabel( "Normal" );
    items += item;

    item.setIdentifier( CallsMenu_Disconnect );
    item.setLabel( "Disconnect other calls first" );
    items += item;

    item.setIdentifier( CallsMenu_Hold );
    item.setLabel( "Hold other calls first" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(callsMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::callsMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        // Item selected.
        switch ( resp.menuItem() ) {

            case CallsMenu_Normal:
            {
                cmd.setType( QSimCommand::SetupCall );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "1194" );
                cmd.setText( "Call 1194?" );
		cmd.setOtherText( "Normal call" );
                command( cmd, this, SLOT(endSession()) );
            }
            break;

            case CallsMenu_Disconnect:
            {
                cmd.setType( QSimCommand::SetupCall );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "1194" );
                cmd.setText( "Call 1194?" );
		cmd.setOtherText( "Disconnect others, then call" );
		cmd.setDisposition( QSimCommand::Disconnect );
                command( cmd, this, SLOT(endSession()) );
            }
            break;

            case CallsMenu_Hold:
            {
                cmd.setType( QSimCommand::SetupCall );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "1194" );
                cmd.setText( "Call 1194?" );
		cmd.setOtherText( "Hold others, then call" );
		cmd.setDisposition( QSimCommand::PutOnHold );
                command( cmd, this, SLOT(endSession()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else {
        // Unknown response - just go back to the main menu.
        endSession();
    }
}

void DemoSimApplication::startSticksGame()
{
    sticksLeft = 21;
    sticksGameShow();
}

void DemoSimApplication::sticksGameShow()
{
    QSimCommand cmd;
    if ( sticksLeft == 1 ) {
        cmd.setType( QSimCommand::GetInkey );
        cmd.setText( "There is only 1 stick left.  You lose.  Play again?" );
        cmd.setWantYesNo( true );
        command( cmd, this, SLOT(sticksGamePlayAgain(QSimTerminalResponse)) );
    } else {
        cmd.setType( QSimCommand::GetInkey );
        cmd.setText( "There are 21 sticks left.  How many do you take (1, 2, or 3)?" );
        cmd.setWantDigits( true );
        if ( sticksLeft == 21 )
            cmd.setHasHelp( true );
        command( cmd, this, SLOT(sticksGameLoop(QSimTerminalResponse)) );
    }
}

void DemoSimApplication::sticksGameLoop( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;
    if ( resp.result() == QSimTerminalResponse::Success ) {
        // User has selected the number of sticks they want.
        int taken = 0;
        if ( resp.text() == "1" ) {
            taken = 1;
        } else if ( resp.text() == "2" ) {
            taken = 2;
        } else if ( resp.text() == "3" ) {
            taken = 3;
        } else {
            cmd.setType( QSimCommand::GetInkey );
            cmd.setText( "Must be 1, 2, or 3.  There are " + QString::number( sticksLeft ) +
                         " sticks left.  How many sticks do you take?" );
            cmd.setWantDigits( true );
            command( cmd, this, SLOT(sticksGameLoop(QSimTerminalResponse)) );
            return;
        }
        cmd.setType( QSimCommand::DisplayText );
        cmd.setDestinationDevice( QSimCommand::Display );
        cmd.setText( "I take " + QString::number( 4 - taken ) + " sticks." );
        cmd.setClearAfterDelay( true );
        sticksLeft -= 4;
        command( cmd, this, SLOT(sticksGameShow()) );
    } else if ( resp.result() == QSimTerminalResponse::HelpInformationRequested ) {
        // Display help for the game.
        cmd.setType( QSimCommand::DisplayText );
        cmd.setDestinationDevice( QSimCommand::Display );
        cmd.setText( "Starting with 21 sticks, players pick up 1, 2, or 3 sticks at a time.  "
                     "The loser is the player who has to pick up the last stick." );
        command( cmd, this, SLOT(startSticksGame()) );
    } else {
        // Probably aborted.
        endSession();
    }
}

void DemoSimApplication::sticksGamePlayAgain( const QSimTerminalResponse& resp )
{
    if ( resp.text() == "Yes" )
        startSticksGame();
    else
        endSession();
}

void DemoSimApplication::getInputLoop( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;
    if ( resp.result() == QSimTerminalResponse::HelpInformationRequested ) {
        // Display help for the game.
        cmd.setType( QSimCommand::DisplayText );
        cmd.setDestinationDevice( QSimCommand::Display );
        cmd.setText("Enter code of the company." );
        command( cmd, this, SLOT(endSession()) );
    } else {
        endSession();
    }
}

void DemoSimApplication::sendToneMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Tones" );

    item.setIdentifier( (uint)QSimCommand::ToneDial );
    item.setLabel( "Dial" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneBusy );
    item.setLabel( "Busy" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneCongestion );
    item.setLabel( "Congestion" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneRadioAck );
    item.setLabel( "Radio Ack" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneDropped );
    item.setLabel( "Dropped" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneError );
    item.setLabel( "Error" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneCallWaiting );
    item.setLabel( "Call Waiting" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneGeneralBeep );
    item.setLabel( "General Beep" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::TonePositiveBeep );
    item.setLabel( "Positive Beep" );
    items += item;

    item.setIdentifier( (uint)QSimCommand::ToneNegativeBeep );
    item.setLabel( "Negative Beep" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(toneMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::toneMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        // Item selected.
        cmd.setType( QSimCommand::PlayTone );
        cmd.setDestinationDevice( QSimCommand::Earpiece );
        cmd.setTone( (QSimCommand::Tone)( resp.menuItem() ) );
        if ( cmd.tone() == QSimCommand::ToneDial )
            cmd.setDuration( 5000 );
        else if ( cmd.tone() == QSimCommand::ToneGeneralBeep ||
                  cmd.tone() == QSimCommand::TonePositiveBeep ||
                  cmd.tone() == QSimCommand::ToneNegativeBeep )
            cmd.setDuration( 1000 );
        else
            cmd.setDuration( 3000 );
        command( cmd, this, SLOT(sendToneMenu()) );
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        // Request to move backward.
        endSession();
    } else {
        // Unknown response - just go back to the main menu.
        endSession();
    }
}

void DemoSimApplication::sendDTMF()
{
    QSimCommand cmd;

    cmd.setType( QSimCommand::SendDTMF );
    cmd.setDestinationDevice( QSimCommand::Network );
    cmd.setNumber( "1p234ppp5" );
    cmd.setText( "Sending DTMFs to network" );

    command( cmd, this, SLOT(endSession()) );
}

void DemoSimApplication::sendIconMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Icons" );

    item.setIdentifier( 1 );
    item.setLabel( "Basic Icon" );
    item.setIconId( 1 );
    items += item;

    item.setIdentifier( 2 );
    item.setLabel( "Color Icon" );
    item.setIconId( 2 );
    items += item;

    item.setIdentifier( 3 );
    item.setLabel( "Bad Icon" );
    item.setIconId( 70 );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(iconMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::sendIconSEMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Icons SE" );

    item.setIdentifier( 1 );
    item.setLabel( "Basic Icon" );
    item.setIconId( 1 );
    item.setIconSelfExplanatory( true );
    items += item;

    item.setIdentifier( 2 );
    item.setLabel( "Color Icon" );
    item.setIconId( 2 );
    item.setIconSelfExplanatory( true );
    items += item;

    item.setIdentifier( 3 );
    item.setLabel( "Bad Icon" );
    item.setIconId( 70 );
    item.setIconSelfExplanatory( true );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(iconSEMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::iconMenu( const QSimTerminalResponse& resp )
{
    if ( resp.result() == QSimTerminalResponse::Success )
        sendIconMenu();
    else
        endSession();
}

void DemoSimApplication::iconSEMenu( const QSimTerminalResponse& resp )
{
    if ( resp.result() == QSimTerminalResponse::Success )
        sendIconSEMenu();
    else
        endSession();
}

void DemoSimApplication::displayTextResponse( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        endSession();
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        // Request to move backward.
        endSession();
    } else {
        // Unknown response - just go back to the main menu.
        endSession();
    }
}

void DemoSimApplication::sendBrowserMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Web Browser" );

    item.setIdentifier( 1 );
    item.setLabel( "Qt Extended" );
    items += item;

    item.setIdentifier( 2 );
    item.setLabel( "Google (normal)" );
    items += item;

    item.setIdentifier( 3 );
    item.setLabel( "Google (if browser not in use)" );
    items += item;

    item.setIdentifier( 4 );
    item.setLabel( "Google (clear history)" );
    items += item;

    item.setIdentifier( 5 );
    item.setLabel( "Default Home Page" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(browserMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::browserMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        // Item selected.
        switch ( resp.menuItem() ) {

            case 1:
            {
                cmd.setType( QSimCommand::LaunchBrowser );
                cmd.setText( "Qt Extended" );
                cmd.setBrowserLaunchMode( QSimCommand::UseExisting );
                cmd.setUrl( "http://www.qtextended.org/" );
                command( cmd, this, SLOT(sendBrowserMenu()) );
            }
            break;

            case 2:
            {
                cmd.setType( QSimCommand::LaunchBrowser );
                cmd.setText( "Google" );
                cmd.setBrowserLaunchMode( QSimCommand::UseExisting );
                cmd.setUrl( "http://www.google.com/" );
                command( cmd, this, SLOT(sendBrowserMenu()) );
            }
            break;

            case 3:
            {
                cmd.setType( QSimCommand::LaunchBrowser );
                cmd.setText( "Google" );
                cmd.setBrowserLaunchMode( QSimCommand::IfNotAlreadyLaunched );
                cmd.setUrl( "http://www.google.com/" );
                command( cmd, this, SLOT(sendBrowserMenu()) );
            }
            break;

            case 4:
            {
                cmd.setType( QSimCommand::LaunchBrowser );
                cmd.setText( "Google" );
                cmd.setBrowserLaunchMode( QSimCommand::CloseExistingAndLaunch );
                cmd.setUrl( "http://www.google.com/" );
                command( cmd, this, SLOT(sendBrowserMenu()) );
            }
            break;

            case 5:
            {
                cmd.setType( QSimCommand::LaunchBrowser );
                cmd.setText( "Default Home Page" );
                cmd.setBrowserLaunchMode( QSimCommand::UseExisting );
                cmd.setUrl( "" );
                command( cmd, this, SLOT(sendBrowserMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        // Request to move backward.
        endSession();
    } else {
        // Unknown response - just go back to the main menu.
        endSession();
    }
}

void DemoSimApplication::sendSendSSMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Send SS" );

    item.setIdentifier( SendSSMenu_CB );
    item.setLabel( "CB (Call Barring)" );
    items += item;

    item.setIdentifier( SendSSMenu_CF );
    item.setLabel( "CF (Call Forwarding)" );
    items += item;

    item.setIdentifier( SendSSMenu_CW );
    item.setLabel( "CW (Call Waiting)" );
    items += item;

    item.setIdentifier( SendSSMenu_CLIP );
    item.setLabel( "CLIP (Calling Line Identification Presentation)" );
    items += item;

    item.setIdentifier( SendSSMenu_CNAP );
    item.setLabel( "CNAP (Calling Name Identification Presentation)" );
    items += item;

    item.setIdentifier( SendSSMenu_CLIR );
    item.setLabel( "CLIR (Calling Line Identification Restriction)" );
    items += item;

    item.setIdentifier( SendSSMenu_CoLP );
    item.setLabel( "CoLP (Connected Line Identification Presentation)" );
    items += item;

    item.setIdentifier( SendSSMenu_CoLR );
    item.setLabel( "CoLR (Connected Line Identification Restriction)" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(sendSSMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::sendSSMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case SendSSMenu_CB:
            {
                sendCBMenu();
            }
            break;

            case SendSSMenu_CF:
            {
                sendCFMenu();
            }
            break;

            case SendSSMenu_CW:
            {
                sendCWMenu();
            }
            break;

            case SendSSMenu_CLIP:
            {
                sendCLIPMenu();
            }
            break;

            case SendSSMenu_CLIR:
            {
                sendCLIRMenu();
            }
            break;

            case SendSSMenu_CoLP:
            {
                sendCoLPMenu();
            }
            break;

            case SendSSMenu_CoLR:
            {
                sendCoLRMenu();
            }
            break;

            default:
                endSession();
                break;
        }
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCBMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CB" );

    item.setIdentifier( CBMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CBMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CBMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CBMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CBMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CBMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*33*3579#" );
                command( cmd, this, SLOT(sendCBMenu()) );
            }
            break;

            case CBMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#33#" );
                command( cmd, this, SLOT(sendCBMenu()) );
            }
                break;

            case CBMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#33*3579#" );
                command( cmd, this, SLOT(sendCBMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCFMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CF" );

    item.setIdentifier( CFMenu_Registration );
    item.setLabel( "Registration" );
    items += item;

    item.setIdentifier( CFMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CFMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CFMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    item.setIdentifier( CFMenu_Erasure );
    item.setLabel( "Erasure" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CFMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CFMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CFMenu_Registration:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "**62*+155543*11#" );
                command( cmd, this, SLOT(sendCFMenu()) );
            }
            break;

            case CFMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*62#" );
                command( cmd, this, SLOT(sendCFMenu()) );
            }
            break;

            case CFMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#62**11#" );
                command( cmd, this, SLOT(sendCFMenu()) );
            }
            break;

            case CFMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#62#" );
                command( cmd, this, SLOT(sendCFMenu()) );
            }
            break;

            case CFMenu_Erasure:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "##62#" );
                command( cmd, this, SLOT(sendCFMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCWMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CW" );

    item.setIdentifier( CWMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CWMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CWMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CWMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CWMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CWMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*43#" );
                command( cmd, this, SLOT(sendCWMenu()) );
            }
            break;

            case CWMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#43#" );
                command( cmd, this, SLOT(sendCWMenu()) );
            }
            break;

            case CWMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#43#" );
                command( cmd, this, SLOT(sendCWMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCLIPMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CLIP" );

    item.setIdentifier( CLIPMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CLIPMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CLIPMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CLIPMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CLIPMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CLIPMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*30#" );
                command( cmd, this, SLOT(sendCLIPMenu()) );
            }
            break;

            case CLIPMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#30#" );
                command( cmd, this, SLOT(sendCLIPMenu()) );
            }
            break;

            case CLIPMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#30#" );
                command( cmd, this, SLOT(sendCLIPMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCLIRMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CLIR" );

    item.setIdentifier( CLIRMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CLIRMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CLIRMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CLIRMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CLIRMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CLIRMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*31#" );
                command( cmd, this, SLOT(sendCLIRMenu()) );
            }
            break;

            case CLIRMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#31#" );
                command( cmd, this, SLOT(sendCLIRMenu()) );
            }
            break;

            case CLIRMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#31#" );
                command( cmd, this, SLOT(sendCLIRMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCoLPMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CoLP" );

    item.setIdentifier( CoLPMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CoLPMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CoLPMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CoLPMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CoLPMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CoLPMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*76#" );
                command( cmd, this, SLOT(sendCoLPMenu()) );
            }
            break;

            case CoLPMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#76#" );
                command( cmd, this, SLOT(sendCoLPMenu()) );
            }
            break;

            case CoLPMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#76#" );
                command( cmd, this, SLOT(sendCoLPMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCoLRMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CoLR" );

    item.setIdentifier( CoLRMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CoLRMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CoLRMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CoLRMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CoLRMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CoLRMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*77#" );
                command( cmd, this, SLOT(sendCoLRMenu()) );
            }
            break;

            case CoLRMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#77#" );
                command( cmd, this, SLOT(sendCoLRMenu()) );
            }
            break;

            case CoLRMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#77#" );
                command( cmd, this, SLOT(sendCoLRMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendCNAPMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "CNAP" );

    item.setIdentifier( CNAPMenu_Activation );
    item.setLabel( "Activation" );
    items += item;

    item.setIdentifier( CNAPMenu_Interrogation );
    item.setLabel( "Interrogation" );
    items += item;

    item.setIdentifier( CNAPMenu_Deactivation );
    item.setLabel( "Deactivation" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(CNAPMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::CNAPMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case CNAPMenu_Activation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*300#" );
                command( cmd, this, SLOT(sendCNAPMenu()) );
            }
            break;

            case CNAPMenu_Interrogation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "*#300#" );
                command( cmd, this, SLOT(sendCNAPMenu()) );
            }
            break;

            case CNAPMenu_Deactivation:
            {
                cmd.setType( QSimCommand::SendSS );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setNumber( "#300#" );
                command( cmd, this, SLOT(sendCNAPMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSendSSMenu();
    } else {
        endSession();
    }
}

void DemoSimApplication::sendLanguageMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Language Notification" );

    item.setIdentifier( Language_Specific );
    item.setLabel( "Specific Language" );
    items += item;

    item.setIdentifier( Language_Non_Specific );
    item.setLabel( "Non-Specific Language" );
    items += item;

    item.setIdentifier( Language_Main );
    item.setLabel( "Return to main menu" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(languageMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::languageMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {

        // Item selected.
        switch ( resp.menuItem() ) {
            case Language_Specific:
            {
                cmd.setType( QSimCommand::LanguageNotification );
                cmd.setQualifier( 1 );
                cmd.setLanguage( "se" );
                command( cmd, this, SLOT(sendLanguageMenu()) );
            }
            break;

            case Language_Non_Specific:
            {
                cmd.setType( QSimCommand::LanguageNotification );
                cmd.setQualifier( 0 );
                command( cmd, this, SLOT(sendLanguageMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else {
        // Unknown response - just go back to the main menu.
        endSession();
    }
}

void DemoSimApplication::sendUSSDMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Send USSD" );

    item.setIdentifier( SendUSSD_7Bit );
    item.setLabel( "Send USSD - 7-Bit" );
    items += item;

    item.setIdentifier( SendUSSD_8Bit );
    item.setLabel( "Send USSD - 8-Bit" );
    items += item;

    item.setIdentifier( SendUSSD_UCS2 );
    item.setLabel( "Send USSD - UCS2" );
    items += item;

    item.setIdentifier( SendUSSD_Error );
    item.setLabel( "Send USSD - Return Error" );
    items += item;

    item.setIdentifier( SendUSSD_Main );
    item.setLabel( "Return to main menu" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(USSDMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::USSDMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {

        // Item selected.
        switch ( resp.menuItem() ) {
            case SendUSSD_7Bit:
            {
                cmd.setType( QSimCommand::SendUSSD );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setText( "7-bit USSD" );
                cmd.setNumber( "ABCD" );
                command( cmd, this, SLOT(sendUSSDMenu()),
                         QSimCommand::PackedStrings );
            }
            break;

            case SendUSSD_8Bit:
            {
                cmd.setType( QSimCommand::SendUSSD );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setText( "8-bit USSD" );
                cmd.setNumber( "ABCD" );
                command( cmd, this, SLOT(sendUSSDMenu()) );
            }
            break;

            case SendUSSD_UCS2:
            {
                cmd.setType( QSimCommand::SendUSSD );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setText( "UCS2 USSD" );
                QTextCodec *codec = QTextCodec::codecForName( "utf8" );
                cmd.setNumber( codec->toUnicode( "ЗДРАВСТВУЙТЕ" ) );
                command( cmd, this, SLOT(sendUSSDMenu()),
                         QSimCommand::UCS2Strings );
            }
            break;

            case SendUSSD_Error:
            {
                cmd.setType( QSimCommand::SendUSSD );
                cmd.setDestinationDevice( QSimCommand::Network );
                cmd.setText( "7-bit USSD" );
                cmd.setNumber( "*100#" );
                command( cmd, this, SLOT(sendUSSDMenu()),
                         QSimCommand::PackedStrings );
            }
            break;

            default:
                endSession();
                break;
        }
    } else {
        // Unknown response - just go back to the main menu.
        endSession();
    }
}

void DemoSimApplication::sendSMSMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Send SMS" );

    item.setIdentifier( SendSMS_Unpacked );
    item.setLabel( "Unpacked" );
    items += item;

    item.setIdentifier( SendSMS_Packed );
    item.setLabel( "Packed text" );
    items += item;

    item.setIdentifier( SendSMS_SetDestination );
    item.setLabel( "Set destination (" + smsDestNumber + ")" );
    items += item;

    item.setIdentifier( SendSMS_SetContents );
    item.setLabel( "Set content text" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(smsMenuResp(QSimTerminalResponse)) );
}

void DemoSimApplication::smsMenuResp( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    /* Item selected. */
    switch ( resp.menuItem() ) {
    case SendSMS_Unpacked:
        break;

    case SendSMS_Packed:
        cmd.setSmsPacking( true );
        break;

    case SendSMS_SetDestination:
        cmd.setType( QSimCommand::GetInput );
        cmd.setText( "Enter recipient number" );
        cmd.setWantDigits( true );
        cmd.setMinimumLength( 2 );
        cmd.setMaximumLength( 20 );
        cmd.setDefaultText( smsDestNumber );
        command( cmd, this, SLOT(smsSetDestResp(QSimTerminalResponse)) );
        return;

    case SendSMS_SetContents:
        cmd.setType( QSimCommand::GetInput );
        cmd.setText( "Enter message text" );
        cmd.setMaximumLength( 100 );
        cmd.setDefaultText( smsText );
        command( cmd, this, SLOT(smsSetTextResp(QSimTerminalResponse)) );
        return;
    }

    QSMSMessage sms;
    sms.setValidityPeriod( -1 );
    sms.setMessageClass( 2 );
    sms.setProtocol( 0 );
    sms.setRecipient( smsDestNumber );
    sms.setText( smsText );
    sms.setForceGsm( false );
    sms.setBestScheme( QSMS_8BitAlphabet );
    sms.setDataCodingScheme( 0xf6 );

    cmd.setType( QSimCommand::SendSMS );
    cmd.setText( "Sending an SMS to our friends at " + smsDestNumber );
    cmd.setNumber( "123" );
    cmd.addExtensionField( 0x8b, sms.toPdu().mid( 1 ) );
    cmd.setDestinationDevice( QSimCommand::Network );

    command( cmd, this, SLOT(endSession()) );
}

void DemoSimApplication::smsSetDestResp( const QSimTerminalResponse& resp )
{
    if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSMSMenu();

        return;
    }

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    smsDestNumber = resp.text();
    sendSMSMenu();
}

void DemoSimApplication::smsSetTextResp( const QSimTerminalResponse& resp )
{
    if ( resp.result() == QSimTerminalResponse::BackwardMove ) {
        sendSMSMenu();

        return;
    }

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    smsText = resp.text();
    sendSMSMenu();
}

void DemoSimApplication::sendPollingMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Polling Menu" );

    item.setIdentifier( Polling_Off );
    item.setLabel( "Polling Off" );
    items += item;

    item.setIdentifier( Polling_30s );
    item.setLabel( "Poll Interval of 30s" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(pollingMenuResp(QSimTerminalResponse)) );
}

void DemoSimApplication::pollingMenuResp( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    /* Item selected. */
    switch ( resp.menuItem() ) {
    case Polling_Off:
        cmd.setType( QSimCommand::PollingOff );
        cmd.setDestinationDevice( QSimCommand::ME );
        command( cmd, this, SLOT(endSession()) );
        break;

    case Polling_30s:
        cmd.setType( QSimCommand::PollInterval );
        cmd.setDuration( 30000 );
        cmd.setDestinationDevice( QSimCommand::ME );
        command( cmd, this, SLOT(endSession()) );
        break;
    }
}

void DemoSimApplication::sendTimersMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Timer ops" );

    item.setIdentifier( Timers_Start );
    item.setLabel( "Reset timer 1 to 1h" );
    items += item;

    item.setIdentifier( Timers_Stop );
    item.setLabel( "Stop all timers" );
    items += item;

    item.setIdentifier( Timers_Sleep );
    item.setLabel( "Sleep for 10s using timer 2" );
    items += item;

    item.setIdentifier( Timers_Query );
    item.setLabel( "Show statuses" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(timersMenuResp(QSimTerminalResponse)) );
}

void DemoSimApplication::timersMenuResp( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    /* Item selected. */
    switch ( resp.menuItem() ) {
    case Timers_Start:
        cmd.setQualifier( 0 );
        cmd.setType( QSimCommand::TimerManagement );
        cmd.setTimerId( 1 );
        cmd.setDuration( 3600000 );
        cmd.setDestinationDevice( QSimCommand::ME );
        command( cmd, this, SLOT(endSession()) );
        break;

    case Timers_Stop:
        cmd.setQualifier( 1 );
        cmd.setType( QSimCommand::TimerManagement );
        cmd.setTimerId( 1 );
        cmd.setDestinationDevice( QSimCommand::ME );
        command( cmd, this, SLOT(timersCmdResp(QSimTerminalResponse)) );
        break;

    case Timers_Sleep:
        cmd.setQualifier( 0 );
        cmd.setType( QSimCommand::TimerManagement );
        cmd.setTimerId( 2 );
        cmd.setDuration( 10000 );
        cmd.setDestinationDevice( QSimCommand::ME );
        command( cmd, NULL, NULL );
        break;

    case Timers_Query:
        cmd.setQualifier( 2 );
        cmd.setType( QSimCommand::TimerManagement );
        cmd.setTimerId( 1 );
        cmd.setDestinationDevice( QSimCommand::ME );
        command( cmd, this, SLOT(timersCmdResp(QSimTerminalResponse)) );
	timerStatus = "";
        break;
    }
}

void DemoSimApplication::timersCmdResp( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.command().qualifier() == 1 ) {
        if ( resp.command().timerId() < 1 ) {
            endSession();
            return;
        }

        /* Stop the next timer */
        cmd.setQualifier( 1 );
        cmd.setType( QSimCommand::TimerManagement );
        cmd.setTimerId( resp.timerId() + 1 );
        cmd.setDestinationDevice( QSimCommand::ME );

        if ( cmd.timerId() >= 8 )
            command( cmd, this, SLOT(endSession()) );
        else
            command( cmd, this, SLOT(timersCmdResp(QSimTerminalResponse)) );

        return;
    }

    if ( resp.command().qualifier() == 2 ) {
        if ( resp.result() == QSimTerminalResponse::Success ) {
            QString status;
            status.sprintf( "Timer %i expires in %i:%02i:%02i.\n",
                    resp.timerId(), resp.duration() / 3600000,
                    (resp.duration() / 60000) % 60,
                    (resp.duration() / 1000) % 60 );

            timerStatus += status;
        }

        if ( resp.timerId() == 8 || resp.timerId() < 1 ) {
            /* All timers done */
            if ( timerStatus.length() == 0 )
                timerStatus = "All timers are stopped.";

            cmd.setType( QSimCommand::DisplayText );
            cmd.setDestinationDevice( QSimCommand::Display );
            cmd.setText( timerStatus.left(220) );
            command( cmd, this, SLOT(endSession()) );
            return;
        }

        /* Interrogate the next timer */
        cmd.setQualifier( 2 );
        cmd.setType( QSimCommand::TimerManagement );
        cmd.setTimerId( resp.timerId() + 1 );
        cmd.setDestinationDevice( QSimCommand::ME );
        command( cmd, this, SLOT(timersCmdResp(QSimTerminalResponse)) );

        return;
    }
}

bool DemoSimApplication::envelope( const QSimEnvelope& env )
{
    if ( env.type() != QSimEnvelope::TimerExpiration )
        return SimApplication::envelope( env );

    QSimCommand cmd;

    cmd.setType( QSimCommand::DisplayText );
    cmd.setDestinationDevice( QSimCommand::Display );
    cmd.setText( "Timer expired." );
    command( cmd, this, SLOT(endSession()) );

    return true;
}

void DemoSimApplication::sendRefreshMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "SIM Refresh menu" );

    /* Use qualifier value + 1 for id */
    item.setIdentifier( 1 );
    item.setLabel( "NAA Initialization+Full File Change Notification" );
    items += item;

    item.setIdentifier( 2 );
    item.setLabel( "File Change Notification (EFmsisdn,EFecc,EFfdn)" );
    items += item;

    item.setIdentifier( 3 );
    item.setLabel( "NAA Initialization+File Change Notification" );
    items += item;

    item.setIdentifier( 4 );
    item.setLabel( "NAA Initialization" );
    items += item;

    item.setIdentifier( 5 );
    item.setLabel( "UICC Reset" );
    items += item;

    item.setIdentifier( 6 );
    item.setLabel( "NAA Application Reset" );
    items += item;

    item.setIdentifier( 7 );
    item.setLabel( "NAA Session Reset" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(refreshMenuResp(QSimTerminalResponse)) );
}

void DemoSimApplication::refreshMenuResp( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() != QSimTerminalResponse::Success ) {
        /* Unknown response - just go back to the main menu. */
        endSession();

        return;
    }

    /* Item selected. */

    cmd.setType( QSimCommand::Refresh );
    cmd.setQualifier( resp.menuItem() - 1 );
    cmd.setDestinationDevice( QSimCommand::ME );
    cmd.setText( "" );

    if ( cmd.refreshType() == QSimCommand::FileChange ||
            cmd.refreshType() == QSimCommand::InitAndFileChange ||
            cmd.refreshType() == QSimCommand::NaaSessionReset ) {
        QByteArray files;
        files += (char) 0x03;
        /* EFmsisdn */
        files += (char) 0x3f;
        files += (char) 0x00;
        files += (char) 0x7f;
        files += (char) 0xff;
        files += (char) 0x6f;
        files += (char) 0x40;
        /* EFecc */
        files += (char) 0x3f;
        files += (char) 0x00;
        files += (char) 0x7f;
        files += (char) 0xff;
        files += (char) 0x6f;
        files += (char) 0xb7;
        /* EFfdn */
        files += (char) 0x3f;
        files += (char) 0x00;
        files += (char) 0x7f;
        files += (char) 0xff;
        files += (char) 0x6f;
        files += (char) 0x3b;
        cmd.addExtensionField( 0x92, files );
    }

    command( cmd, this, SLOT(endSession()) );
}

void DemoSimApplication::sendLocalInfoMenu()
{
    QSimCommand cmd;
    QSimMenuItem item;
    QList<QSimMenuItem> items;

    cmd.setType( QSimCommand::SelectItem );
    cmd.setTitle( "Provide Local Information" );

    item.setIdentifier( LocalInfoMenu_Time );
    item.setLabel( "Date, time and time zone" );
    items += item;

    item.setIdentifier( LocalInfoMenu_Lang );
    item.setLabel( "Language setting" );
    items += item;

    cmd.setMenuItems( items );

    command( cmd, this, SLOT(localInfoMenu(QSimTerminalResponse)) );
}

void DemoSimApplication::localInfoMenu( const QSimTerminalResponse& resp )
{
    QSimCommand cmd;

    if ( resp.result() == QSimTerminalResponse::Success ) {
        switch ( resp.menuItem() ) {

            case LocalInfoMenu_Time:
            {
                cmd.setType( QSimCommand::ProvideLocalInformation );
                cmd.setQualifier( 0x03 );
                command( cmd, this, SLOT(sendLocalInfoMenu()) );
            }
            break;

            case LocalInfoMenu_Lang:
            {
                cmd.setType( QSimCommand::ProvideLocalInformation );
                cmd.setQualifier( 0x04 );
                command( cmd, this, SLOT(sendLocalInfoMenu()) );
            }
            break;

            default:
                endSession();
                break;
        }
    } else {
        endSession();
    }
}
