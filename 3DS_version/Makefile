# TARGET #

TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif
endif

# PORTLIBS #

export PORTLIBS_PATH	:=	$(DEVKITPRO)/portlibs
PORTLIBS	:=	$(PORTLIBS_PATH)/armv6k $(PORTLIBS_PATH)/3ds

# COMMON CONFIGURATION #

NAME := NDS-controller

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS := include
SOURCE_DIRS := source

EXTRA_OUTPUT_FILES :=

LIBRARY_DIRS := $(DEVKITPRO)/libctru $(PORTLIBS)
LIBRARIES := 

BUILD_FLAGS := -DVERSION='"0.1.1"'
RUN_FLAGS :=

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    LIBRARIES += citro3dd ctrud
    BUILD_FLAGS += -Og
else
    LIBRARIES += citro3d ctru
    BUILD_FLAGS += -O4
endif

# 3DS/Wii U CONFIGURATION #

ifeq ($(TARGET),$(filter $(TARGET),3DS WIIU))
    TITLE := NDS-controller
    DESCRIPTION := Use your DS as a controller for Android!
    AUTHOR := Louisvh
endif

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    LIBRARY_DIRS += $(DEVKITPRO)/libctru
    LIBRARIES += ctru

    PRODUCT_CODE := CTR-P-NDSC
    UNIQUE_ID := 0x22322

    CATEGORY := Application
    USE_ON_SD := false

    MEMORY_TYPE := Application
    SYSTEM_MODE := 64MB
    SYSTEM_MODE_EXT := 124MB
    CPU_SPEED := 804MHz
    ENABLE_L2_CACHE := true

    ICON_FLAGS :=

    ROMFS_DIR := romfs
    BANNER_AUDIO := meta/banner.wav
    BANNER_IMAGE := meta/banner.png
    ICON := meta/icon.png
    LOGO := 
endif

# INTERNAL #

include buildtools/make_base
