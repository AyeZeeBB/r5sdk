#pragma once

__declspec(dllexport) void DummyExport()
{
    // Required for detours.
}

static const char* const R5R_EMBLEM[] =
{
    R"(+-------------------------------------------------------------+)",
    R"(|  ___ ___ ___     _              _        _       ___   ____ |)",
    R"(| | _ \ __| _ \___| |___  __ _ __| |___ __| | __ _|_  ) |__ / |)",
    R"(| |   /__ \   / -_) / _ \/ _` / _` / -_) _` | \ V // / _ |_ \ |)",
    R"(| |_|_\___/_|_\___|_\___/\__,_\__,_\___\__,_|  \_//___(_)___/ |)",
    R"(|                                                             |)"/*,
    R"(+-------------------------------------------------------------+)"*/
};
