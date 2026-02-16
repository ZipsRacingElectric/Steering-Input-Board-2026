# Project name
PROJECT = sib_2026

# Imported files
CHIBIOS  := $(CHIBIOS_SOURCE_PATH)

# Directories
CONFDIR		:= ./config
BUILDDIR	:= ./build
DEPDIR		:= ./build/dep
BOARDDIR	:= ./build/board
COMMONDIR	:= ./common

# Includes
ALLINC += src

# Source files
CSRC =	$(ALLCSRC)		\
		src/main.c		\
		src/peripherals/steering_input.c	
		
# Common library includes
include common/src/debug.mk
include common/src/fault_handler.mk
include common/src/peripherals/adc/stm_adc.mk
include common/src/peripherals/adc/analog_linear.mk

# Compiler flags
USE_OPT += -Og -Wall -Wextra

# C macro definitions
UDEFS +=

# ASM definitions
UADEFS +=

# Include directories
UINCDIR +=

# Library directories
ULIBDIR +=

# Libraries
ULIBS +=

# Common toolchain includes
include common/common.mk
include common/make/openocd.mk

# ChibiOS compilation hooks
PRE_MAKE_ALL_RULE_HOOK: $(BOARD_FILES) $(CLANGD_FILE)