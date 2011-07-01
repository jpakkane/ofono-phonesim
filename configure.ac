AC_PREREQ(2.60)
AC_INIT(phonesim, 1.15)

AM_INIT_AUTOMAKE([foreign subdir-objects])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT(/usr/local)

PKG_PROG_PKG_CONFIG

AC_PROG_CXX
AC_PROG_INSTALL

AC_ARG_ENABLE(optimization, AC_HELP_STRING([--disable-optimization],
			[disable code optimization through compiler]), [
	if (test "${enableval}" = "no"); then
		CXXFLAGS="$CXXFLAGS -O0"
	fi
])

PKG_CHECK_MODULES(QT, QtCore QtGui QtXml QtNetwork QtScript QtDBus, dummy=yes,
						AC_MSG_ERROR(Qt is required))
AC_SUBST(QT_CFLAGS)
AC_SUBST(QT_LIBS)

AC_MSG_CHECKING(for moc)
MOC="`$PKG_CONFIG --variable=moc_location QtCore`"
AC_SUBST(MOC)
AC_MSG_RESULT($MOC)

AC_MSG_CHECKING(for uic)
UIC="`$PKG_CONFIG --variable=uic_location QtCore`"
AC_SUBST(UIC)
AC_MSG_RESULT($UIC)

AC_OUTPUT(Makefile)
