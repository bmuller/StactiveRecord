SUBDIRS=src

noinst_PROGRAMS = test db_test

test_SOURCES = test.cpp
test_LDFLAGS = -lstactiverecord -L./src/.libs
#test_DEPENDENCIES = libstactiverecord.la

db_test_SOURCES = tests/db_test.cpp
db_test_LDFLAGS = -lstactiverecord -L./src/.libs
#db_test_DEPENDENCIES = src/libstactiverecord.la