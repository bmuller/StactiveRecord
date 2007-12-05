SUBDIRS = stactiverecord

noinst_PROGRAMS = example db_test object_test
nobase_include_HEADERS = stactiverecord/stactive_record.h stactiverecord/config.h stactiverecord/cud_property_register.h \
	stactiverecord/exception.h stactiverecord/query.h stactiverecord/record.h stactiverecord/stactive_record.h \
	stactiverecord/types.h stactiverecord/utils.h stactiverecord/storage.h stactiverecord/where.h stactiverecord/datetime.h

example_SOURCES = tests/example.cpp
example_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs

db_test_SOURCES = tests/db_test.cpp
db_test_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs

object_test_SOURCES = tests/object_test.cpp
object_test_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs