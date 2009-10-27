TARGET = phonesim

FORMS *= controlbase.ui

DEFINES += PHONESIM

HEADERS *= control.h attranslator.h gsmspec.h gsmitem.h \
                  phonesim.h server.h hardwaremanipulator.h \
                  qsmsmessagelist.h \
                  qsmsmessage.h \
                  qcbsmessage.h \
                  callmanager.h \
                  simfilesystem.h \
                  simapplication.h \
                  serial/qgsmcodec.h \
                  serial/qatutils.h \
                  serial/qatresultparser.h \
                  serial/qatresult.h \
                  wap/qwsppdu.h \
                  qsimcommand.h \
                  qsimenvelope.h \
                  qsimterminalresponse.h \
                  qsimcontrolevent.h \

SOURCES *= main.cpp control.cpp attranslator.cpp gsmspec.cpp gsmitem.cpp \
                  phonesim.cpp server.cpp hardwaremanipulator.cpp \
                  qsmsmessagelist.cpp \
                  qsmsmessage.cpp \
                  qcbsmessage.cpp \
                  callmanager.cpp \
                  simfilesystem.cpp \
                  simapplication.cpp \
                  serial/qgsmcodec.cpp \
                  serial/qatutils.cpp \
                  serial/qatresultparser.cpp \
                  serial/qatresult.cpp \
                  wap/qwsppdu.cpp \
                  qsimcommand.cpp \
                  qsimenvelope.cpp \
                  qsimterminalresponse.cpp \
                  qsimcontrolevent.cpp

TEMPLATE = app

QT *= core gui xml network

INCLUDEPATH += serial wap

CONFIG += debug

DESTDIR = ..

target.path = $$INSTALL_PREFIX/bin

INSTALLS += target
