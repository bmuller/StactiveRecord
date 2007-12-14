HEADERDIR = ../stactiverecord
noinst_PROGRAMS = dbs many_to_many one_to_many one_to_one querying simple
nobase_include_HEADERS = $(HEADERDIR)/stactive_record.h $(HEADERDIR)/config.h $(HEADERDIR)/cud_property_register.h \
        $(HEADERDIR)/exception.h $(HEADERDIR)/query.h $(HEADERDIR)/record.h $(HEADERDIR)/stactive_record.h \
        $(HEADERDIR)/types.h $(HEADERDIR)/utils.h $(HEADERDIR)/storage.h $(HEADERDIR)/where.h $(HEADERDIR)/datetime.h

dbs_SOURCES = dbs.cpp  
dbs_LDFLAGS = -lstactiverecord -L$(HEADERDIR)/.libs

many_to_many_SOURCES = many_to_many.cpp  
many_to_many_LDFLAGS = -lstactiverecord -L$(HEADERDIR)/.libs

one_to_many_SOURCES = one_to_many.cpp  
one_to_many_LDFLAGS = -lstactiverecord -L$(HEADERDIR)/.libs

one_to_one_SOURCES = one_to_one.cpp  
one_to_one_LDFLAGS = -lstactiverecord -L$(HEADERDIR)/.libs

querying_SOURCES = querying.cpp  
querying_LDFLAGS = -lstactiverecord -L$(HEADERDIR)/.libs

simple_SOURCES = simple.cpp  
simple_LDFLAGS = -lstactiverecord -L$(HEADERDIR)/.libs
