#include <cstdint>
#include <memory>
#include <unordered_set>
#include "openrac/mod.hpp"
#include "logic.hpp"

void load(openrac::Mod& mod){
    if(mod.game()!="rac1" || !mod.has_scenes()) throw openrac::Error("Cutscene Manager requires RAC1 interface 1.9+");
    const int mode=static_cast<int>(mod.setting<std::int64_t>("mode"));
    const int minimum=static_cast<int>(mod.setting<std::int64_t>("min_frames"));
    auto seen=std::make_shared<std::unordered_set<std::uint64_t>>();
    auto active=std::make_shared<std::uint64_t>(~std::uint64_t{0});
    auto skipped=std::make_shared<bool>(false);
    mod.on_frame([&mod,mode,minimum,seen,active,skipped]{
        const auto status=mod.play_status();
        int frame=0;
        const int scene=mod.scene_playing(&frame);
        if(scene<0 || status.level<0){
            if(*active!=~std::uint64_t{0}) seen->insert(*active);
            *active=~std::uint64_t{0}; *skipped=false; return;
        }
        const auto key=(static_cast<std::uint64_t>(static_cast<std::uint32_t>(status.level))<<32) | static_cast<std::uint32_t>(scene);
        if(*active!=key){
            if(*active!=~std::uint64_t{0}) seen->insert(*active);
            *active=key; *skipped=false;
        }
        if(!*skipped && qol::should_skip(mode,seen->count(key)!=0,frame,minimum)){
            *skipped=mod.play_skip();
            if(*skipped) seen->insert(key);
        }
    });
    mod.info("cutscene policy active");
}
OPENRAC_MOD_CPP("cutscene-manager","0.1.0",load)
