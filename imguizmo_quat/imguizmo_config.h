//------------------------------------------------------------------------------
//  Copyright (c) 2018-2025 Michele Morrone
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
//------------------------------------------------------------------------------
// v3.0 and later
//
//      IMGUIZMO_USE_ONLY_ROT
//
//      Used to remove Pan & Dolly feature to imGuIZMO.quat widget and to use
//          only rotation feature (like v2.2 and above)
//
//          Pan/Dolly use virtualGizmo3DClass just a little bit complex of
//          virtualGizmoClass that uses only "quat" rotations
//          uncomment for very low resources ==> Pan & Dolly will be disabled
//
// Default ==> Pan & Dolly enabled
//------------------------------------------------------------------------------

//#define IMGUIZMO_USE_ONLY_ROT

//------------------------------------------------------------------------------
// v3.1 and later
//
//      IMGUIZMO_MANUAL_IMGUI_INCLUDE
//
//        - if DEFINED (uncommented/defined) is necessary to specify the following
//            ImGui includes:
//                  #include <imgui.h>
//                  #include <imgui_internal.h>
//            or
//                  #include <imgui_folder/imgui.h>
//                  #include <imgui_folder/imgui_internal.h>
//            or
//                  #include "imgui_folder/imgui.h"
//                  #include "imgui_folder/imgui_internal.h"
//
//            in relation to where is your imgui_folder and if is in your "INCLUDE"
//              search paths
//
//          if DEFINED (uncommented/defined) IMGUIZMO_IMGUI_FOLDER is NOT used
//            (see below)
//------------------------------------------------------------------------------

// #define IMGUIZMO_MANUAL_IMGUI_INCLUDE
// #include <imgui.h>
// #include <imgui_internal.h>

//------------------------------------------------------------------------------
// v3.0 and later ==> from 3.1 non more strictly necessary
//                                              (maintained for compatibility)
//
//      IMGUIZMO_IMGUI_FOLDER
//
//      used to specify where ImGui include files should be searched
//          #define IMGUIZMO_IMGUI_FOLDER
//              is equivalent to use:
//                  #include <imgui.h>
//                  #include <imgui_internal.h>
//
//          #define IMGUIZMO_IMGUI_FOLDER myLibs/ImGui/
//              (final slash is REQUIRED) is equivalent to use:
//                  #include <myLib/ImGui/imgui.h>
//                  #include <myLib/ImGui/imgui_internal.h>
//
//          Default: IMGUIZMO_IMGUI_FOLDER undefined (commented)
//              is equivalent to use:
//                  #include <imgui/imgui.h>
//                  #include <imgui/imgui_internal.h>
//
// N.B. Final slash to end of path is REQUIRED!
//------------------------------------------------------------------------------

// #define IMGUIZMO_IMGUI_FOLDER ImGui/

// TODO for v.3.2?
//------------------------------------------------------------------------------
// v3.1 and later
//
//      IMGUIZMO_FLIP_LIGHT
//------------------------------------------------------------------------------


#define BACKEND_IS_VULKAN
