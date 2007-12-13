HEADERDIR = ../stactiverecord
noinst_PROGRAMS = example
nobase_include_HEADERS = $(HEADERDIR)/stactive_record.h $(HEADERDIR)/config.h $(HEADERDIR)/cud_property_register.h \
        $(HEADERDIR)/exception.h $(HEADERDIR)/query.h $(HEADERDIR)/record.h $(HEADERDIR)/stactive_record.h \
        $(HEADERDIR)/types.h $(HEADERDIR)/utils.h $(HEADERDIR)/storage.h $(HEADERDIR)/where.h $(HEADERDIR)/datetime.h

example_SOURCES = example.cpp
example_LDFLAGS = -lstactiverecord -L$(HEADERDIR)/.libs
