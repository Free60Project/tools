#
#             LUFA Library
#     Copyright (C) Dean Camera, 2012.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

MCU          = at90usb162
ARCH         = AVR8
BOARD        = OLIMEX162
F_CPU        = 8000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = isd2100_flasher
SRC          = main.c isd2100.c Descriptors.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = ../LUFA-120730/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =

# Default target
all:

zip:
	tar cfvz isd2100.tar.gz *.c *.h isd2100.py Makefile README Config

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
