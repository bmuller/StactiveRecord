AC_INIT([StactiveRecord], [0.1], [bmuller@butterfat.net])
AC_DEFINE([PACKAGE_URL],["http://findingscience.com/StactiveRecord"],[project url])
AM_CONFIG_HEADER(stactiverecord/config.h)
AM_INIT_AUTOMAKE()

AC_CONFIG_MACRO_DIR([acinclude.d])

AC_PROG_CXX
AC_PROG_CXXCPP
AC_LANG_CPLUSPLUS
AC_CANONICAL_HOST
AC_PROG_INSTALL
AM_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_HEADER_STDC

# provide flag --enable-debug
AC_ARG_ENABLE(debug, AC_HELP_STRING([[--enable-debug]], [enable debugging output to stdout]),
[if test "$enableval" = "yes"; then
    DEBUG="yes"
    CPPFLAGS="$CPPFLAGS -DDEBUG"
fi]
)

# use sqlite
AX_LIB_SQLITE3([3.3.0])
if test "$SQLITE3_VERSION" != ""; then
   CPPFLAGS="$CPPFLAGS -DSQLITE"
fi

AX_LIB_MYSQL()
AX_LIB_POSTGRESQL()

AM_CONDITIONAL([SQLITE3], [test "$SQLITE3_VERSION" != ""])
AM_CONDITIONAL([MYSQL], [test "$MYSQL_VERSION" != ""])
AM_CONDITIONAL([POSTGRESQL], [test "$POSTGRESQL_VERSION" != ""])

AC_CONFIG_FILES([
 Makefile
 libstactiverecord.pc
 stactiverecord/Makefile
 examples/Makefile
 tests/Makefile
])
AC_OUTPUT

echo " ***"

if test "$SQLITE3_VERSION" != ""; then
echo " *** + compiling WITH support for SQLite $SQLITE3_VERSION"
else
echo " *** - compiling WITHOUT support for SQLite"
fi

if test "$MYSQL_VERSION" != ""; then
echo " *** + compiling WITH support for MySQL $MYSQL_VERSION"
else
echo " *** - compiling WITHOUT support for MySQL"
fi

if test "$POSTGRESQL_VERSION" != ""; then
echo " *** + compiling WITH support for PostgreSQL $POSTGRESQL_VERSION"
else
echo " *** - compiling WITHOUT support for PostgreSQL"
fi

if test "$DEBUG" != ""; then
echo " *** + compiling WITH debugging output"
fi

echo " ***"
echo " *** You are now ready to build StactiveRecord"
echo " *** Enter the following commands:"
echo " ***"
echo " *** $> make"
echo " *** $> su root"
echo " *** $> make install"
echo " ***"
echo " *** Report bugs at http://github.com/bmuller/StactiveRecord/issues"
echo " *** Thanks for using free (as in speech, not beer) software."
echo " ***"
echo
