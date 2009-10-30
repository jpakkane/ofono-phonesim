AC_PREREQ(2.60)
AC_INIT(phonesim, 0.0)

AM_INIT_AUTOMAKE([foreign subdir-objects])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT(/usr/local)

AC_PROG_CXX
AC_PROG_INSTALL

AC_MSG_CHECKING(Qt directory)
QTDIR=""
AC_ARG_WITH([qtdir],
	AC_HELP_STRING([--with-qtdir=PATH], [path to the Qt directory]),
		[QTDIRLIST="$withval"],
		[QTDIRLIST="/usr/lib/qt4 /usr/lib64/qt4 /usr/lib32/qt4"])
for x in $QTDIRLIST; do
	if test -d $x ; then
		QTDIR="$x"
	fi
done
if test -z $QTDIR ; then
	AC_MSG_ERROR(could not locate Qt directory)
fi
AC_MSG_RESULT($QTDIR)
AC_SUBST(QTDIR)

AC_OUTPUT(Makefile)
