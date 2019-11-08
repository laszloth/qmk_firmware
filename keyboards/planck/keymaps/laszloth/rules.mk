# laszloth keymap

GIT_DESC = $(shell git describe --tags --dirty --always --abbrev=10)
CFLAGS += -DSERIAL_NUMBER=\"$(GIT_DESC)\"

BOOTMAGIC_ENABLE = no
TAP_DANCE_ENABLE = yes

ifeq ($(strip $(KEYBOARD)), planck/rev6)
COMBO_ENABLE = no
DYNAMIC_MACRO_ENABLE = yes
endif

ifeq ($(strip $(KEYBOARD)), planck/rev4)
AUDIO_ENABLE = no
BACKLIGHT_ENABLE = yes
CONSOLE_ENABLE = no
endif

ifeq ($(strip $(AUDIO_ENABLE)), yes)
SRC += muse.c
endif
