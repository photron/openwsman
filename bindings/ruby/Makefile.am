
rubydir = $(shell ruby -r rbconfig -e "print Config::CONFIG['vendorarchdir']")
rbincdir = $(shell ruby -r rbconfig -e "print Config::CONFIG['archdir']")
rbldadd = $(shell ruby -r rbconfig -e "print Config::CONFIG['LIBRUBYARG']")

INCLUDES =  \
           -I$(top_srcdir) \
	   -I$(top_srcdir)/include \
	   -I$(top_srcdir)/bindings \
	    $(OPENSSL_CFLAGS) \
	   -DSYSCONFDIR=\""$(sysconfdir)/openwsman"\" \
	   -I${rbincdir}

LDADD = \
       ${rbldadd}       

LIBS = \
       $(OPENSSL_LIBS) \
       $(top_builddir)/src/lib/libwsman.la \
       $(top_builddir)/src/lib/libwsman_client.la \
       $(top_builddir)/src/lib/libwsman_curl_client_transport.la


GENERATED = openwsman_wrap.c \
	    rbwsman.rb
SWIG_SOURCES = ../openwsman.i
rbwsman_la_SOURCES = openwsman_wrap.c \
			 ../openwsman.c \
			 $(SWIG_SOURCES)


rbwsman_la_LIBADD = $(LIBS) -lpthread $(CURL_LIBS)

rbwsman_la_LDFLAGS = -module -avoid-version $(NO_UNDEFINED)

ruby_LTLIBRARIES = rbwsman.la

openwsman_wrap.c: $(SWIG_SOURCES)
	$(SWIG) -ruby -o $@ $<

install-data-local:
	@INSTALL@ -m 0644 rbwsman.rb ${rbvendordir}

CLEANFILES= $(GENERATED)
