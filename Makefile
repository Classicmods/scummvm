# $Header$

#######################################################################
# Default compilation parameters. Normally don't edit these           #
#######################################################################

srcdir      ?= .

DEFINES     := -DHAVE_CONFIG_H
LDFLAGS     :=
INCLUDES    := -I. -I$(srcdir) -I$(srcdir)/common
LIBS	    :=
OBJS	    :=

MODULES     :=
MODULE_DIRS :=

# Load the make rules generated by configure
include config.mak

# Uncomment this for stricter compile time code verification
# CXXFLAGS+= -Werror

CXXFLAGS:= -Wall $(CXXFLAGS)
CXXFLAGS+= -O -Wuninitialized
CXXFLAGS+= -Wno-long-long -Wno-multichar -Wno-unknown-pragmas
# Even more warnings...
CXXFLAGS+= -pedantic -Wpointer-arith -Wcast-qual -Wconversion
CXXFLAGS+= -Wshadow -Wimplicit -Wundef -Wnon-virtual-dtor
CXXFLAGS+= -Wno-reorder -Wwrite-strings -fcheck-new -Wctor-dtor-privacy 

######################################################################
# Plugin settings
######################################################################

# Plugin prefix. Typically "lib" on Unix, and nothing everywhere else
PLUGIN_PREFIX := lib
# Plugin suffix. For static/shared libs this is typically ".so"/".a" on Unix,
# ".dll"/".lib" on Windows, ".bundle"/".a" on OS X, etc.
PLUGIN_SUFFIX := .so

PLUGIN_EXTRA_DEPS := $(EXECUTABLE)

#######################################################################
# Misc stuff - you should never have to edit this                     #
#######################################################################

EXECUTABLE  := scummvm$(EXEEXT)

include $(srcdir)/Makefile.common

# check if configure has been run or has been changed since last run
config.mak: $(srcdir)/configure
	@echo "You need to run ./configure before you can run make"
	@echo "Either you haven't run it before or it has changed."
	@exit 1

install: all
	$(INSTALL) -d "$(BINDIR)"
	$(INSTALL) -c -s -m 755 "$(srcdir)/scummvm$(EXEEXT)" "$(BINDIR)/scummvm$(EXEEXT)"
	$(INSTALL) -d "$(MANDIR)/man6/"
	$(INSTALL) -c -m 644 "$(srcdir)/scummvm.6" "$(MANDIR)/man6/scummvm.6"
	$(INSTALL) -d "$(PREFIX)/share/pixmaps/"
	$(INSTALL) -c -m 644 "$(srcdir)/scummvm.xpm" "$(PREFIX)/share/pixmaps/scummvm.xpm"
	$(INSTALL) -d "$(PREFIX)/share/doc/scummvm/"
	$(INSTALL) -c -m 644 "$(srcdir)/AUTHORS" "$(srcdir)/COPYING" "$(srcdir)/NEWS" "$(srcdir)/README" "$(PREFIX)/share/doc/scummvm/"

uninstall:
	rm -f "$(BINDIR)/scummvm$(EXEEXT)"
	rm -f "$(MANDIR)/man6/scummvm.6"
	rm -f "$(PREFIX)/share/pixmaps/scummvm.xpm"
	rm -rf "$(PREFIX)/share/doc/scummvm/"

scummvmico.o: scummvm.ico
	windres scummvm.rc scummvmico.o

dist:
	$(RM) $(ZIPFILE)
	$(ZIP) $(ZIPFILE) $(DISTFILES)

deb:
	ln -sf dists/debian;
	debian/prepare
	fakeroot debian/rules binary

#######################################################################
# Unit/regression tests                                               #
# In order to use 'make test' you have to install cxxtest inside the  #
# test/cxxtest dir. Get it from http://cxxtest.sourceforge.net.       #
#######################################################################

CXXTEST := test/cxxtest
TESTS := test/common/*.h
CPPFLAGS += -I$(CXXTEST)
test: runner
	./runner
runner: runner.o common/libcommon.a
	$(CXX) -o $@ $+
runner.cpp: $(TESTS)
	$(CXXTEST)/cxxtestgen.py --error-printer -o  $@ $+



# Special target to create a application wrapper for Mac OS X
bundle_name = ScummVM.app
bundle: scummvm-static
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	cp $(srcdir)/Info.plist $(bundle_name)/Contents/
	cp $(srcdir)/scummvm.icns $(bundle_name)/Contents/Resources/
	cp $(srcdir)/scummvm-static $(bundle_name)/Contents/MacOS/scummvm
	strip $(bundle_name)/Contents/MacOS/scummvm

# location of additional libs for OS X usually /sw/ for fink or
# /opt/local/ for darwinports
OSXOPT=/sw
# Special target to create a static linked binary for Mac OS X
scummvm-static: $(OBJS)
	$(CXX) $(LDFLAGS) -o scummvm-static $(OBJS) \
		`sdl-config --static-libs` \
		$(OSXOPT)/lib/libmad.a \
		$(OSXOPT)/lib/libvorbisfile.a \
		$(OSXOPT)/lib/libvorbis.a \
		$(OSXOPT)/lib/libogg.a \
		$(OSXOPT)/lib/libmpeg2.a \
		$(OSXOPT)/lib/libFLAC.a \
		-lz

# Special target to create a snapshot disk image for Mac OS X
osxsnap: bundle
	mkdir ScummVM-snapshot
	cp AUTHORS ./ScummVM-snapshot/ScummVM\ Authors
	cp COPYING ./ScummVM-snapshot/License
	cp NEWS ./ScummVM-snapshot/News
	cp README ./ScummVM-snapshot/ScummVM\ ReadMe
	/Developer/Tools/SetFile -t TEXT -c ttxt ./ScummVM-snapshot/*
	/Developer/Tools/CpMac -r $(bundle_name) ./ScummVM-snapshot/
	hdiutil create -ov -format UDZO -srcfolder ScummVM-snapshot ScummVM-snapshot.dmg
	rm -rf ScummVM-snapshot

# Special target to create a win32 snapshot binary
win32dist: scummvm$(EXEEXT)
	mkdir -p $(WIN32PATH)
	strip scummvm.exe -o $(WIN32PATH)/scummvm$(EXEEXT)
	cp AUTHORS $(WIN32PATH)/AUTHORS.txt
	cp COPYING $(WIN32PATH)/COPYING.txt
	cp NEWS $(WIN32PATH)/NEWS.txt
	cp README $(WIN32PATH)/README.txt
	cp /usr/local/README-SDL.txt $(WIN32PATH)
	cp /usr/local/bin/SDL.dll $(WIN32PATH)
	u2d $(WIN32PATH)/*.txt


.PHONY: deb bundle test osxsnap win32dist dist install uninstall
