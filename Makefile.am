
AM_MAKEFLAGS = --no-print-directory

bin_PROGRAMS = src/phonesim

src_phonesim_SOURCES = src/main.cpp \
			src/control.h src/control.cpp \
			src/attranslator.h src/attranslator.cpp \
			src/gsmspec.h src/gsmspec.cpp \
			src/gsmitem.h src/gsmitem.cpp \
			src/phonesim.h src/phonesim.cpp \
			src/server.h src/server.cpp \
			src/hardwaremanipulator.h src/hardwaremanipulator.cpp \
			src/qsmsmessagelist.h src/qsmsmessagelist.cpp \
			src/qsmsmessage_p.h \
			src/qsmsmessage.h src/qsmsmessage.cpp \
			src/qcbsmessage.h src/qcbsmessage.cpp \
			src/callmanager.h src/callmanager.cpp \
			src/simfilesystem.h src/simfilesystem.cpp \
			src/simapplication.h src/simapplication.cpp \
			src/qgsmcodec.h src/qgsmcodec.cpp \
			src/qatutils.h src/qatutils.cpp \
			src/qatresultparser.h src/qatresultparser.cpp \
			src/qatresult.h src/qatresult.cpp \
			src/qwsppdu.h src/qwsppdu.cpp \
			src/qsimcommand.h src/qsimcommand.cpp \
			src/qsimenvelope.h src/qsimenvelope.cpp \
			src/qsimterminalresponse.h src/qsimterminalresponse.cpp \
			src/qsimcontrolevent.h src/qsimcontrolevent.cpp

nodist_src_phonesim_SOURCES = src/ui_controlbase.h \
				src/moc_control.cpp \
				src/moc_phonesim.cpp \
				src/moc_hardwaremanipulator.cpp \
				src/moc_callmanager.cpp \
				src/moc_simfilesystem.cpp \
				src/moc_simapplication.cpp \
				src/moc_qwsppdu.cpp

src_phonesim_LDADD = -lQtXml -lQtGui -lQtNetwork -lQtCore -lpthread

QT_CXXFLAGS = -DQT_XML_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB

AM_CXXFLAGS = -pipe -g -fPIC -Wall -W -D_REENTRANT $(QT_CXXFLAGS)

INCLUDES = -I$(top_srcdir)/src -I$(top_builddir)/src \
			-I$(QTDIR)/mkspecs/linux-g++-64 \
			-I/usr/include/QtCore -I/usr/include/QtNetwork \
			-I/usr/include/QtGui -I/usr/include/QtXml

CLEANFILES = src/control.moc $(nodist_src_phonesim_SOURCES)

EXTRA_DIST = src/controlbase.ui src/moblin.xml

MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 configure depcomp missing install-sh


$(src_phonesim_OBJECTS): src/control.moc src/ui_controlbase.h

src/control.moc: src/control.cpp
	$(AM_V_GEN)$(QTDIR)/bin/moc $(QT_CXXFLAGS) $(INCLUDES) $< -o $@

src/moc_%.cpp: src/%.h
	$(AM_V_GEN)$(QTDIR)/bin/moc $< -o $@

src/ui_%.h: src/%.ui
	$(AM_V_GEN)$(QTDIR)/bin/uic $< -o $@
