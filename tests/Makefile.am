HEADERDIR = ../stactiverecord
noinst_PROGRAMS = db_test object_test
nobase_include_HEADERS = $(HEADERDIR)/stactive_record.h $(HEADERDIR)/config.h $(HEADERDIR)/cud_property_register.h \
        $(HEADERDIR)/exception.h $(HEADERDIR)/query.h $(HEADERDIR)/record.h $(HEADERDIR)/stactive_record.h \
        $(HEADERDIR)/types.h $(HEADERDIR)/utils.h $(HEADERDIR)/storage.h $(HEADERDIR)/where.h $(HEADERDIR)/datetime.h

db_test_SOURCES = db_test.cpp
db_test_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs

object_test_SOURCES = object_test.cpp
object_test_LDFLAGS = -lstactiverecord -L./stactiverecord/.libs