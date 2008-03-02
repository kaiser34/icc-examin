include config

CC=cc
CXX = c++
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


prefix		= /opt/local
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

ifdef APPLE
  OPTS=-Wall -g -DPIC $(DEBUG)
  LINK_FLAGS = -dynamiclib
  OSX_CPP = $(OSX_CPPFILES)
  INCL=-I$(includedir) -I/usr/X11R6/include -I./ -I/usr/include/gcc/darwin/default/c++
else
  SO = .so
  ifdef LINUX
    OPTS = -Wall  -Os -g $(DEBUG) #-fomit-frame-pointer -g
    INCL=-I$(includedir) -I/usr/X11R6/include -I./
    LINK_FLAGS = -shared -ldl -L.
    LIBSONAME = lib$(TARGET)$(SO).$(VERSION_A)
    LINK_NAME = -Wl,-soname -Wl,$(LIBSONAME)
    LINK_LIB_PATH = -Wl,--rpath -Wl,$(libdir)
    LINK_SRC_PATH = -Wl,--rpath -Wl,$(srcdir)
  else
    OPTS=-Wall -O2 -g -fpic -L.
    RM = rm -f
    LINK_FLAGS = -shared -ldl $(ICONV)
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
			$(FLU_H) $(FLTK_H) $(X_H) $(OSX_H) $(OYRANOS_H) $(LCMS_H)

CXXFLAGS=$(OPTS) $(ALL_INCL)
CFLAGS = $(OPTS)

LDLIBS = -L$(libdir) -L./ $(FLTK_LIBS) -licc_examin \
	$(X11_LIBS) -llcms $(OYRANOS_LIBS) $(FLU_LIBS) $(LCMS_LIBS)

CPP_HEADERS = \
	agviewer.h \
	Fl_Slot.H \
	icc_betrachter.h \
	cccie64.h \
	ciexyz64_1.h \
	icc_cgats_filter.h \
	icc_draw.h \
	icc_draw_fltk.h \
	icc_fenster.h \
	icc_formeln.h \
	icc_examin.h \
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
	icc_waehler.h
COMMON_CPPFILES = \
	agviewer.cpp \
	icc_cgats_filter.cpp \
	icc_draw.cpp \
	icc_examin.cpp \
	icc_examin_histogram.cpp \
	icc_examin_io.cpp \
	icc_examin_tagwahl.cpp \
	icc_fenster.cpp \
	icc_formeln.cpp \
	icc_gl.cpp \
	icc_helfer.cpp \
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
	icc_ueber.cpp \
	icc_utils.cpp \
	icc_vrml.cpp \
	icc_vrml_parser.cpp
FREEGLUT_HFILES = \
	freeglut_internal.h
FREEGLUT_CFILES = \
	freeglut_stroke_mono_roman.c \
	freeglut_stroke_roman.c \
	freeglut_glutfont_definitions.c \
	freeglut_font.c \
	freeglut_font_data.c \
	freeglut_geometry.c
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
	AUTHORS
FLUID = \
	icc_betrachter.fl \
	fl_oyranos.fl

SOURCES = $(ALL_SOURCEFILES) $(ALL_HEADERFILES)
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o)
CLIB_OBJECTS =  $(CFILES:.c=.o)
TARGET  = icc_examin

ifdef APPLE
REZ     = /Developer/Tools/Rez -t APPL -o $(TARGET) /opt/local/include/FL/mac.r
endif

timedir = .
mtime   = `find $(timedir) -prune -printf %Ty%Tm%Td.%TT | sed s/://g`

#.SILENT:

all:	mkdepend $(TARGET)

release:	icc_alles.o
	echo Linking $@...
	$(CXX) $(OPTS) -o $(TARGET) \
	icc_alles.o \
	$(LDLIBS)
	$(REZ)
	$(RM) icc_alles.o

$(TARGET):	$(OBJECTS) $(LIBNAME) #$(LIBSONAMEFULL)
	echo Linking $@...
	$(CXX) $(OPTS) -o $(TARGET) \
	$(OBJECTS) \
	$(LDLIBS) $(LINK_LIB_PATH) $(LINK_SRC_PATH)
	$(REZ)

$(LIBSONAMEFULL):	$(CLIB_OBJECTS)
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME) -o $(LIBSONAMEFULL) \
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

static:		$(OBJECTS)
	echo Linking $@...
	$(CXX) $(OPTS) -o $(TARGET) \
	$(OBJECTS) \
	$(LDLIBS) -static -ljpeg -lpng -lX11 -lpthread -lz -ldl \
	-lfreetype -lfontconfig -lXrender -lGLU -lXext -lexpat
	$(REZ)

test:	icc_formeln.o icc_utils.o
	$(CXX) $(OPTS) $(INCL) -o dE2000_test.o -c dE2000_test.cpp
	$(CXX) $(OPTS) -o dE2000_test dE2000_test.o icc_formeln.o icc_utils.o \
	-L$(libdir) -llcms
	$(REZ)

install:	$(TARGET)
	$(COPY) $(TARGET) $(bindir)

uninstall:
	$(RM) $(bindir)/$(TARGET)

clean:
	echo "mache sauber"
	$(RM) mkdepend config config.h
	$(RM) $(OBJECTS) $(CLIB_OBJECTS) $(TARGET) \
	$(LIBSO) $(LIBSONAME) $(LIBSONAMEFULL)

config:
	configure.sh

depend:
	echo "schaue nach Abhängikeiten ..."
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

tgz:
	mkdir Entwickeln
	$(COPY) \
	$(SOURCES) \
	makefile \
	configure.sh \
	$(DOKU) \
	$(FLUID) \
	Entwickeln
	tar cf - Entwickeln/ \
	| gzip > $(TARGET)_$(mtime).tgz
	test -d ../Archiv && mv -v $(TARGET)_*.tgz ../Archiv
	test -d Entwickeln && \
	test `pwd` != `(cd Entwickeln; pwd)` && \
	rm -R Entwickeln

targz:
	mkdir icc_examin_$(VERSION)
	$(COPY) \
	$(SOURCES) \
	makefile \
	configure.sh \
	$(DOKU) \
	$(FLUID) \
	icc_examin_$(VERSION)
	tar cf - icc_examin_$(VERSION)/ \
	| gzip > $(TARGET)_$(mtime).tgz
	test -d ../Archiv && mv -v $(TARGET)_*.tgz ../Archiv
	test -d icc_examin_$(VERSION) && \
	test `pwd` != `(cd icc_examin_$(VERSION); pwd)` && \
	rm -R icc_examin_$(VERSION) 

# Abhängigkeiten
include mkdepend

ifndef MAKEDEPEND_ISUP
mkdepend: depend
endif

