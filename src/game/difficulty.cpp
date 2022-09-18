#include "difficulty.h"

#include "core/calc.h"
#include "game/settings.h"

static const struct {
    int money;
    int enemies;
    int starting_kingdom;
    int sentiment;
} data[] = {
        {300, 40,  70, 80}, // very easy
        {200, 60,  60, 70}, // easy
        {150, 80,  50, 60}, // normal
        {100, 100, 50, 50}, // hard
        {75,  120, 40, 40} // very hard
};

int difficulty_starting_kingdom(void) {
    auto& settings = Settings::instance();
    return data[static_cast<int>(settings.difficulty())].starting_kingdom;
}

int difficulty_sentiment(void) {
    auto& settings = Settings::instance();
    return data[static_cast<int>(settings.difficulty())].sentiment;
}

int difficulty_adjust_money(int money) {
    auto& settings = Settings::instance();
    return calc_adjust_with_percentage(money, data[static_cast<int>(settings.difficulty())].money);
}

int difficulty_adjust_enemies(int enemies) {
    auto& settings = Settings::instance();
    return calc_adjust_with_percentage(enemies, data[static_cast<int>(settings.difficulty())].enemies);
}

int difficulty_adjust_wolf_attack(int attack) {
    auto& settings = Settings::instance();
    switch (settings.difficulty()) {
        case Difficulty::VERY_EASY:
            return 2;
        case Difficulty::EASY:
            return 4;
        case Difficulty::NORMAL:
            return 6;
        default:
            return attack;
    }
    return attack;
}
