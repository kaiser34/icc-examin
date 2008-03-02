include config

ifdef BUILD64
CC =  cc-64
CXX = c++-64
else
CC = cc
CXX = c++
endif
COLLECT = ar cru
RANLIB = ranlib
MAKEDEPEND	= makedepend -Y
LNK = ln -s
RM = rm -vf
ifdef LINUX
COPY = cp -vdpa
else
  ifdef APPLE
  COPY = cp -v
  else
  COPY = cp
  endif
endif
INSTALL = install -v

exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
datadir		= ${prefix}/share
includedir	= ${prefix}/include
libdir		= ${exec_prefix}/lib
mandir		= ${prefix}/man

DEBUG = -DDEBUG
DL = --ldflags # --ldstaticflags

X_CPPFILES = icc_helfer_x.cpp
OSX_CPPFILES = icc_helfer_osx.cpp
FLTK_CPPFILES = icc_helfer_fltk.cpp
MSGFMT = msgfmt -c --statistics
RPMARCH = `rpmbuild --showrc | awk '/^build arch/ {print $$4}'`

ifdef APPLE
  OPTS=-Wall -g $(DEBUG) -DPIC
  LIBLINK_FLAGS = -dynamiclib
  I18N_LIB = -lintl #-liconv
  OSX_CPP = $(OSX_CPPFILES)
  INCL=-I$(includedir) -I/usr/X11R6/include -I./ -I/usr/include/gcc/darwin/default/c++
  REZ = \
      test -d ICC\ Examin.app/Contents/MacOS/ || mkdir ICC\ Examin.app/ ICC\ Examin.app/Contents/ ICC\ Examin.app/Contents/MacOS/ ICC\ Examin.app/Contents/Resources/; \
      $(COPY) Info.plist ICC\ Examin.app/Contents/Info.plist; \
      $(COPY) $(TARGET) ICC\ Examin.app/Contents/MacOS/ICC\ Examin; \
      $(COPY) $(TARGET) ICC\ Examin/build/ICC\ Examin.app/Contents/MacOS/ICC\ Examin; \
      fltk-config --post $(TARGET);
  MAKEDEPEND = /usr/X11R6/bin/makedepend -Y
  DBG_LIBS = #-lMallocDebug
  MSGMERGE = msgmerge
  XGETTEXT_OPTIONS = \
	--keyword=gettext \
	--keyword=_ \
	--keyword=N_
else
  SO = .so
    MSGMERGE = msgmerge --update
    XGETTEXT_OPTIONS = \
	--keyword=gettext --flag=gettext:1:pass-c-format \
	--keyword=_ --flag=_:1:pass-c-format \
	--keyword=N_ --flag=N_:1:pass-c-format \
	--copyright-holder='Kai-Uwe Behrmann' \
	--msgid-bugs-address='ku.b@gmx.de' \
	--from-code=utf-8
  ifdef LINUX
    OPTS = -Wall  -Os -g $(DEBUG) #-fomit-frame-pointer -g
    INCL=-I$(includedir) -I/usr/X11R6/include -I./
    LIBLINK_FLAGS = -shared -ldl -L.
    LIBSONAME = lib$(TARGET)$(SO).$(VERSION_A)
    LINK_NAME = -Wl,-soname -Wl,$(LIBSONAME)
    LINK_LIB_PATH = -Wl,--rpath -Wl,$(libdir)
    LINK_SRC_PATH = -Wl,--rpath -Wl,$(srcdir)
  else
    OPTS=-Wall -O2 -g $(DEBUG) -L.
    RM = rm -f
    LIBLINK_FLAGS = -shared -ldl
    I18N_LIB = $(ICONV) -lintl
  endif
endif

LIBSONAMEFULL = lib$(TARGET)$(SO).$(VERSION_L)
LIBSONAME = lib$(TARGET)$(SO).$(VERSION_A)
LIBSO = lib$(TARGET)$(SO)
LIBNAME = lib$(TARGET).a

ifdef FLTK
  TOOLKIT_FILES = $(FLTK_CPPFILES)
endif

ifdef X11
  X_CPP = $(X_CPPFILES)
  X11_LIBS=-L/usr/X11R6/lib -lX11 -lXxf86vm -lXext
endif

INCL_DEP = $(INCL) $(X_H) $(OSX_H) $(OYRANOS_H) $(SOURCES)
ALL_INCL = $(INCL) \
			$(FLU_H) $(FLTK_H) $(X_H) $(OSX_H) $(OYRANOS_H) $(LCMS_H) $(FTGL_H)

CXXFLAGS=$(OPTS) $(ALL_INCL)
CFLAGS = $(OPTS) $(ALL_INCL)

ifdef FLU
  FLU_FLTK_LIBS = $(FLU_LIBS) $(FLTK_LIBS)
else
  FLU_FLTK_LIBS = $(FLTK_LIBS)
endif

LDLIBS = -L$(libdir) -L./ $(FLU_FLTK_LIBS) -licc_examin \
	$(X11_LIBS) -llcms $(OYRANOS_LIBS) $(LCMS_LIBS) $(FTGL_LIBS) $(I18N_LIB) \
	$(DBG_LIBS)

CPP_HEADERS = \
	agviewer.h \
	icc_betrachter.h \
	cccie64.h \
	ciexyz64_1.h \
	icc_cgats_filter.h \
	icc_draw.h \
	icc_draw_fltk.h \
	icc_fenster.h \
	icc_formeln.h \
	icc_examin.h \
	icc_gamut.h \
	icc_gl.h \
	icc_helfer.h \
	icc_helfer_fltk.h \
	icc_helfer_ui.h \
	icc_helfer_x.h \
	icc_info.h \
	icc_icc.h \
	icc_kette.h \
	icc_measurement.h \
	icc_modell_beobachter.h \
	fl_oyranos.h \
	icc_oyranos.h \
	icc_oyranos_extern.h \
	icc_profile.h \
	icc_profile_header.h \
	icc_profile_tags.h \
	icc_speicher.h \
	icc_ueber.h \
	icc_utils.h \
	icc_version.h \
	icc_vrml.h \
	icc_vrml_parser.h \
	icc_waehler.h \
	threads.h
#	Fl_Slot.H

COMMON_CPPFILES = \
	agviewer.cpp \
	icc_cgats_filter.cpp \
	icc_draw.cpp \
	icc_examin.cpp \
	icc_examin_farbraum.cpp \
	icc_examin_io.cpp \
	icc_examin_tagwahl.cpp \
	icc_fenster.cpp \
	icc_formeln.cpp \
	icc_gamut.cpp \
	icc_gl.cpp \
	icc_helfer.cpp \
    icc_helfer_i18n.cpp \
	icc_info.cpp \
	icc_kette.cpp \
	icc_main.cpp \
	icc_measurement.cpp \
	icc_modell_beobachter.cpp \
	icc_oyranos.cpp \
	icc_oyranos_extern.cpp \
	icc_profile.cpp \
	icc_profile_header.cpp \
	icc_profile_tags.cpp \
	icc_schnell.cpp \
	icc_speicher.cpp \
	icc_ueber.cpp \
	icc_utils.cpp \
	icc_vrml.cpp \
	icc_vrml_parser.cpp \
	threads.cpp
FREEGLUT_HFILES = \
	freeglut_internal.h
FREEGLUT_CFILES = \
	freeglut_geometry.c

LINGUAS = \
	de #en_GB

COMMON_CFILES = \
	$(FREEGLUT_CFILES)
CFILES = \
	$(COMMON_CFILES)
CPPFILES = \
	$(COMMON_CPPFILES) \
	$(TOOLKIT_FILES) \
	$(X_CPP) \
	$(OSX_CPP)
CXXFILES = \
	icc_betrachter.cxx \
	fl_oyranos.cxx
TEST = \
	dE2000_test.cpp \
	ciede2000testdata.h

ALL_SOURCEFILES = \
	$(COMMON_CFILES) \
	$(COMMON_CPPFILES) \
	$(OSX_CPPFILES) \
	$(X_CPPFILES) \
	$(FLTK_CPPFILES) \
	$(CXXFILES) \
	$(TEST)

ALL_HEADERFILES = \
	$(CPP_HEADERS) \
	$(FREEGLUT_HFILES)

DOKU = \
	TODO \
	README \
	ChangeLog \
	COPYING \
	BUGS \
	AUTHORS \
	icc_examin.spec \
	icc_examin.desktop
FLUID = \
	icc_betrachter.fl \
	fl_oyranos.fl
FONT = FreeSans.ttf

SOURCES = $(ALL_SOURCEFILES) $(ALL_HEADERFILES)
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o)
CLIB_OBJECTS =  $(CFILES:.c=.o)

POT_FILE = po/$(TARGET).pot

APPL_FILES = \
	ICC\ Examin.app/ \
	Info.plist

ALL_FILES =	$(SOURCES) \
	makefile \
	configure \
	$(DOKU) \
	$(FONT) \
	$(APPL_FILES) \
	$(FLUID)

timedir = .
mtime   = `find $(timedir) -prune -printf %Ty%Tm%Td.%TT | sed s/://g`

.SILENT:

all:	dynamic
	
base:	config mkdepend 
	
release:	icc_alles.o
	echo Linking $@...
	$(CXX) $(OPTS) -o $(TARGET) \
	icc_alles.o \
	$(LDLIBS)
	$(REZ)
	$(RM) icc_alles.o

$(TARGET):	base $(OBJECTS) $(LIBNAME) pot #$(LIBSONAMEFULL)
	
dynamic:	$(TARGET)
	echo Linking $@...
	$(CXX) $(OPTS) -o $(TARGET) \
	$(OBJECTS) \
	$(LDLIBS) $(LINK_LIB_PATH) $(LINK_SRC_PATH)
	$(REZ)

$(LIBSONAMEFULL):	$(CLIB_OBJECTS)
	echo Linking $@ ...
	$(CC) $(OPTS) $(LIBLINK_FLAGS) $(LINK_NAME) -o $(LIBSONAMEFULL) \
	$(CLIB_OBJECTS) 
	$(REZ)
	$(RM)  $(LIBSONAME)
	$(LNK) $(LIBSONAMEFULL) $(LIBSONAME)
	$(RM)  $(LIBSO)
	$(LNK) $(LIBSONAMEFULL) $(LIBSO)

$(LIBNAME):	$(CLIB_OBJECTS)
	echo Linking $@ ...
	$(COLLECT) $(LIBNAME) $(CLIB_OBJECTS)
	$(RANLIB) $(LIBNAME)

static:	$(TARGET)
	echo Linking $@ ...
	$(CXX) -Wall -O3 -o $(TARGET) $(OBJECTS) \
	-L./  -licc_examin \
	`flu-config --ldstaticflags` \
	`fltk-config --use-gl --use-images --ldstaticflags` \
	-L/usr/X11R6/lib -lX11 -lXxf86vm -lXext -L/opt/local/lib \
	`oyranos-config --ld_x_staticflags` -L/$(prefix)/lib \
	-L$(prefix)/lib -lGLU -lGL -lfreetype \
	`pkg-config --libs ftgl`  -lsupc++ \
	$(I18N_LIB) \
	$(DBG_LIBS) \
	`test -f /opt/kai-uwe/lib/liblcms.a && echo /opt/kai-uwe/lib/liblcms.a || pkg-config --libs lcms` #/usr/lib/libkdb.a # Hack for static lcms
	strip $(TARGET)
	$(REZ)

static_static:	$(OBJECTS)
	$(CXX) $(OPTS) -o $(TARGET) \
	$(OBJECTS) \
	$(LDLIBS) -static -ljpeg -lpng -lX11 -lpthread -lz -ldl \
	-lfreetype -lfontconfig -lXrender -lGLU -lXext -lexpat \
	-L/opt/local/lib liblcms.a
	$(REZ)

test:	icc_formeln.o icc_utils.o
	$(CXX) $(OPTS) $(INCL) -o dE2000_test.o -c dE2000_test.cpp
	$(CXX) $(OPTS) -o dE2000_test dE2000_test.o icc_formeln.o icc_utils.o \
	-L$(libdir) -llcms
	$(REZ)

pot:	$(POT_FILE)
	echo Preparing Linguas ...
	for ling in $(LINGUAS); do \
	  echo "update po/$${ling}.gmo ..."; \
	  test -f po/$${ling}.po \
        && ($(MSGFMT) -o po/$${ling}.gmo po/$${ling}.po; \
            if [ ! -d po/$${ling} ]; then \
              mkdir po/$${ling}; \
            fi; \
            if [ ! -d po/$${ling}/LC_MESSAGES ]; then \
              mkdir po/$${ling}/LC_MESSAGES; \
            fi; \
            test -f po/$${ling}/LC_MESSAGES/$(TARGET).mo && $(RM) po/$${ling}/LC_MESSAGES/$(TARGET).mo; \
            ln -s ../../$${ling}.gmo po/$${ling}/LC_MESSAGES/$(TARGET).mo;) \
        || (echo $${ling}.po is not yet ready ... skipping) \
	done;

potfile:
	xgettext --force-po $(XGETTEXT_OPTIONS) \
	-d $(TARGET) \
	-o $(POT_FILE) \
	$(SOURCES)
	for ling in $(LINGUAS); do \
            $(MSGMERGE) po/$${ling}.po $(POT_FILE); \
	done;

$(POT_FILE):	potfile

install:	static
	echo Installing ...
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET) $(DESTDIR)$(bindir)
	mkdir -p $(DESTDIR)$(datadir)/fonts/
	$(INSTALL) -m 644 $(FONT) $(DESTDIR)$(datadir)/fonts/
	mkdir -p $(DESTDIR)$(datadir)/applications/
	$(INSTALL) -m 644 icc_examin.desktop $(DESTDIR)$(datadir)/applications/icc_examin.desktop
	echo  Linguas ...
	for ling in $(LINGUAS); do \
	  echo "update po/$${ling}.gmo ..."; \
      mkdir -p $(DESTDIR)$(datadir)/locale/$${ling}/LC_MESSAGES; \
      test -f po/$${ling}.gmo \
		&& (mkdir -p $(DESTDIR)$(datadir)/locale/$${ling}/LC_MESSAGES; \
            $(INSTALL) -m 644 po/$${ling}.gmo $(DESTDIR)$(datadir)/locale/$${ling}/LC_MESSAGES/$(TARGET).mo ) \
		|| (echo $${ling}.gmo is not yet ready ... skipping); \
	done;
	echo ... Installation finished

uninstall:
	echo Uninstalling ...
	$(RM) $(DESTDIR)$(bindir)/$(TARGET)
	$(RM) $(DESTDIR)$(datadir)/fonts/$(FONT)
	for ling in $(LINGUAS); do \
	  $(RM) $(DESTDIR)$(datadir)/locale/$${ling}/LC_MESSAGES/$(TARGET).mo; \
	done;

clean:
	echo "mache sauber"
	$(RM) mkdepend config config.h
	$(RM) $(OBJECTS) $(CLIB_OBJECTS) $(TARGET) \
	$(LIBNAME) $(LIBSO) $(LIBSONAME) $(LIBSONAMEFULL)
	$(RM) ICC\ Examin.app/Contents/MacOS/ICC\ Examin
	for ling in $(LINGUAS); do \
	  test -f po/$${ling}.gmo \
        && $(RM) po/$${ling}.gmo; \
	done;

config:
	configure

depend:
	echo "schaue nach Abhaengikeiten ..."
	echo "MAKEDEPEND_ISUP = 1" > mkdepend
	$(MAKEDEPEND) -f mkdepend \
	-s "#nicht editieren/dont edit - automatisch generiert" \
	-I. $(INCL_DEP)


# The extension to use for executables...
EXEEXT		= 

# Build commands and filename extensions...
.SUFFIXES:	.0 .1 .3 .c .cxx .h .fl .man .o .z $(EXEEXT)

.o$(EXEEXT):
	echo Linking $@ ...
	$(CXX) $(CXXFLAGS) $< $(LINKFLTK) $(LDLIBS) -o $@
	$(POSTBUILD) $@ ../FL/mac.r

.c.o:
	echo Compiling $< ...
	$(CC) $(CFLAGS) -c $<

.cxx.o:
	echo Compiling $< ...
	$(CXX) $(CXXFLAGS) -c $<

.cpp.o:
	echo Compiling $< ...
	$(CXX) $(CXXFLAGS) -c $<

.po:
	echo Generating $@ ...
	msgfmt $<

tgz:
	mkdir Entwickeln
	$(COPY) -R \
	$(ALL_FILES) \
	Entwickeln
	mkdir Entwickeln/po
	$(COPY) $(POT_FILE) Entwickeln/po
	for ling in $(LINGUAS); do \
	  test -f po/$${ling}.po \
        && $(COPY) po/$${ling}.po Entwickeln/po/; \
	done;
	tar cf - Entwickeln/ \
	| gzip > $(TARGET)_$(mtime).tgz
	test -d ../Archiv && mv -v $(TARGET)_*.tgz ../Archiv
	test -d Entwickeln && \
	test `pwd` != `(cd Entwickeln; pwd)` && \
	$(RM) -R Entwickeln

targz:
	mkdir $(TARGET)_$(VERSION)
	$(COPY) -R \
	$(ALL_FILES) \
	$(TARGET)_$(VERSION)
	mkdir $(TARGET)_$(VERSION)/po
	$(COPY) $(POT_FILE) $(TARGET)_$(VERSION)/po
	for ling in $(LINGUAS); do \
	  test -f po/$${ling}.po \
        && $(COPY) po/$${ling}.po $(TARGET)_$(VERSION)/po/; \
	done;
	tar cf - $(TARGET)_$(VERSION)/ \
	| gzip > $(TARGET)_$(mtime).tgz
	test -d ../Archiv && mv -v $(TARGET)_*.tgz ../Archiv
	test -d $(TARGET)_$(VERSION) && \
	test `pwd` != `(cd $(TARGET)_$(VERSION); pwd)` && \
	$(RM) -R $(TARGET)_$(VERSION) 

dist: targz
	$(COPY) ../Archiv/$(TARGET)_$(mtime).tgz $(TARGET)_$(VERSION).tar.gz

rpm:	dist
	mkdir -p rpmdir/BUILD \
	rpmdir/SPECS \
	rpmdir/SOURCES \
	rpmdir/SRPMS \
	rpmdir/RPMS/$(RPMARCH)
	cp -f $(TARGET)_$(VERSION).tar.gz rpmdir/SOURCES
	rpmbuild --clean -ba $(srcdir)/$(TARGET).spec --define "_topdir $$PWD/rpmdir"
	@echo "============================================================"
	@echo "Finished - the packages are in rpmdir/RPMS and rpmdir/SRPMS!"


# Abhaengigkeiten
include mkdepend

ifndef MAKEDEPEND_ISUP
mkdepend: depend
endif

