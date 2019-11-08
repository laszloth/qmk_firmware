/* Copyright 2015-2017 Jack Humbert
 * Copyright 2019-2020 Laszlo Toth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "muse.h"

enum planck_layers {
    _QWERTY,
    _LOWER,
    _RAISE,
    _PLOVER,
    _ADJUST
};

enum planck_keycodes {
    QWERTY = SAFE_RANGE,
    PLOVER,
    BACKLIT,
    EXT_PLV,
    TOGGLE_INPUT,
    MACRO_COPYALL,
};

#ifdef TAP_DANCE_ENABLE
enum td_actions {
    TD_LSFT,
    TD_LCTL,
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_LSFT] = ACTION_TAP_DANCE_DOUBLE(KC_LSFT, KC_CAPS),
    [TD_LCTL] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, KC_RCTL),
};
#endif /* ifdef TAP_DANCE_ENABLE */

#ifdef COMBO_ENABLE
enum combo_events {
    COMBO_ASA,
};

const uint16_t PROGMEM combo_asa[] = {KC_A, KC_S, KC_A, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    [COMBO_ASA] = COMBO_ACTION(combo_asa),
};
#endif /* ifdef COMBO_ENABLE */

#ifdef RGBLIGHT_LAYERS
/* All LEDs red when Caps Lock is on */
const rgblight_segment_t PROGMEM rgb_cl_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, RGBLIGHT_LED_COUNT, HSV_RED });

/* All LEDs cyan when L1 */
const rgblight_segment_t PROGMEM rgb_l1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, RGBLIGHT_LED_COUNT, HSV_CYAN });

/* All LEDs purple when L2 is on */
const rgblight_segment_t PROGMEM rgb_l2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, RGBLIGHT_LED_COUNT, HSV_PURPLE });

const rgblight_segment_t* const PROGMEM rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    rgb_cl_layer,
    rgb_l1_layer,
    rgb_l2_layer
);
#endif /* ifdef RGBLIGHT_LAYERS */

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

#ifdef AUDIO_ENABLE
static bool muse_mode = false;
static uint8_t last_muse_note = 0;
static uint16_t muse_counter = 0;
static uint8_t muse_offset = 70;
static uint16_t muse_tempo = 50;

static float plover_song[][2]    = SONG(PLOVER_SOUND);
static float plover_gb_song[][2] = SONG(PLOVER_GOODBYE_SOUND);

static bool noisy_mode;
static float audio_on_song[][2]  = SONG(AUDIO_ON_SOUND);
#ifdef KEYBOARD_planck_rev6
static float audio_off_song[][2] = SONG(AUDIO_OFF_SOUND);
#endif /* ifdef KEYBOARD_planck_rev6 */
static float toggle_input_on_song[][2] = SONG(PLOVER_SOUND);
static float toggle_input_off_song[][2] = SONG(PLOVER_GOODBYE_SOUND);
#endif /* ifdef AUDIO_ENABLE */

static bool disable_keyboard_input;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* Qwerty
     * ,-----------------------------------------------------------------------------------.
     * | Tab  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | End  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Esc  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |Backlt| Ctrl | Alt  | GUI  |Lower |Space | Bksp |Raise | Left | Down |  Up  |Right |
     * `-----------------------------------------------------------------------------------'
     */
    [_QWERTY] = LAYOUT_planck_grid(
            KC_TAB,      KC_Q,        KC_W,    KC_E,    KC_R,  KC_T,   KC_Y,    KC_U,  KC_I,    KC_O,    KC_P,    KC_END,
            KC_ESC,      KC_A,        KC_S,    KC_D,    KC_F,  KC_G,   KC_H,    KC_J,  KC_K,    KC_L,    KC_SCLN, KC_QUOT,
            TD(TD_LSFT), KC_Z,        KC_X,    KC_C,    KC_V,  KC_B,   KC_N,    KC_M,  KC_COMM, KC_DOT,  KC_SLSH, KC_ENT ,
            BACKLIT,     TD(TD_LCTL), KC_LALT, KC_LGUI, LOWER, KC_SPC, KC_BSPC, RAISE, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
            ),

    /* Lower
     * ,-----------------------------------------------------------------------------------.
     * |   ~  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  | End  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Del  |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |   _  |   +  |   {  |   }  |  |   |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |ISO / |ISO | | Home | End  |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |DMREC1|DMSTOP|DMPLAY|      |      |      |      | Next | Vol- | Vol+ | Play |
     * `-----------------------------------------------------------------------------------'
     */
    [_LOWER] = LAYOUT_planck_grid(
            KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR,    KC_LPRN, KC_RPRN, KC_END,
            KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_UNDS, KC_PLUS,    KC_LCBR, KC_RCBR, KC_PIPE,
            _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NUBS, S(KC_NUBS), KC_HOME, KC_END,  _______,
            _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT,    KC_VOLD, KC_VOLU, KC_MPLY
            ),

    /* Raise
     * ,-----------------------------------------------------------------------------------.
     * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | End  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Del  |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |   -  |   =  |   [  |   ]  |  \   |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |ISO ~ |ISO # |Pg Up |Pg Dn |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      | Next | Vol- | Vol+ | Play |
     * `-----------------------------------------------------------------------------------'
     */
    [_RAISE] = LAYOUT_planck_grid(
            KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,       KC_8,    KC_9,    KC_0,    KC_END,
            KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS,    KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
            _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  S(KC_NUHS), KC_NUHS, KC_PGUP, KC_PGDN, _______,
            _______, _______, _______, _______, _______, _______, _______, _______,    KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
            ),

    /* Plover layer (http://opensteno.org)
     * ,-----------------------------------------------------------------------------------.
     * |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |   #  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |   S  |   T  |   P  |   H  |   *  |   *  |   F  |   P  |   L  |   T  |   D  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |   S  |   K  |   W  |   R  |   *  |   *  |   R  |   B  |   G  |   S  |   Z  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Exit |      |      |   A  |   O  |      |      |   E  |   U  |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */
    [_PLOVER] = LAYOUT_planck_grid(
            KC_1,    KC_1,    KC_1,    KC_1, KC_1, KC_1,    KC_1,    KC_1, KC_1, KC_1,    KC_1,    KC_1   ,
            _______, KC_Q,    KC_W,    KC_E, KC_R, KC_T,    KC_Y,    KC_U, KC_I, KC_O,    KC_P,    KC_LBRC,
            _______, KC_A,    KC_S,    KC_D, KC_F, KC_G,    KC_H,    KC_J, KC_K, KC_L,    KC_SCLN, KC_QUOT,
            EXT_PLV, _______, _______, KC_C, KC_V, _______, _______, KC_N, KC_M, _______, _______, _______
            ),

    /* Adjust (Lower + Raise)
     *                      v------------------------RGB CONTROL--------------------v
     * ,-----------------------------------------------------------------------------------.
     * |      | Reset|Debug | RGB  |RGBMOD| HUE+ | HUE- | SAT+ | SAT- |BRGTH+|BRGTH-|Tglinp|
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |MUSmod|Aud on|Audoff|AGnorm|AGswap|Qwerty|      |      |Plover|      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |Voice-|Voice+|Mus on|Musoff|MIDIon|MIDIof|TermOn|TermOf|      |      |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |             |      |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */
    [_ADJUST] = LAYOUT_planck_grid(
            _______, QK_BOOT, DB_TOGG, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, TOGGLE_INPUT,
            _______, _______, MU_NEXT, AU_ON,   AU_OFF,  AG_NORM, AG_SWAP, QWERTY,  _______, _______, PLOVER,  _______,
            _______, AU_PREV, AU_NEXT, MU_ON,   MU_OFF,  MI_ON,   MI_OFF,  _______, _______, _______, _______, _______,
            _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
            )

};

#ifdef RGBLIGHT_LAYERS
layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(1, layer_state_cmp(state, 1));
    rgblight_set_layer_state(2, layer_state_cmp(state, 2));
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(0, led_state.caps_lock);
    return true;
}
#endif /* ifdef RGBLIGHT_LAYERS */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == TOGGLE_INPUT) {
        if (record->event.pressed) {
            disable_keyboard_input = !disable_keyboard_input;
#ifdef AUDIO_ENABLE
            if (disable_keyboard_input)
                PLAY_SONG(toggle_input_off_song);
            else
                PLAY_SONG(toggle_input_on_song);
#endif
        }
        return false;
    }

    if (disable_keyboard_input) {
        if (record->event.pressed) {
            dprintf("Keyboard input is disabled\n");
        }
        return false;
    }

    switch (keycode) {
        case QWERTY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_QWERTY);
            }
            return false;
            break;
        case BACKLIT:
            if (record->event.pressed) {
#ifdef RGBLIGHT_ENABLE
                if (IS_LAYER_ON(_LOWER)) {
                    dprintf("** Saving current RGB mode to EEPROM **\n");
                    rgblight_mode(rgblight_get_mode());
                } else {
                    dprintf("** Stepping RGB mode **\n");
                    rgblight_step_noeeprom();
                }
#endif

                if (IS_LAYER_ON(_LOWER)) {
#ifdef AUDIO_ENABLE
                    clicky_toggle();
                    noisy_mode = !noisy_mode;
                    if (noisy_mode)
                        PLAY_SONG(audio_on_song);
#endif
                } else {
#ifdef BACKLIGHT_ENABLE
                    backlight_step();
#endif
                }

#ifdef KEYBOARD_planck_rev5
                writePinLow(E6);
#endif
            } else {
#ifdef KEYBOARD_planck_rev5
                writePinHigh(E6);
#endif
            }
            return false;
            break;
        case PLOVER:
            if (record->event.pressed) {
#ifdef AUDIO_ENABLE
                stop_all_notes();
                PLAY_SONG(plover_song);
#endif
                layer_off(_RAISE);
                layer_off(_LOWER);
                layer_off(_ADJUST);
                layer_on(_PLOVER);
                if (!eeconfig_is_enabled()) {
                    eeconfig_init();
                }
                keymap_config.raw = eeconfig_read_keymap();
                keymap_config.nkro = 1;
                eeconfig_update_keymap(keymap_config.raw);
            }
            return false;
            break;
        case EXT_PLV:
            if (record->event.pressed) {
#ifdef AUDIO_ENABLE
                PLAY_SONG(plover_gb_song);
#endif
                layer_off(_PLOVER);
            }
            return false;
            break;
            /* macros */
        case MACRO_COPYALL:
            if (record->event.pressed)
                SEND_STRING(SS_LCTL("ac")); // selects all and copies
            return false;
            break;
    }
    return true;
}

#ifdef COMBO_ENABLE
void process_combo_event(uint8_t combo_index, bool pressed) {
    if (pressed) {
        dprintf("Combo event #%u registered\n", combo_index);

        switch(combo_index) {
            case COMBO_ASA:
                tap_code16(KC_ESC);
                break;
        }
    }
}
#endif /* ifdef COMBO_ENABLE */

#ifdef KEYBOARD_planck_rev6
static void encoder_tap_code(uint8_t code) {
    register_code(code);
    wait_ms(10);
    unregister_code(code);
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    dprintf("Encoder %s\n", clockwise ? "CW" : "CCW");

#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (IS_LAYER_ON(_RAISE)) {
            if (clockwise) {
                muse_offset++;
            } else {
                muse_offset--;
            }
        } else {
            if (clockwise) {
                muse_tempo+=1;
            } else {
                muse_tempo-=1;
            }
        }
    } else {
#endif /* ifdef AUDIO_ENABLE */
        if (clockwise) {
            encoder_tap_code(KC_VOLU);
#ifdef AUDIO_ENABLE
            if (noisy_mode)
                PLAY_SONG(audio_on_song);
#endif
        } else {
            encoder_tap_code(KC_VOLD);
#ifdef AUDIO_ENABLE
            if (noisy_mode)
                PLAY_SONG(audio_off_song);
#endif
        }
#ifdef AUDIO_ENABLE
    }
#endif /* ifdef AUDIO_ENABLE */

    return true;
}

bool dip_switch_update_user(uint8_t index, bool active) {
#ifdef AUDIO_ENABLE
    static bool play_sound = false;
#endif

    dprintf("DIP state change: %u to %s\n", index+1, active ? "ON" : "OFF");

    switch (index) {

        /* Toggle adjust layer  */
        case 0:
            if (active) {
#ifdef AUDIO_ENABLE
                if (play_sound) { PLAY_SONG(plover_song); }
#endif
                layer_on(_ADJUST);
            } else {
#ifdef AUDIO_ENABLE
                if (play_sound) { PLAY_SONG(plover_gb_song); }
#endif
                layer_off(_ADJUST);
            }
            break;

        /* Toggle muse mode */
        case 1:
#ifdef AUDIO_ENABLE
            muse_mode = active;
#endif
            break;

        /* Toggle keyboard input disabled/enabled */
        case 2:
            disable_keyboard_input = active;
            break;

        /* Toggle noisy mode */
        case 3:
#ifdef AUDIO_ENABLE
#ifdef AUDIO_CLICKY
            active ? clicky_on() : clicky_off();
#endif
            noisy_mode = active;
            if (play_sound && active) { PLAY_SONG(audio_on_song); }
            play_sound = true;
#endif
            break;
    }

    return true;
}
#endif /* ifdef KEYBOARD_planck_rev6 */

#ifdef AUDIO_ENABLE
void matrix_scan_user(void) {
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
}

bool music_mask_user(uint16_t keycode) {
    switch (keycode) {
        case RAISE:
        case LOWER:
            return false;
        default:
            return true;
    }
}
#endif /* ifdef AUDIO_ENABLE */

void keyboard_post_init_user(void) {
    //debug_enable=true;
    //debug_matrix=true;
    //debug_keyboard=true;
    //debug_mouse=true;

#ifdef AUDIO_CLICKY
    clicky_off();
#endif /* ifdef AUDIO_CLICKY */

#ifdef RGBLIGHT_LAYERS
    /* Enable the LED layers */
    rgblight_layers = rgb_layers;
#endif /* ifdef RGBLIGHT_LAYERS */
}

// vim: tabstop=4 softtabstop=4 shiftwidth=4 expandtab textwidth=80 filetype=c
