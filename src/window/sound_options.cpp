#include "sound_options.h"

#include "game/settings.h"
#include "graphics/elements/arrow_button.h"
#include "graphics/elements/generic_button.h"
#include "graphics/boilerplate.h"
#include "graphics/elements/lang_text.h"
#include "graphics/elements/panel.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "input/input.h"
#include "sound/city.h"
#include "sound/effect.h"
#include "sound/music.h"
#include "sound/speech.h"
#include "graphics/image.h"

static void button_toggle(int type, int param2);
static void button_ok(int param1, int param2);
static void button_cancel(int param1, int param2);

static void arrow_button_music(int is_down, int param2);
static void arrow_button_speech(int is_down, int param2);
static void arrow_button_effects(int is_down, int param2);
static void arrow_button_city(int is_down, int param2);

static generic_button buttons[] = {
        {64,  162, 224, 20, button_toggle, button_none, static_cast<int>(SoundType::MUSIC),   0},
        {64,  192, 224, 20, button_toggle, button_none, static_cast<int>(SoundType::SPEECH),  0},
        {64,  222, 224, 20, button_toggle, button_none, static_cast<int>(SoundType::EFFECTS), 0},
        {64,  252, 224, 20, button_toggle, button_none, static_cast<int>(SoundType::CITY),    0},
        {144, 296, 192, 20, button_ok,     button_none, 1,             0},
        {144, 326, 192, 20, button_cancel, button_none, 1,             0},
};

static arrow_button arrow_buttons[] = {
        {112, 100, 17, 24, arrow_button_music,   1, 0},
        {136, 100, 15, 24, arrow_button_music,   0, 0},
        {112, 130, 17, 24, arrow_button_speech,  1, 0},
        {136, 130, 15, 24, arrow_button_speech,  0, 0},
        {112, 160, 17, 24, arrow_button_effects, 1, 0},
        {136, 160, 15, 24, arrow_button_effects, 0, 0},
        {112, 190, 17, 24, arrow_button_city,    1, 0},
        {136, 190, 15, 24, arrow_button_city,    0, 0},
};

static struct {
    int focus_button_id;
    void (*close_callback)(void);

    SettingSound original_effects;
    SettingSound original_music;
    SettingSound original_speech;
    SettingSound original_city;
} data;

static void init(void (*close_callback)(void)) {
    data.focus_button_id = 0;
    data.close_callback = close_callback;

    auto& settings = Settings::instance();
    data.original_effects = settings.sound(SoundType::EFFECTS);
    data.original_music = settings.sound(SoundType::MUSIC);
    data.original_speech = settings.sound(SoundType::SPEECH);
    data.original_city = settings.sound(SoundType::CITY);
}

static void draw_foreground(void) {
    graphics_set_to_dialog();

    outer_panel_draw(48, 80, 24, 18);

    // on/off labels
    label_draw(64, 162, 14, data.focus_button_id == 1 ? 1 : 2);
    label_draw(64, 192, 14, data.focus_button_id == 2 ? 1 : 2);
    label_draw(64, 222, 14, data.focus_button_id == 3 ? 1 : 2);
    label_draw(64, 252, 14, data.focus_button_id == 4 ? 1 : 2);
    // ok/cancel labels
    label_draw(144, 296, 12, data.focus_button_id == 5 ? 1 : 2);
    label_draw(144, 326, 12, data.focus_button_id == 6 ? 1 : 2);

    // title
    lang_text_draw_centered(46, 0, 96, 92, 288, FONT_LARGE_BLACK_ON_LIGHT);

    lang_text_draw_centered(46, 12, 128, 300, 224, FONT_NORMAL_BLACK_ON_DARK);
    lang_text_draw_centered(46, 9, 128, 330, 224, FONT_NORMAL_BLACK_ON_DARK);

    lang_text_draw(46, 10, 112, 142, FONT_SMALL_PLAIN);
    lang_text_draw(46, 11, 336, 142, FONT_SMALL_PLAIN);

    auto& settings = Settings::instance();
    const auto music = settings.sound(SoundType::MUSIC);
    lang_text_draw_centered(46, music.enabled ? 2 : 1, 64, 166, 224, FONT_NORMAL_BLACK_ON_DARK);
    text_draw_percentage(music.volume, 374, 166, FONT_SMALL_PLAIN);

    const auto speech = settings.sound(SoundType::SPEECH);
    lang_text_draw_centered(46, speech.enabled ? 4 : 3, 64, 196, 224, FONT_NORMAL_BLACK_ON_DARK);
    text_draw_percentage(speech.volume, 374, 196, FONT_SMALL_PLAIN);

    const auto effects = settings.sound(SoundType::EFFECTS);
    lang_text_draw_centered(46, effects.enabled ? 6 : 5, 64, 226, 224, FONT_NORMAL_BLACK_ON_DARK);
    text_draw_percentage(effects.volume, 374, 226, FONT_SMALL_PLAIN);

    const auto city = settings.sound(SoundType::CITY);
    lang_text_draw_centered(46, city.enabled ? 8 : 7, 64, 256, 224, FONT_NORMAL_BLACK_ON_DARK);
    text_draw_percentage(city.volume, 374, 256, FONT_SMALL_PLAIN);

    arrow_buttons_draw(208, 60, arrow_buttons, 8);

    graphics_reset_dialog();
}

static void handle_input(const mouse *m, const hotkeys *h) {
    const mouse *m_dialog = mouse_in_dialog(m);
    if (generic_buttons_handle_mouse(m_dialog, 0, 0, buttons, 6, &data.focus_button_id) ||
        arrow_buttons_handle_mouse(m_dialog, 208, 60, arrow_buttons, 8, 0))
        return;
    if (input_go_back_requested(m, h))
        data.close_callback();

}

static void button_toggle(int type, int param2) {
    auto& settings = Settings::instance();
    settings.toggle_sound_enabled(static_cast<SoundType>(type));
    if (static_cast<SoundType>(type) == SoundType::MUSIC) {
        if (settings.sound(SoundType::MUSIC).enabled) {
            sound_music_update(true);
        }
        else {
            sound_music_stop();
        }
    } else if (static_cast<SoundType>(type) == SoundType::SPEECH) {
        if (!settings.sound(SoundType::SPEECH).enabled) {
            sound_speech_stop();
        }
    }
}

static void button_ok(int param1, int param2) {
    data.close_callback();
}

static void button_cancel(int param1, int param2) {
    auto& settings = Settings::instance();
    settings.reset_sound(SoundType::EFFECTS, data.original_effects);
    settings.reset_sound(SoundType::MUSIC, data.original_music);
    settings.reset_sound(SoundType::SPEECH, data.original_speech);
    settings.reset_sound(SoundType::CITY, data.original_city);
    if (data.original_music.enabled) {
        if (settings.sound(SoundType::MUSIC).enabled != data.original_music.enabled) {
            sound_music_update(true);
        }

    } else {
        sound_music_stop();
    }
    sound_music_set_volume(data.original_music.volume);
    sound_speech_set_volume(data.original_speech.volume);
    sound_effect_set_volume(data.original_effects.volume);
    sound_city_set_volume(data.original_city.volume);

    data.close_callback();
}

static void update_volume(int type, int is_decrease) {
    auto& settings = Settings::instance();
    if (is_decrease) {
        settings.decrease_sound_volume(static_cast<SoundType>(type));
    }
    else {
        settings.increase_sound_volume(static_cast<SoundType>(type));
    }
}

static void arrow_button_music(int is_down, int param2) {
    auto& settings = Settings::instance();
    update_volume(static_cast<int>(SoundType::MUSIC), is_down);
    sound_music_set_volume(settings.sound(SoundType::MUSIC).volume);
}

static void arrow_button_speech(int is_down, int param2) {
    auto& settings = Settings::instance();
    update_volume(static_cast<int>(SoundType::SPEECH), is_down);
    sound_speech_set_volume(settings.sound(SoundType::SPEECH).volume);
}

static void arrow_button_effects(int is_down, int param2) {
    auto& settings = Settings::instance();
    update_volume(static_cast<int>(SoundType::EFFECTS), is_down);
    sound_effect_set_volume(settings.sound(SoundType::EFFECTS).volume);
}

static void arrow_button_city(int is_down, int param2) {
    auto& settings = Settings::instance();
    update_volume(static_cast<int>(SoundType::CITY), is_down);
    sound_city_set_volume(settings.sound(SoundType::CITY).volume);
}

void window_sound_options_show(void (*close_callback)(void)) {
    window_type window = {
            WINDOW_SOUND_OPTIONS,
            window_draw_underlying_window,
            draw_foreground,
            handle_input,
    };
    init(close_callback);
    window_show(&window);
}
