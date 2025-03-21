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
    std::mutex mutex;
    camera camera{};
    bool w2s{true};
    imgui imgui{};
    bool do_new_frame{true};
};

extern hbdraw g_hbdraw;
