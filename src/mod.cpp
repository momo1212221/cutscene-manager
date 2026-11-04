#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_set>
#include "openrac/mod.hpp"
#include "logic.hpp"

namespace {
constexpr std::uint64_t kNone = ~std::uint64_t{0};

std::uint64_t scene_key(int level, int scene) {
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(level)) << 32)
           | static_cast<std::uint32_t>(scene);
}
}

void load(openrac::Mod& mod) {
    if (mod.game() != "rac1" || !mod.has_scenes())
        throw openrac::Error("Cutscene Manager requires RAC1 interface 1.9+");

    const int mode = std::clamp(static_cast<int>(mod.setting<std::int64_t>("mode")), 0, 1);
    const int minimum = static_cast<int>(mod.setting<std::int64_t>("min_frames"));

    auto safe = std::make_shared<std::unordered_set<std::uint64_t>>();
    auto unsafe = std::make_shared<std::unordered_set<std::uint64_t>>();
    auto active = std::make_shared<std::uint64_t>(kNone);
    auto active_level = std::make_shared<int>(-1);
    auto active_loads = std::make_shared<std::uint32_t>(0);
    auto skipped = std::make_shared<bool>(false);
    auto candidate = std::make_shared<std::uint64_t>(kNone);
    auto candidate_level = std::make_shared<int>(-1);
    auto candidate_loads = std::make_shared<std::uint32_t>(0);
    auto grace = std::make_shared<int>(0);

    mod.on_frame([&mod, mode, minimum, safe, unsafe, active, active_level, active_loads, skipped,
                  candidate, candidate_level, candidate_loads, grace] {
        const auto status = mod.play_status();

        // A naturally completed scene is only declared safe after a grace window in which no
        // level/load transition follows. Level-ending/progression scenes therefore become unsafe.
        if (*grace > 0) {
            if (status.level != *candidate_level || status.loads != *candidate_loads) {
                unsafe->insert(*candidate);
                safe->erase(*candidate);
                *candidate = kNone;
                *grace = 0;
            } else if (--*grace == 0 && *candidate != kNone) {
                safe->insert(*candidate);
                *candidate = kNone;
            }
        }

        int frame = 0;
        const int scene = mod.scene_playing(&frame);
        if (scene < 0 || status.level < 0) {
            if (*active != kNone) {
                if (!*skipped) {
                    if (status.level != *active_level || status.loads != *active_loads) {
                        unsafe->insert(*active);
                    } else {
                        *candidate = *active;
                        *candidate_level = *active_level;
                        *candidate_loads = *active_loads;
                        *grace = 90;
                    }
                }
                *active = kNone;
                *skipped = false;
            }
            return;
        }

        const auto key = scene_key(status.level, scene);
        if (*active != key) {
            if (*active != kNone && !*skipped) {
                *candidate = *active;
                *candidate_level = *active_level;
                *candidate_loads = *active_loads;
                *grace = 90;
            }
            *active = key;
            *active_level = status.level;
            *active_loads = status.loads;
            *skipped = false;
        }

        if (mode == 1 && !*skipped && frame >= minimum && safe->count(key) != 0
            && unsafe->count(key) == 0) {
            *skipped = mod.play_skip();
        }
    });

    mod.info(mode == 0 ? "automatic scene skipping disabled"
                       : "safe-repeat scene skipping active");
}

OPENRAC_MOD_CPP("cutscene-manager", "0.2.0", load)
