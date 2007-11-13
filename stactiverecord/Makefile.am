ACLOCAL_AMFLAGS=-I ${top_srcdir}/acinclude.d/

INCLUDES = ${BDB_CPPFLAGS} ${SQLITE3_CFLAGS} ${MYSQL_CFLAGS}
AM_LDFLAGS = ${BDB_LIBS} ${SQLITE3_LDFLAGS} ${MYSQL_LDFLAGS}

lib_LTLIBRARIES = libstactiverecord.la
#libstactiverecord_la_LIBADD = $(LIB_KPARTS)
#libstactiverecord_la_LDFLAGS = $(all_libraries) -version-info 6:0:2 -no-undefined
libstactiverecord_la_SOURCES = utils.cpp cud_property_register.cpp storage/sar_dbi.cpp record.cpp query.cpp where.cpp

if SQLITE3
libstactiverecord_la_SOURCES += storage/sqlite_storage.cpp 
endif

if MYSQL
libstactiverecord_la_SOURCES += storage/mysql_storage.cpp
endif

#if BDB
#libstactiverecord_la_SOURCES += storage/bdb_storage.cpp
#endif

