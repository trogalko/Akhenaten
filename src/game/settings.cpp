#include "settings.h"

#include "core/buffer.h"
#include "core/encoding.h"
#include "core/string.h"
#include "io/io.h"

#include <algorithm>

Settings::Settings()
    :m_inf_file{std::make_unique<buffer>(INF_SIZE)}
{
}

void Settings::load() 
{
    _load_default_settings();

    // TODO: load <Pharaoh.inf>
    int size = io_read_file_into_buffer(
        SETTINGS_FILENAME.data(), NOT_LOCALIZED, m_inf_file.get(), INF_SIZE);
    if (!size) {
        return;
    }
    _load_settings(m_inf_file.get());

    if (m_window_width + m_window_height < 500) {
        // most likely migration from Caesar 3
        m_window_width = 800;
        m_window_height = 600;
    }
}

void Settings::save()
{
    auto buffer = m_inf_file;

    constexpr int data1 = 1;
    constexpr int data3 = 3;
    constexpr int data4 = 4;
    constexpr int data6 = 6;
    constexpr int data8 = 8;
    constexpr int data16 = 16;

    constexpr int data_save_game_mission_id = data4;
    constexpr int data_starting_kingdom = data4;
    constexpr int data_personal_savings_last_mission = data4;
    constexpr int data_current_mission_id = data4;
    constexpr int data_is_custom_scenario = data4;
    constexpr int data_autoclear = data1;

    constexpr int data_ram = data8;
    constexpr int max_confirmed_resolution = data8;

    buffer->skip(data4);
    buffer->write_i32(static_cast<int32_t>(m_fullscreen));
    buffer->skip(data3);
    buffer->write_u8(static_cast<uint8_t>(m_sound_effects.enabled));
    buffer->write_u8(static_cast<uint8_t>(m_sound_music.enabled));
    buffer->write_u8(static_cast<uint8_t>(m_sound_speech.enabled));
    buffer->skip(data6);
    buffer->write_i32(static_cast<int32_t>(m_game_speed));
    buffer->write_i32(static_cast<int32_t>(m_scroll_speed));
    buffer->write_raw(m_player_name.data(), m_player_name.size());
    buffer->skip(data16);
    buffer->write_i32(static_cast<int32_t>(m_last_advisor));
    buffer->skip(data_save_game_mission_id); 
    buffer->write_i32(static_cast<int32_t>(m_tooltips));
    buffer->skip(data_starting_kingdom); 
    buffer->skip(data_personal_savings_last_mission); 
    buffer->skip(data_current_mission_id);
    buffer->skip(data_is_custom_scenario); 
    buffer->write_u8(static_cast<uint8_t>(m_sound_city.enabled));
    buffer->write_u8(static_cast<uint8_t>(m_warnings));
    buffer->write_u8(static_cast<uint8_t>(m_monthly_autosave));
    buffer->skip(data_autoclear); 
    buffer->write_i32(static_cast<int32_t>(m_sound_effects.volume));
    buffer->write_i32(static_cast<int32_t>(m_sound_music.volume));
    buffer->write_i32(static_cast<int32_t>(m_sound_speech.volume));
    buffer->write_i32(static_cast<int32_t>(m_sound_city.volume));
    buffer->skip(data_ram); 
    buffer->write_i32(static_cast<int32_t>(m_window_width));
    buffer->write_i32(static_cast<int32_t>(m_window_height));
    buffer->skip(max_confirmed_resolution); 

    for (std::size_t i = 0; i < m_personal_savings.size(); i++) {
        buffer->write_i32(static_cast<int32_t>(m_personal_savings[i]));
    }

    buffer->write_i32(static_cast<int32_t>(m_victory_video));
    buffer->write_i32(static_cast<int32_t>(m_difficulty));
    buffer->write_i32(static_cast<int32_t>(m_gods_enabled));

    io_write_buffer_to_file(SETTINGS_FILENAME.data(), buffer.get(), INF_SIZE);    
}

bool Settings::fullscreen() const
{
    return m_fullscreen;
}

void Settings::toggle_fullscreen()
{
    m_fullscreen = !m_fullscreen;
}

int Settings::window_width() const
{
    return m_window_width;
}

int Settings::window_height() const
{
    return m_window_height;
}

void Settings::set_display(bool fullscreen, int window_width, int window_height) 
{
    m_fullscreen = fullscreen;
    if (!fullscreen) {
        m_window_width = window_width;
        m_window_height = window_height;
    }
}

SettingSound Settings::sound(SoundType sound_type) const
{
    return *_sound(sound_type);
}

void Settings::toggle_sound_enabled(SoundType sound_type)
{
    auto sound = _sound(sound_type);
    sound->enabled = !sound->enabled;
}

void Settings::increase_sound_volume(SoundType sound_type) 
{
  auto sound = _sound(sound_type);
  sound->volume = std::clamp(sound->volume + 1, 0, 100);
}

void Settings::decrease_sound_volume(SoundType sound_type) 
{
  auto sound = _sound(sound_type);
  sound->volume = std::clamp(sound->volume - 1, 0, 100);
}

void Settings::reset_sound(SoundType sound_type, const SettingSound& set_sound)
{
    auto sound = _sound(sound_type);
    *sound =  set_sound;
}

int Settings::game_speed() const 
{ 
    return m_game_speed; 
}

void Settings::increase_game_speed() 
{
    // TODO why can this be ever > 100. Why not only the else part?
    if (m_game_speed >= 100) { 
        if (m_game_speed < 1000) {
        m_game_speed += 100;
        }
    } 
    else {
        m_game_speed = std::clamp(m_game_speed + 10, 10, 100);
    }
}

void Settings::decrease_game_speed() 
{
    // TODO why can this be ever > 100. Why not only the else part?
    if (m_game_speed > 100) {
        m_game_speed -= 100;
    }
    else {
        m_game_speed = std::clamp(m_game_speed - 10, 10, 100);
    }
}

int Settings::scroll_speed() const 
{
    return m_scroll_speed;
}

void Settings::increase_scroll_speed()
{
    m_scroll_speed = std::clamp(m_scroll_speed + 10, 0, 100);
}

void Settings::decrease_scroll_speed()
{
    m_scroll_speed = std::clamp(m_scroll_speed - 10, 0, 100);
}

void Settings::reset_speeds(int game_speed, int scroll_speed)
{
    m_game_speed = std::clamp(game_speed, 0, 100);
    m_scroll_speed = std::clamp(scroll_speed, 0, 100);
}

Tooltips Settings::tooltips() const
{
    return m_tooltips;
}

void Settings::cycle_tooltips()
{
    if(m_tooltips == Tooltips::FULL) {
        m_tooltips = Tooltips::NONE;
    }
    else {
        auto enum_index = static_cast<int>(m_tooltips);
        ++enum_index;
        m_tooltips = static_cast<Tooltips>(enum_index);
    }
}

bool Settings::warnings() const
{
    return m_warnings;
}

void Settings::toggle_warnings()
{
    m_warnings = !m_warnings;
}

bool Settings::monthly_autosave() const
{
    return m_monthly_autosave;
}

void Settings::toggle_monthly_autosave()
{
    m_monthly_autosave = !m_monthly_autosave;
}

CitiesNamesStyle Settings::city_names_style() const
{
    return m_city_names_style;
}

void Settings::cycle_city_names_style()
{
    if(m_city_names_style == CitiesNamesStyle::OLD_NAMES) {
        m_city_names_style = CitiesNamesStyle::NEW_NAMES;
    }
    else { // m_city_names_style == CitiesNamesStyle::NEW_NAMES
        m_city_names_style = CitiesNamesStyle::OLD_NAMES;
    }
}

bool Settings::pyramid_speedup() const
{
    return m_pyramid_speedup;
}

void Settings::toggle_pyramid_speedup()
{
    m_pyramid_speedup = !m_pyramid_speedup;
}

int Settings::popup_messages() const
{ 
    return m_popup_messages; 
}

void Settings::toggle_popup_messages(int flag) 
{ 
    m_popup_messages ^= flag; 
}

bool Settings::gods_enabled() const
{
    return m_gods_enabled;
}

void Settings::toggle_gods_enabled()
{
    m_gods_enabled = !m_gods_enabled;
}

Difficulty Settings::difficulty() const
{
    return m_difficulty;
}

void Settings::increase_difficulty()
{
    if(m_difficulty == Difficulty::HARD) {
        return;
    }
    auto enum_index = static_cast<int>(m_difficulty);
    ++enum_index;
    m_difficulty = static_cast<Difficulty>(enum_index);
}

void Settings::decrease_difficulty()
{
    if(m_difficulty == Difficulty::VERY_EASY) {
        return;
    }
    auto enum_index = static_cast<int>(m_difficulty);
    --enum_index;
    m_difficulty = static_cast<Difficulty>(enum_index); 
}

bool Settings::victory_video() const
{
    return m_victory_video;
}

Advisor Settings::last_advisor() const
{
    return m_last_advisor;
}

void Settings::set_last_advisor(Advisor advisor)
{
    m_last_advisor = advisor;
}

const uint8_t *Settings::player_name() const
{
    return m_player_name.data();
}

const char *Settings::player_name_utf8() const
{
    return m_player_name_utf8.data();
}

void Settings::set_player_name(const uint8_t *player_name)
{
    string_copy(player_name, m_player_name.data(), m_player_name.size());
    encoding_to_utf8(player_name, m_player_name_utf8.data(), m_player_name_utf8.size(), 0);   
}

int Settings::personal_savings_for_mission(int mission_id) const
{
    return m_personal_savings.at(mission_id);
}

void Settings::set_personal_savings_for_mission(int mission_id, int savings) 
{
    m_personal_savings.at(mission_id) = savings;
}

void Settings::clear_personal_savings(void) {
    for(auto& personal_saving: m_personal_savings) {
        personal_saving = 0;
    }
}

void Settings::_load_default_settings()
{
    m_fullscreen = true;
    m_window_width = 800;
    m_window_height = 600;

    m_sound_effects = SettingSound{};
    m_sound_music = SettingSound{true, 80};
    m_sound_speech = SettingSound{};
    m_sound_city = SettingSound{};

    m_game_speed = 90;
    m_scroll_speed = 70;

    m_difficulty = Difficulty::HARD;
    m_tooltips = Tooltips::FULL;
    m_warnings = true;
    m_gods_enabled = true;
    m_victory_video = false;
    m_last_advisor = Advisor::LABOR;

    m_popup_messages = 0;
    m_city_names_style = CitiesNamesStyle::OLD_NAMES;
    m_pyramid_speedup = false;

    for(auto& personal_saving : m_personal_savings) {
        personal_saving = 0;
    }
}

void Settings::_load_settings(buffer *buffer) 
{
    constexpr int data1 = 1;
    constexpr int data3 = 3;
    constexpr int data4 = 4;
    constexpr int data6 = 6;
    constexpr int data8 = 8;
    constexpr int data16 = 16;

    constexpr int data_save_game_mission_id = data4;
    constexpr int data_starting_kingdom = data4;
    constexpr int data_personal_savings_last_mission = data4;
    constexpr int data_current_mission_id = data4;
    constexpr int data_is_custom_scenario = data4;
    constexpr int data_autoclear = data1;

    constexpr int data_ram = data8;
    constexpr int max_confirmed_resolution = data8;

    buffer->skip(data4);

    m_fullscreen = static_cast<bool>(buffer->read_i32());

    buffer->skip(data3);

    m_sound_effects.enabled = static_cast<bool>(buffer->read_u8());
    m_sound_music.enabled = static_cast<bool>(buffer->read_u8());
    m_sound_speech.enabled = static_cast<bool>(buffer->read_u8());

    buffer->skip(data6);

    m_game_speed = static_cast<int>(buffer->read_i32());
    m_game_speed = 80; // todo: fix settings
    m_scroll_speed = static_cast<int>(buffer->read_i32());

    buffer->read_raw(m_player_name.data(), m_player_name.size());

    buffer->skip(data16);

    m_last_advisor =  static_cast<Advisor>(buffer->read_i32());
    // TODO: remove this ?
    m_last_advisor = Advisor::TRADE; // debug

    buffer->skip(data_save_game_mission_id); 

    m_tooltips = static_cast<Tooltips>(buffer->read_i32());

    buffer->skip(data_starting_kingdom); 
    buffer->skip(data_personal_savings_last_mission); 
    buffer->skip(data_current_mission_id); 
    buffer->skip(data_is_custom_scenario);

    m_sound_city.enabled = static_cast<bool>(buffer->read_u8());
    m_warnings = static_cast<bool>(buffer->read_u8());
    m_monthly_autosave = static_cast<bool>(buffer->read_u8());

    buffer->skip(data_autoclear);

    m_sound_effects.volume = static_cast<int>(buffer->read_i32());
    m_sound_music.volume = static_cast<int>(buffer->read_i32());
    m_sound_speech.volume = static_cast<int>(buffer->read_i32());
    m_sound_city.volume = static_cast<int>(buffer->read_i32());

    buffer->skip(data_ram);

    m_window_width = static_cast<int>(buffer->read_i32());
    m_window_height = static_cast<int>(buffer->read_i32());

    buffer->skip(max_confirmed_resolution);

    for (std::size_t i = 0; i < m_personal_savings.size(); i++) {
      m_personal_savings[i] = static_cast<int>(buffer->read_i32());
    }
    m_victory_video = buffer->read_i32();

    // TODO is this switch valid for pharaoh since it mentions c3?
    if (buffer->at_end()) {
      // Settings file is from unpatched C3, use default values
      m_difficulty = Difficulty::HARD;
      m_gods_enabled = true;
    } 
    else {
      m_difficulty = static_cast<Difficulty>(buffer->read_i32());
      m_gods_enabled = static_cast<bool>(buffer->read_i32());
    }    
}

const SettingSound* Settings::_sound(SoundType sound_type) const
{
    switch (sound_type) {
    case SoundType::MUSIC:
        return &m_sound_music;
    case SoundType::EFFECTS:
        return &m_sound_effects;
    case SoundType::SPEECH:
        return &m_sound_speech;
    case SoundType::CITY:
        return &m_sound_city;
    default:
        return &m_sound_music;
    }
    return &m_sound_music;
}

SettingSound* Settings::_sound(SoundType sound_type)
{
    switch (sound_type) {
    case SoundType::MUSIC:
        return &m_sound_music;
    case SoundType::EFFECTS:
        return &m_sound_effects;
    case SoundType::SPEECH:
        return &m_sound_speech;
    case SoundType::CITY:
        return &m_sound_city;
    default:
        return &m_sound_music;
    }
    return &m_sound_music;
}