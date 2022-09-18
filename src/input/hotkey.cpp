#include "hotkey.h"

#include "building/type.h"
#include "city/constants.h"
#include "game/settings.h"
#include "game/state.h"
#include "game/system.h"
#include "graphics/screenshot.h"
#include "graphics/video.h"
#include "graphics/window.h"
#include "input/scroll.h"
#include "window/hotkey_editor.h"
#include "window/popup_dialog.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    int *action;
    int value;
    int key;
    int modifiers;
    int repeatable;
} hotkey_definition;

typedef struct {
    void (*action)(int is_down);
    int key;
} arrow_definition;

typedef struct {
    int center_screen;
    int toggle_fullscreen;
    int resize_to;
    int save_screenshot;
    int save_city_screenshot;
} global_hotkeys;

static struct {
    global_hotkeys global_hotkey_state;
    hotkeys hotkey_state;

    hotkey_definition *definitions;
    int num_definitions;
    arrow_definition *arrows;
    int num_arrows;
} data;

static void add_definition(const hotkey_mapping *mapping) {
    hotkey_definition *hotkey_definition = &data.definitions[data.num_definitions];
    hotkey_definition->key = mapping->key;
    hotkey_definition->modifiers = mapping->modifiers;
    hotkey_definition->value = 1;
    hotkey_definition->repeatable = 0;
    switch (mapping->action) {
        case HOTKEY_TOGGLE_PAUSE:
            hotkey_definition->action = &data.hotkey_state.toggle_pause;
            break;
        case HOTKEY_TOGGLE_OVERLAY:
            hotkey_definition->action = &data.hotkey_state.toggle_overlay;
            break;
        case HOTKEY_CYCLE_LEGION:
            hotkey_definition->action = &data.hotkey_state.cycle_legion;
            break;
        case HOTKEY_INCREASE_GAME_SPEED:
            hotkey_definition->action = &data.hotkey_state.increase_game_speed;
            hotkey_definition->repeatable = 1;
            break;
        case HOTKEY_DECREASE_GAME_SPEED:
            hotkey_definition->action = &data.hotkey_state.decrease_game_speed;
            hotkey_definition->repeatable = 1;
            break;
        case HOTKEY_ROTATE_MAP_LEFT:
            hotkey_definition->action = &data.hotkey_state.rotate_map_left;
            break;
        case HOTKEY_ROTATE_MAP_RIGHT:
            hotkey_definition->action = &data.hotkey_state.rotate_map_right;
            break;
        case HOTKEY_SHOW_ADVISOR_LABOR:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::LABOR);
            break;
        case HOTKEY_SHOW_ADVISOR_MILITARY:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::MILITARY);
            break;
        case HOTKEY_SHOW_ADVISOR_IMPERIAL:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::IMPERIAL);
            break;
        case HOTKEY_SHOW_ADVISOR_RATINGS:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::RATINGS);
            break;
        case HOTKEY_SHOW_ADVISOR_TRADE:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::TRADE);
            break;
        case HOTKEY_SHOW_ADVISOR_POPULATION:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::POPULATION);
            break;
        case HOTKEY_SHOW_ADVISOR_HEALTH:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::HEALTH);
            break;
        case HOTKEY_SHOW_ADVISOR_EDUCATION:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::EDUCATION);
            break;
        case HOTKEY_SHOW_ADVISOR_ENTERTAINMENT:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::ENTERTAINMENT);
            break;
        case HOTKEY_SHOW_ADVISOR_RELIGION:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::RELIGION);
            break;
        case HOTKEY_SHOW_ADVISOR_FINANCIAL:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::FINANCIAL);
            break;
        case HOTKEY_SHOW_ADVISOR_CHIEF:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::CHIEF);
            break;
        case HOTKEY_SHOW_ADVISOR_HOUSING:
            hotkey_definition->action = &data.hotkey_state.show_advisor;
            hotkey_definition->value = static_cast<int>(Advisor::HOUSING);
            break;
        case HOTKEY_SHOW_OVERLAY_WATER:
            hotkey_definition->action = &data.hotkey_state.show_overlay;
            hotkey_definition->value = OVERLAY_WATER;
            break;
        case HOTKEY_SHOW_OVERLAY_FIRE:
            hotkey_definition->action = &data.hotkey_state.show_overlay;
            hotkey_definition->value = OVERLAY_FIRE;
            break;
        case HOTKEY_SHOW_OVERLAY_DAMAGE:
            hotkey_definition->action = &data.hotkey_state.show_overlay;
            hotkey_definition->value = OVERLAY_DAMAGE;
            break;
        case HOTKEY_SHOW_OVERLAY_CRIME:
            hotkey_definition->action = &data.hotkey_state.show_overlay;
            hotkey_definition->value = OVERLAY_CRIME;
            break;
        case HOTKEY_SHOW_OVERLAY_PROBLEMS:
            hotkey_definition->action = &data.hotkey_state.show_overlay;
            hotkey_definition->value = OVERLAY_PROBLEMS;
            break;
        case HOTKEY_EDITOR_TOGGLE_BATTLE_INFO:
            hotkey_definition->action = &data.hotkey_state.toggle_editor_battle_info;
            break;
        case HOTKEY_LOAD_FILE:
            hotkey_definition->action = &data.hotkey_state.load_file;
            break;
        case HOTKEY_SAVE_FILE:
            hotkey_definition->action = &data.hotkey_state.save_file;
            break;
        case HOTKEY_ROTATE_BUILDING:
            hotkey_definition->action = &data.hotkey_state.rotate_building;
            break;
        case HOTKEY_CHANGE_BUILDING_VARIANT:
            hotkey_definition->action = &data.hotkey_state.change_building_variant;
            break;
        case HOTKEY_GO_TO_BOOKMARK_1:
            hotkey_definition->action = &data.hotkey_state.go_to_bookmark;
            hotkey_definition->value = 1;
            break;
        case HOTKEY_GO_TO_BOOKMARK_2:
            hotkey_definition->action = &data.hotkey_state.go_to_bookmark;
            hotkey_definition->value = 2;
            break;
        case HOTKEY_GO_TO_BOOKMARK_3:
            hotkey_definition->action = &data.hotkey_state.go_to_bookmark;
            hotkey_definition->value = 3;
            break;
        case HOTKEY_GO_TO_BOOKMARK_4:
            hotkey_definition->action = &data.hotkey_state.go_to_bookmark;
            hotkey_definition->value = 4;
            break;
        case HOTKEY_SET_BOOKMARK_1:
            hotkey_definition->action = &data.hotkey_state.set_bookmark;
            hotkey_definition->value = 1;
            break;
        case HOTKEY_SET_BOOKMARK_2:
            hotkey_definition->action = &data.hotkey_state.set_bookmark;
            hotkey_definition->value = 2;
            break;
        case HOTKEY_SET_BOOKMARK_3:
            hotkey_definition->action = &data.hotkey_state.set_bookmark;
            hotkey_definition->value = 3;
            break;
        case HOTKEY_SET_BOOKMARK_4:
            hotkey_definition->action = &data.hotkey_state.set_bookmark;
            hotkey_definition->value = 4;
            break;
        case HOTKEY_CENTER_WINDOW:
            hotkey_definition->action = &data.global_hotkey_state.center_screen;
            break;
        case HOTKEY_TOGGLE_FULLSCREEN:
            hotkey_definition->action = &data.global_hotkey_state.toggle_fullscreen;
            break;
        case HOTKEY_RESIZE_TO_640:
            hotkey_definition->action = &data.global_hotkey_state.resize_to;
            hotkey_definition->value = 640;
            break;
        case HOTKEY_RESIZE_TO_800:
            hotkey_definition->action = &data.global_hotkey_state.resize_to;
            hotkey_definition->value = 800;
            break;
        case HOTKEY_RESIZE_TO_1024:
            hotkey_definition->action = &data.global_hotkey_state.resize_to;
            hotkey_definition->value = 1024;
            break;
        case HOTKEY_SAVE_SCREENSHOT:
            hotkey_definition->action = &data.global_hotkey_state.save_screenshot;
            break;
        case HOTKEY_SAVE_CITY_SCREENSHOT:
            hotkey_definition->action = &data.global_hotkey_state.save_city_screenshot;
            break;
        case HOTKEY_BUILD_VACANT_HOUSE:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_HOUSE_VACANT_LOT;
            break;
        case HOTKEY_BUILD_CLEAR_LAND:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_CLEAR_LAND;
            break;
        case HOTKEY_BUILD_ROAD:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_ROAD;
            break;
        case HOTKEY_BUILD_ENGINEERS_POST:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_ENGINEERS_POST;
            break;
        case HOTKEY_BUILD_WALL:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_WALL;
            break;
        case HOTKEY_BUILD_GATEHOUSE:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_GATEHOUSE;
            break;
        case HOTKEY_BUILD_PREFECTURE:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_POLICE_STATION;
            break;
        case HOTKEY_BUILD_GRANARY:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_GRANARY;
            break;
        case HOTKEY_BUILD_WAREHOUSE:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_WAREHOUSE;
            break;
        case HOTKEY_BUILD_MARKET:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_MARKET;
            break;
        case HOTKEY_BUILD_PLAZA:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_PLAZA;
            break;
        case HOTKEY_BUILD_GARDENS:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_GARDENS;
            break;
        case HOTKEY_BUILD_RESERVOIR:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_WATER_LIFT;
            break;
        case HOTKEY_BUILD_AQUEDUCT:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_IRRIGATION_DITCH;
            break;
        case HOTKEY_BUILD_FOUNTAIN:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_MENU_BEAUTIFICATION;
            break;
        case HOTKEY_BUILD_DOCTOR:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_APOTHECARY;
            break;
        case HOTKEY_BUILD_ROADBLOCK:
            hotkey_definition->action = &data.hotkey_state.building;
            hotkey_definition->value = BUILDING_ROADBLOCK;
            break;
        case HOTKEY_DEBUG_1_UP:
            hotkey_definition->action = &data.hotkey_state.debug_1_up;
            hotkey_definition->repeatable = 1;
            break;
        case HOTKEY_DEBUG_1_DOWN:
            hotkey_definition->action = &data.hotkey_state.debug_1_down;
            hotkey_definition->repeatable = 1;
            break;
        case HOTKEY_DEBUG_2_UP:
            hotkey_definition->action = &data.hotkey_state.debug_2_up;
            hotkey_definition->repeatable = 1;
            break;
        case HOTKEY_DEBUG_2_DOWN:
            hotkey_definition->action = &data.hotkey_state.debug_2_down;
            hotkey_definition->repeatable = 1;
            break;
        default:
            hotkey_definition->action = 0;
    }
    if (hotkey_definition->action) {
        data.num_definitions++;
    }
}

static void add_arrow(const hotkey_mapping *mapping) {
    arrow_definition *arrow = &data.arrows[data.num_arrows];
    arrow->key = mapping->key;
    switch (mapping->action) {
        case HOTKEY_ARROW_UP:
            arrow->action = scroll_arrow_up;
            break;
        case HOTKEY_ARROW_DOWN:
            arrow->action = scroll_arrow_down;
            break;
        case HOTKEY_ARROW_LEFT:
            arrow->action = scroll_arrow_left;
            break;
        case HOTKEY_ARROW_RIGHT:
            arrow->action = scroll_arrow_right;
            break;
        default:
            arrow->action = 0;
            break;
    }
    if (arrow->action)
        data.num_arrows++;

}

static int allocate_mapping_memory(int total_definitions, int total_arrows) {
    free(data.definitions);
    free(data.arrows);
    data.num_definitions = 0;
    data.num_arrows = 0;
    data.definitions = (hotkey_definition *) malloc(sizeof(hotkey_definition) * total_definitions);
    data.arrows = (arrow_definition *) malloc(sizeof(arrow_definition) * total_arrows);
    if (!data.definitions || !data.arrows) {
        free(data.definitions);
        free(data.arrows);
        return 0;
    }
    return 1;
}

void hotkey_install_mapping(hotkey_mapping *mappings, int num_mappings) {
    int total_definitions = 2; // Enter and ESC are fixed hotkeys
    int total_arrows = 0;
    for (int i = 0; i < num_mappings; i++) {
        int action = mappings[i].action;
        if (action == HOTKEY_ARROW_UP || action == HOTKEY_ARROW_DOWN ||
            action == HOTKEY_ARROW_LEFT || action == HOTKEY_ARROW_RIGHT) {
            total_arrows++;
        } else
            total_definitions++;
    }
    if (!allocate_mapping_memory(total_definitions, total_arrows))
        return;

    // Fixed keys: Escape and Enter
    data.definitions[0].action = &data.hotkey_state.enter_pressed;
    data.definitions[0].key = KEY_ENTER;
    data.definitions[0].modifiers = 0;
    data.definitions[0].repeatable = 0;
    data.definitions[0].value = 1;

    data.definitions[1].action = &data.hotkey_state.escape_pressed;
    data.definitions[1].key = KEY_ESCAPE;
    data.definitions[1].modifiers = 0;
    data.definitions[1].repeatable = 0;
    data.definitions[1].value = 1;

    data.num_definitions = 2;

    for (int i = 0; i < num_mappings; i++) {
        int action = mappings[i].action;
        if (action == HOTKEY_ARROW_UP || action == HOTKEY_ARROW_DOWN ||
            action == HOTKEY_ARROW_LEFT || action == HOTKEY_ARROW_RIGHT) {
            add_arrow(&mappings[i]);
        } else
            add_definition(&mappings[i]);
    }
}

const hotkeys *hotkey_state(void) {
    return &data.hotkey_state;
}

void hotkey_reset_state(void) {
    memset(&data.hotkey_state, 0, sizeof(data.hotkey_state));
    memset(&data.global_hotkey_state, 0, sizeof(data.global_hotkey_state));
}

void hotkey_key_pressed(int key, int modifiers, int repeat) {
    if (window_is(WINDOW_HOTKEY_EDITOR)) {
        window_hotkey_editor_key_pressed(key, modifiers);
        return;
    }
    if (key == KEY_NONE)
        return;
    for (int i = 0; i < data.num_arrows; i++) {
        arrow_definition *arrow = &data.arrows[i];
        if (arrow->key == key)
            arrow->action(1);
    }
    for (int i = 0; i < data.num_definitions; i++) {
        hotkey_definition *def = &data.definitions[i];
        if (def->key == key && def->modifiers == modifiers && (!repeat || def->repeatable))
            *(def->action) = def->value;
    }
}

void hotkey_key_released(int key, int modifiers) {
    if (window_is(WINDOW_HOTKEY_EDITOR)) {
        window_hotkey_editor_key_released(key, modifiers);
        return;
    }
    if (key == KEY_NONE)
        return;
    for (int i = 0; i < data.num_arrows; i++) {
        arrow_definition *arrow = &data.arrows[i];
        if (arrow->key == key)
            arrow->action(0);

    }
}

static void confirm_exit(bool accepted) {
    if (accepted)
        system_exit();

}

void hotkey_handle_escape(void) {
    video_stop();
    window_popup_dialog_show(POPUP_DIALOG_QUIT, confirm_exit, 1);
}

void hotkey_handle_global_keys(void) {
    if (data.global_hotkey_state.center_screen)
        system_center();

    if (data.global_hotkey_state.resize_to) {
        switch (data.global_hotkey_state.resize_to) {
            case 640:
                system_resize(640, 480);
                break;
            case 800:
                system_resize(800, 600);
                break;
            case 1024:
                system_resize(1024, 768);
                break;
        }
    }
    if (data.global_hotkey_state.toggle_fullscreen) {
        auto& settings = Settings::instance();
        settings.toggle_fullscreen();
    }

//    if (data.global_hotkey_state.save_screenshot) {
//        graphics_save_screenshot(0);
//    }
//    if (data.global_hotkey_state.save_city_screenshot) {
//        graphics_save_screenshot(1);
//    }
}
