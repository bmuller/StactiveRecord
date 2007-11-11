SUBDIRS = stactiverecord

noinst_PROGRAMS = example db_test query_test
nobase_include_HEADERS = stactiverecord/stactive_record.h stactiverecord/config.h stactiverecord/cud_property_register.h \
	stactiverecord/exception.h stactiverecord/query.h stactiverecord/record.h stactiverecord/stactive_record.h \
	stactiverecord/types.h stactiverecord/utils.h stactiverecord/storage.h

example_SOURCES = tests/example.cpp
example_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs

db_test_SOURCES = tests/db_test.cpp
db_test_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs

query_test_SOURCES = tests/query_test.cpp
query_test_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs