#pragma once

#include <sol/sol.hpp>

#include "scene.h"
#include <mutex>

struct imgui {
    bool initialized{false};
    unsigned num_segments = 32;
    unsigned outline_tickness = 1;
};

struct hbdraw {
    lua_State *lua{};
    camera camera{};
    bool w2s{true};
    imgui imgui{};
    std::vector<sol::protected_function> draw_fns;
    std::mutex mutex;
};

extern hbdraw g_hbdraw;
