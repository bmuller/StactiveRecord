SUBDIRS = stactiverecord tests examples
HEADERDIR = stactiverecord
TESTDIR = tests
ACLOCAL_AMFLAGS = -I acinclude.d

pkgconfigdir=${libdir}/pkgconfig
pkgconfig_DATA=libstactiverecord.pc

nobase_include_HEADERS = $(HEADERDIR)/stactive_record.h $(HEADERDIR)/config.h $(HEADERDIR)/cud_property_register.h \
        $(HEADERDIR)/exception.h $(HEADERDIR)/query.h $(HEADERDIR)/record.h $(HEADERDIR)/stactive_record.h \
        $(HEADERDIR)/types.h $(HEADERDIR)/utils.h $(HEADERDIR)/storage.h $(HEADERDIR)/where.h $(HEADERDIR)/datetime.h

test: all
	cd tests
	(test $(DBCONFIG) && (($(TESTDIR)/db_test $(DBCONFIG) && $(TESTDIR)/object_test $(DBCONFIG) && echo "Tests completed successfully") \
	|| echo "Tests failed")) || (echo You should set the DBCONFIG environment variable to "scheme://[user[:password]@host[:port]/]database". \
	For instance: && echo     make test DBCONFIG=sqlite://:memory:)
