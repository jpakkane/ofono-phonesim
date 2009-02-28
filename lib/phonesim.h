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

#ifndef PHONESIM_H
#define PHONESIM_H

#include <qstring.h>
#include <qlist.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qxml.h>
#include <qtcpsocket.h>
#include <qapplication.h>
#include <qmap.h>
#include <qtimer.h>

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class SimState;
class SimItem;
class SimChat;
class SimUnsolicited;
class SimRules;
class SimFileSystem;
class CallManager;
class SimApplication;


class SimXmlNode
{
public:
    SimXmlNode( const QString& tag );
    ~SimXmlNode();

    SimXmlNode *parent, *next, *children, *attributes;
    QString tag;
    QString contents;

    void addChild( SimXmlNode *child );
    void addAttribute( SimXmlNode *child );
    QString getAttribute( const QString& name );
};


class SimXmlHandler : public QXmlDefaultHandler
{
public:
    SimXmlHandler();
    ~SimXmlHandler();

    bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
    bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName );
    bool characters( const QString& ch );
    bool ignorableWhitespace( const QString& ch );

    SimXmlNode *documentElement() const;

private:
    SimXmlNode *tree;
    SimXmlNode *current;
};


class SimState
{
    friend class SimRules;
public:
    SimState( SimRules *rules, SimXmlNode& e );
    ~SimState() {}

    // Get the rules object that contains this state.
    SimRules *rules() const { return _rules; }

    // Get the name of this state.
    QString name() const { return _name; }

    // Enter this state, and enable unsolicited events.
    void enter();

    // Leave this state, after disabling unsolicited events.
    void leave();

    // Handle a command.  Returns false if the command was not understood.
    bool command( const QString& cmd );

private:
    SimRules *_rules;
    QString _name;
    QList<SimItem *> items;

};


class SimItem : public QObject
{
    Q_OBJECT
public:
    SimItem( SimState *state ) { _state = state; }
    virtual ~SimItem() {}

    // Get the state that contains this item.
    SimState *state() { return _state; }

    // Receive notification of the item's state being entered.
    virtual void enter() {}

    // Receive notification of the item's state being left.
    virtual void leave() {}

    // Attempt to handle a command.  Returns false if not recognised.
    virtual bool command( const QString& ) { return false; }

private:
    SimState *_state;

};


class SimChat : public SimItem
{
    Q_OBJECT
public:
    SimChat( SimState *state, SimXmlNode& e );
    ~SimChat() {}

    virtual bool command( const QString& cmd );

private:
    QString _command;
    QString response;
    int responseDelay;
    QString switchTo;
    bool wildcard;
    bool eol;
    QString variable;
    QString value;
    QString variable2;
    QString value2;
    QString peer;
    bool peerConnect;
    bool peerHangup;
    QString newCallVar;
    QString forgetCallId;
    bool listSMS;
    bool deleteSMS;
};


class SimUnsolicited : public SimItem
{
    Q_OBJECT
public:
    SimUnsolicited( SimState *state, SimXmlNode& e );
    ~SimUnsolicited() {}

    virtual void enter();
    virtual void leave();

private:
    QString response;
    int responseDelay;
    QString switchTo;
    bool doOnce;
    bool done;
    QTimer *timer;

private slots:
    void timeout();

};


class SimPhoneBook : public QObject
{
    Q_OBJECT
public:
    SimPhoneBook( int size, QObject *parent );
    ~SimPhoneBook();

    int size() const { return numbers.size(); }
    int used() const;

    QString number( int index ) const;
    QString name( int index ) const;

    void setDetails( int index, const QString& number, const QString& name );

    bool contains( const QString& number ) const { return numbers.contains( number ); }

private:
    QStringList numbers;
    QStringList names;
};

class HardwareManipulatorFactory;
class HardwareManipulator;
class SimRules : public QTcpSocket
{
    Q_OBJECT
public:
    SimRules(int fd, QObject *parent, const QString& filename, HardwareManipulatorFactory *hmf );
    ~SimRules() {}

    // get the variable value for.
    QString variable(const QString &name);

    // Load the peer information from a separate XML file.
    void loadPeers( const QString& filename );

    // Get the current simulator state.
    SimState *current() const { return currentState; }

    // Get the default simulator state.
    SimState *defaultState() const { return defState; }

    // Issue a response to the client.
    void respond( const QString& resp, int delay, bool eol=true );

    // Expand variable references in a string.
    QString expand( const QString& s );

    // Send a command to the connected peer, if any.
    void peerCommand( const QString& cmd );

    // Connect to a phone simulator peer if the number is recognised as a peer.
    // Returns false if not using a peer.
    bool peerConnect( const QString& number );

    // Hangup the connected peer, if any.
    void peerHangup();

    // Get the phone number for this peer.
    QString phoneNumber();

    // Print the phone number for this peer.
    void printPeerNumber();

    // force the next returned reply to be 'error'
    void setReturnError( const QString &error, uint repeat = 0 );

    // Allocate a new call identifier.
    int newCall();

    // Forget a call identifier.
    void forgetCall( int id );

    // Forget all call identifiers (global hangup).
    void forgetAllCalls();

    void setPhoneNumber(const QString &s);

    // Gets the hardware manipulator
    HardwareManipulator * getMachine() const;

    // Get the call manager for this rule object.
    CallManager *callManager() const { return _callManager; }

    // Get or set the SIM toolkit application.
    SimApplication *simApplication() const { return toolkitApp; }
    void setSimApplication( SimApplication *app );

signals:
    void returnQueryVariable( const QString&, const QString & );
    void returnQueryState( const QString& );
    void modemCommand( const QString& );
    void modemResponse( const QString& );

public slots:
    void queryVariable( const QString &name );
    void queryState( );
    // Set a variable to a new value.
    void setVariable( const QString& name, const QString& value );

    // Switch to a new simulator state.
    void switchTo(const QString& name);

    // Process a command.
    void command( const QString& cmd );

    // Send an unsolicited response to the client.
    void unsolicited( const QString& resp );

    // Send a response line.
    void respond( const QString& resp ) { respond( resp, 0 ); }

private slots:
    void tryReadCommand();
    void destruct();
    void delayTimeout();
    void dialCheck( const QString& number, bool& ok );

private:
    SimState *currentState;
    SimState *defState;
    QList<SimState *> states;
    QMap<QString,QString> variables;
    QMap<QString,QString> peers;
    int usedCallIds;
    bool useGsm0710;
    int currentChannel;
    char incomingBuffer[1024];
    int incomingUsed;
    char lineBuffer[1024];
    int lineUsed;
    SimFileSystem *fileSystem;
    SimApplication *defaultToolkitApp;
    SimApplication *toolkitApp;

    // Get a particular state object.
    SimState *state( const QString& name ) const;

    QString return_error_string;
    uint return_error_count;
    QString mPhoneNumber;
    HardwareManipulator *machine;

    void writeGsmFrame( int type, const char *data, uint len );
    void writeChatData( const char *data, uint len );

    void initPhoneBooks();
    void phoneBook( const QString& cmd );
    SimPhoneBook *currentPB() const;
    void loadPhoneBook( SimXmlNode& node );

    QString currentPhoneBook;
    QMap< QString, SimPhoneBook * > phoneBooks;

    CallManager *_callManager;
};


class SimDelayTimer : public QTimer
{
    Q_OBJECT
public:
    SimDelayTimer( const QString& response, int channel )
        : QTimer() { this->response = response; this->channel = channel; }

public:
    QString response;
    int channel;
};


#endif /* PHONESIM_H */
