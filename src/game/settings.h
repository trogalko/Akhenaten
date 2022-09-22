#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include "city/constants.h"
#include "core/game_environment.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

enum class Tooltips {
    NONE = 0,
    SOME = 1,
    FULL = 2
};

enum class Difficulty {
    VERY_EASY = 0,
    EASY = 1,
    NORMAL = 2,
    HARD = 3,
    VERY_HARD = 4
};

enum class SoundType {
    MUSIC = 1,
    SPEECH = 2,
    EFFECTS = 3,
    CITY = 4,
};

enum class CitiesNamesStyle {
    OLD_NAMES = 0,
    NEW_NAMES = 1
};

struct SettingSound {
    bool enabled{true};
    int volume{100};
};

class buffer;
class Settings {
private:
    explicit Settings();
public:
    ~Settings() = default;

    Settings(Settings && rhs) = default;
    Settings& operator=(Settings && rhs) = default;

    Settings(const Settings& rhs) = delete;
    Settings& operator=(const Settings& rhs) = delete;

    static Settings& instance()
    {
        static Settings settings;
        return settings;
    }

    void load();
    void save();

    bool fullscreen() const;
    void toggle_fullscreen();  
    int window_width() const;
    int window_height() const;
    void set_display(bool fullscreen, int window_width, int window_height);

    SettingSound sound(SoundType sound_type) const;
    void toggle_sound_enabled(SoundType sound_type);
    void increase_sound_volume(SoundType sound_type);
    void decrease_sound_volume(SoundType sound_type);
    void reset_sound(SoundType sound_type, const SettingSound& set_sound);

    int game_speed() const;
    void increase_game_speed();
    void decrease_game_speed();

    int scroll_speed() const;
    void increase_scroll_speed(); 
    void decrease_scroll_speed(); 

    void reset_speeds(int game_speed, int scroll_speed);

    Tooltips tooltips() const;
    void cycle_tooltips();

    bool warnings() const;
    void toggle_warnings();

    bool monthly_autosave() const;
    void toggle_monthly_autosave();

    CitiesNamesStyle city_names_style() const;
    void cycle_city_names_style();

    bool pyramid_speedup() const;
    void toggle_pyramid_speedup();

    bool gods_enabled() const;
    void toggle_gods_enabled();

    // TODO consider add docs how this collection of flags is organized.
    // Are these different bits in a byte?
    // If yes maybe use std::bitset for clarity.
    int popup_messages() const;
    void toggle_popup_messages(int flag);

    Difficulty difficulty() const;
    void increase_difficulty();
    void decrease_difficulty();

    bool victory_video() const;

    Advisor last_advisor() const;
    void set_last_advisor(Advisor advisor);

    const uint8_t *player_name() const;
    const char *player_name_utf8() const;
    void set_player_name(const uint8_t *player_name);

    // TODO consider explaining what savings int means.
    // is it like a set of bits?
    int personal_savings_for_mission(int mission_id) const;
    void set_personal_savings_for_mission(int mission_id, int savings);
    void clear_personal_savings();

private: 
    void _load_default_settings();
    void _load_settings(buffer *buffer);
    const SettingSound* _sound(SoundType sound_type) const; 
    SettingSound* _sound(SoundType sound_type); 

    static constexpr int INF_SIZE = 560;
    static constexpr int MAX_PERSONAL_SAVINGS = 100;
    static constexpr std::string_view SETTINGS_FILENAME = "c3.inf";

    // display settings
    bool m_fullscreen{};
    int m_window_width{};
    int m_window_height{};
    // sound settings
    SettingSound m_sound_effects{};
    SettingSound m_sound_music{};
    SettingSound m_sound_speech{};
    SettingSound m_sound_city{};
    // speed settings
    int m_game_speed{};
    int m_scroll_speed{};
    // misc settings
    Difficulty m_difficulty{};
    Tooltips m_tooltips{};
    int m_monthly_autosave{};
    bool m_warnings{};
    bool m_gods_enabled{};
    bool m_victory_video{};
    // pharaoh settings
    int m_popup_messages{};
    CitiesNamesStyle m_city_names_style{};
    bool m_pyramid_speedup{};
    // persistent game state
    Advisor m_last_advisor{};
    std::array<uint8_t, MAX_PLAYER_NAME> m_player_name;
    std::array<char, MAX_PLAYER_NAME> m_player_name_utf8;
    // personal savings
    std::array<int, MAX_PERSONAL_SAVINGS> m_personal_savings;
    // file data
    std::shared_ptr<buffer> m_inf_file{nullptr};
};

#endif // GAME_SETTINGS_H