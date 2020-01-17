# laszloth keymap

TAP_DANCE_ENABLE = yes

ifeq ($(strip $(KEYBOARD)), planck/rev6)
DYNAMIC_MACRO_ENABLE = yes
COMBO_ENABLE = yes
endif

ifeq ($(strip $(KEYBOARD)), planck/rev4)
CONSOLE_ENABLE = no
BACKLIGHT_ENABLE = yes
endif

ifeq ($(strip $(AUDIO_ENABLE)), yes)
SRC += muse.c
endif
