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
    imgui imgui{};
    std::mutex mutex;
    bool is_frame{false};
};

extern hbdraw g_hbdraw;
