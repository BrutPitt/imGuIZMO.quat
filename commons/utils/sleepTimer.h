//------------------------------------------------------------------------------
//  Copyright (c) 2025 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#pragma once

#if defined (__linux__) || defined (__APPLE__) // POSIX
#include <unistd.h>
    void waitFor(unsigned usec = 16000) { usleep(usec); }
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
void waitFor(DWORD usec = 16000)  { Sleep(static_cast<DWORD>(usec / 1000)); }
#elif defined(__EMSCRIPTEN__)
#else
void waitFor(unsigned usec = 16000) {}
    #warning "No sleep timer!"
#endif
