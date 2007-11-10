AC_INIT([StactiveRecord], [0.0], [bmuller@butterfat.net])
AC_DEFINE([PACKAGE_URL],["http://www.butterfat.net/wiki/Projects/StactiveRecord"],[project url])
AM_CONFIG_HEADER(src/config.h)
AM_INIT_AUTOMAKE()

AC_PROG_CXX
AC_PROG_CXXCPP
AC_LANG_CPLUSPLUS
AC_CANONICAL_HOST
AC_PROG_INSTALL
AM_PROG_LIBTOOL

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

AX_PATH_BDB([3], [], [])	 
AC_SUBST([BDB_LIBS])
AC_SUBST([BDB_CPPFLAGS])

AM_CONDITIONAL([SQLITE], [test "$SQLITE3_VERSION" != ""])
AM_CONDITIONAL([BDB], [test "$BDB_LIBS" != ""])
AM_CONDITIONAL([MYSQL], [test "$MYSQL_VERSION" != ""])
AM_CONDITIONAL([POSTGRESQL], [test "$POSTGRESQL_VERSION" != ""])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT


echo " ***"
echo " *** You are now ready to build StactiveRecord"
echo " *** Enter the following commands:"
echo " ***"
echo " *** $> make"
echo " *** $> su root"
echo " *** $> make install"
echo " ***"
echo " *** Report bugs at https://www.butterfat.net/tracker/butterfat/index"
echo " *** Thanks for using free (as in speech, not beer) software."
echo " ***"
echo
