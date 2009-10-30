TARGET = phonesim

FORMS *= controlbase.ui

HEADERS *= control.h attranslator.h gsmspec.h gsmitem.h \
                  phonesim.h server.h hardwaremanipulator.h \
                  qsmsmessagelist.h \
                  qsmsmessage.h \
                  qcbsmessage.h \
                  callmanager.h \
                  simfilesystem.h \
                  simapplication.h \
                  qgsmcodec.h \
                  qatutils.h \
                  qatresultparser.h \
                  qatresult.h \
                  qwsppdu.h \
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
                  qgsmcodec.cpp \
                  qatutils.cpp \
                  qatresultparser.cpp \
                  qatresult.cpp \
                  qwsppdu.cpp \
                  qsimcommand.cpp \
                  qsimenvelope.cpp \
                  qsimterminalresponse.cpp \
                  qsimcontrolevent.cpp

TEMPLATE = app

QT *= core gui xml network

CONFIG += debug

DESTDIR = ..

target.path = $$INSTALL_PREFIX/bin

INSTALLS += target
