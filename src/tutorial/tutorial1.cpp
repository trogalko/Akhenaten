#include "game/tutorial.h"

#include "city/city.h"
#include "game/game_events.h"
#include "game/game.h"
#include "city/city_building_menu_ctrl.h"
#include "building/building_granary.h"
#include "io/gamefiles/lang.h"
#include "city/city_message.h"

struct tutorial_1 : public tutorial_t {
    virtual int missionid() const override { return 1; }
    virtual void init() override;
    virtual void reset() override;
    virtual void update_step(xstring s) override;
    virtual xstring goal_text() override;
};

tutorial_1 g_tutorial_1;

void tutorial_handle_advance_day(event_advance_day ev) {
    auto &tut = g_tutorials_flags.tutorial_1;

    if (!tut.building_burned) {
        const auto found = std::max_element(city_buildings().begin(), city_buildings().end(), [] (const building& a, const building& b) { return a.fire_risk < b.fire_risk; });
        if (found != city_buildings().end()) {
            building_id max_collapse_bid = found->id;
            buildings_valid_do([max_collapse_bid] (building& b) { 
                if (b.id != max_collapse_bid) 
                    b.fire_risk = 0;
                    b.damage_risk = 0;
            });
        }
    }

    if (tut.building_burned && !tut.building_collapsed) {
        const auto found = std::max_element(city_buildings().begin(), city_buildings().end(), [] (const building& a, const building& b) { return a.damage_risk < b.damage_risk; });
        if (found != city_buildings().end()) {
            building_id max_collapse_bid = found->id;
            buildings_valid_do([max_collapse_bid] (building& b) {
                if (b.id != max_collapse_bid)
                    b.damage_risk = 0;
            });
        }
    }
}

void tutorial1_handle_fire(event_fire_damage) {
    auto &tut = g_tutorials_flags.tutorial_1;

    if (tut.building_burned) {
        return;
    }

    events::unsubscribe(&tutorial1_handle_fire);
    g_tutorials_flags.pharaoh.last_action = game.simtime.absolute_day(true);
    tut.building_burned = true;
    
    for (auto &item: g_scenario.extra_damage) {
        item.fire = 0;
    }

    events::emit(event_building_menu_update{ tutorial_stage.tutorial_fire });
    messages::popup(MESSAGE_TUTORIAL_FIRE_IN_THE_VILLAGE, 0, 0);
}

void tutorial1_popultion_cap(city_migration_t& migration) {
    auto &tut = g_tutorials_flags.tutorial_1;

    const int population_cap_firstfire = g_scenario.vars.get_int("population_cap_firstfire", 80);
    const int max_pop = (!tut.building_burned || !tut.building_collapsed) ? population_cap_firstfire : 0;
    migration.population_cap = max_pop;
}

void tutorial1_handle_population_for_granary(event_population_changed ev) {
    auto &tut = g_tutorials_flags.tutorial_1;

    if (tut.granary_opened) {
        return;
    }

    if (ev.value < g_scenario.vars.get_int("granary_open_population", 150)) {
        return;
    }

    events::unsubscribe(&tutorial1_handle_population_for_granary);
    events::emit(event_building_menu_update{ "tutorial_food" });
    g_tutorials_flags.pharaoh.last_action = game.simtime.absolute_day(true);
    tut.granary_opened = true;
    messages::popup(MESSAGE_TUTORIAL_FOOD_OR_FAMINE, 0, 0);
}

void tutorial1_handle_collapse(event_collase_damage) {
    auto &tut = g_tutorials_flags.tutorial_1;

    if (tut.building_collapsed) {
        return;
    }

    for (auto& item : g_scenario.extra_damage) {
        item.collapse = 0;
    }

    events::unsubscribe(&tutorial1_handle_collapse);
    events::emit(event_building_menu_update{ tutorial_stage.tutorial_collapse });
    g_tutorials_flags.pharaoh.last_action = game.simtime.absolute_day(true);
    tut.building_collapsed = true;
    messages::popup(MESSAGE_TUTORIAL_COLLAPSED_BUILDING, 0, 0);
}

void tutorial1_on_filled_granary(event_granary_resource_added ev) {
    auto &tut = g_tutorials_flags.tutorial_1;

    if (tut.gamemeat_stored) {
        return;
    }

    if (ev.amount <= g_scenario.vars.get_int("granary_meat_stored", 400)) {
        return;
    }

    events::unsubscribe(&tutorial1_on_filled_granary);
    events::emit(event_building_menu_update{ tutorial_stage.tutorial_water });
    g_tutorials_flags.pharaoh.last_action = game.simtime.absolute_day(true);
    tut.gamemeat_stored = true;
    messages::popup(MESSAGE_TUTORIAL_CLEAN_WATER, 0, 0);
}

void tutorial1_handle_building_create(event_building_create ev) {
    auto &tut = g_tutorials_flags.tutorial_1;
    if (tut.architector_built) {
        return;
    }

    events::unsubscribe(&tutorial1_handle_building_create);
    g_tutorials_flags.pharaoh.last_action = game.simtime.absolute_day(true);
}

bool tutorial1_is_success() {
    auto &tut = g_tutorials_flags.tutorial_1;
    const bool may_finish = (tut.building_burned && tut.building_collapsed && tut.granary_opened);
    const int victory_last_action_delay = g_scenario.vars.get_int("victory_last_action_delay", 3);
    const bool some_days_after_last_action = (game.simtime.absolute_day(true) - g_tutorials_flags.pharaoh.last_action) > victory_last_action_delay;
    return may_finish && some_days_after_last_action;
}

void tutorial_1::init() {
    auto &tut = g_tutorials_flags.tutorial_1;

    const bool building_burned = tut.building_burned;
    events::emit_if(building_burned, event_building_menu_update{ tutorial_stage.tutorial_fire });

    events::subscribe_if(!building_burned, &tutorial1_handle_fire);

    const bool granary_opened = tut.granary_opened;
    events::emit_if(granary_opened, event_building_menu_update{ "tutorial_food" });
    
    events::subscribe_if(!granary_opened, &tutorial1_handle_population_for_granary);

    const bool architector_built = tut.architector_built;
    events::subscribe_if(!architector_built, &tutorial1_handle_building_create);


    const bool building_collapsed = tut.building_collapsed;
    events::emit_if(building_collapsed, event_building_menu_update{ tutorial_stage.tutorial_collapse });

    events::subscribe_if(!building_collapsed, &tutorial1_handle_collapse);

    const bool gamemeat_stored = tut.gamemeat_stored;
    events::emit_if(gamemeat_stored, event_building_menu_update{ tutorial_stage.tutorial_water });
    
    events::subscribe_if(!gamemeat_stored, &tutorial1_on_filled_granary);

    g_city.victory_state.add_condition(tutorial1_is_success);
    g_city.migration.add_condition(tutorial1_popultion_cap);

    events::subscribe(&tutorial_handle_advance_day);
}

void tutorial_1::reset() {
    auto &tut = g_tutorials_flags.tutorial_1;

    tut.building_burned = 0;
    tut.granary_opened = 0;
    tut.gamemeat_stored = 0;
    tut.building_collapsed = 0;
    tut.started = 0;
}

xstring tutorial_1::goal_text() {
    auto &tut = g_tutorials_flags.tutorial_1;

    if (!tut.granary_opened)
        return lang_get_xstring(62, 21);
    
    if (!tut.gamemeat_stored)
        return lang_get_xstring(62, 19);

    return lang_get_xstring(62, 20);
}

void tutorial_1::update_step(xstring s) {
    auto &tut = g_tutorials_flags.tutorial_1;

    if (s == tutorial_stage.tutorial_fire) {
        tut.building_burned = false;
        tutorial1_handle_fire({ 0 });
    } else if (s == tutorial_stage.tutorial_collapse) {
        tut.building_collapsed = false;
        tutorial1_handle_collapse({ 0 });
    } else if (s == "tutorial_food") {
        tut.granary_opened = false;
        tutorial1_handle_population_for_granary({ 0 });
    } else if (s == tutorial_stage.tutorial_water) {
        tut.gamemeat_stored = false;
        tutorial1_on_filled_granary({ 0 });
    }
}
