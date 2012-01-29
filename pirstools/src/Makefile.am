bin_PROGRAMS = pirsread pirs

pirsread_SOURCES = \
        util.c	\
        pirs_display.c	\
        pirsread.c	\
	util.h	\
	pirs_display.h

pirs_SOURCES = \
	pirs.c

pirs_LDADD = \
	@LIBS@ $(top_builddir)/src/lib/libpirs.la

pirsread_LDADD = \
	@LIBS@ $(top_builddir)/src/lib/libpirs.la

pirsreadincludedir = -I$(top_srcdir)/src/lib
pirsincludedir = -I$(top_srcdir)/src/lib

pirsreaddir = $(includedir)/pirsread
pirsdir = $(includedir)/pirsread

pirsread_HEADERS = \
	util.h \
	pirs_display.h

SUBDIRS = lib

AM_CPPFLAGS = \
	-Wall -Werror

INCLUDES = $(pirsreadincludedir) $(pirsincludedir)

CLEANFILES = core core.* *~

indent:
	indent -gnu -nut *.c *.h

strip:
	strip pirsread
