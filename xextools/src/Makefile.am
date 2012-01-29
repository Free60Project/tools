bin_PROGRAMS = xexread

xexread_SOURCES = \
        swissknife.c    \
        xex_display.c   \
        xexread.c	\
	swissknife.h	\
	xex_display.h
	

xexread_LDADD = \
	@LIBS@ $(top_builddir)/src/lib/libxex.la

xexreadincludedir = -I$(top_srcdir)/src/lib

xexreaddir = $(includedir)/xexread

xexread_HEADERS = \
	swissknife.h \
	xex_display.h

SUBDIRS = lib

AM_CPPFLAGS = \
	-Wall #-Werror

INCLUDES = $(xexreadincludedir)

CLEANFILES = core core.* *~

indent:
	indent -gnu -nut *.c *.h

strip:
	strip xexread
