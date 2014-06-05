MODULE := engines/bladerunner

MODULE_OBJS = \
	archive.o \
	aud_decoder.o \
	bladerunner.o \
	chapters.o \
	decompress_lcw.o \
	decompress_lzo.o \
	detection.o \
	gameinfo.o \
	image.o \
	matrix.o \
	outtake.o \
	scene.o \
	script/rc01.o \
	script/script.o \
	set.o \
	settings.o \
	view.o \
	vqa_decoder.o \
	vqa_player.o

# This module can be built as a plugin
ifeq ($(ENABLE_BLADERUNNER), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
