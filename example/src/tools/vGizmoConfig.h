//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
//
//  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
//  
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#pragma once

// uncomment to use DOUBLE precision
//      It automatically enable also VGIZMO_USES_TEMPLATE (read below)
// Default ==> SINGLE precision: float
//------------------------------------------------------------------------------
//#define VGIZMO_USES_DOUBLE_PRECISION

// uncomment to use TEMPLATE internal vGizmoMath classes/types
//
// This is if you need to extend the use of different math types in your code
//      or for your purposes:
//          float  ==>  vec2 /  vec3 /  vec4 /  quat /  mat3 /  mat4
//          double ==> dvec2 / dvec3 / dvec4 / dquat / dmat3 / dmat4
// If you select TEMPLATE classes the widget too will use internally them 
//      with single precision (float)
// Default ==> NO template
//------------------------------------------------------------------------------
//#define VGIZMO_USES_TEMPLATE

// uncomment to use "glm" (0.9.9 or higher) library instead of vGizmoMath
//      Need to have "glm" installed and in your INCLUDE research compiler path
//
// vGizmoMath is a subset of GLM: it is only intended to make vGizmo / ImGuIZMOquat
//      standalone and/or to avoid TEMPLATES uses.
//      All vGizmoMath sub-set is compatible with GLM types and calls
// Default ==> use vGizmoMath
//      If you enable GLM use, automatically is enabled also VGIZMO_USES_TEMPLATE
//          if you can, I recommend to use GLM
//------------------------------------------------------------------------------
//#define VGIZMO_USES_GLM

// uncomment to use LeftHanded 
//
// This is used only in: lookAt / perspective / ortho / frustrum - functions
//      DX is LeftHanded, OpenGL is RightHanded
// Default ==> RightHanded
//------------------------------------------------------------------------------
//#define VGIZMO_USES_LEFT_HAND_AXES

#ifdef VGIZMO_USES_DOUBLE_PRECISION
    #define VG_T_TYPE double
    #define VGIZMO_USES_TEMPLATE
#else
    #define VG_T_TYPE float
#endif

#ifdef VGIZMO_USES_GLM

    #ifndef VGIZMO_USES_TEMPLATE
        #define VGIZMO_USES_TEMPLATE
    #endif

    #include <glm/glm.hpp>
    #include <glm/gtx/vector_angle.hpp>
    #include <glm/gtx/exterior_product.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/gtc/matrix_transform.hpp>

    using namespace glm;

    using tVec2 = tvec2<VG_T_TYPE>;
    using tVec3 = tvec3<VG_T_TYPE>;
    using tVec4 = tvec4<VG_T_TYPE>;
    using tQuat = tquat<VG_T_TYPE>;
    using tMat3 = tmat3x3<VG_T_TYPE>;
    using tMat4 = tmat4x4<VG_T_TYPE>;

    #define T_PI pi<VG_T_TYPE>()

    #define VGIZMO_BASE_CLASS virtualGizmoBaseClass<T>
    #define TEMPLATE_TYPENAME_T  template<typename T>
#else
    #ifdef VGIZMO_USES_TEMPLATE
        #define TEMPLATE_TYPENAME_T  template<typename T>

        #define VEC2_T Vec2<T>
        #define VEC3_T Vec3<T>
        #define VEC4_T Vec4<T>
        #define QUAT_T Quat<T>
        #define MAT3_T Mat3<T>
        #define MAT4_T Mat4<T>

        #define VEC2_PRECISION Vec2<VG_T_TYPE>
        #define VEC3_PRECISION Vec3<VG_T_TYPE>
        #define VEC4_PRECISION Vec4<VG_T_TYPE>
        #define QUAT_PRECISION Quat<VG_T_TYPE>
        #define MAT3_PRECISION Mat3<VG_T_TYPE>
        #define MAT4_PRECISION Mat4<VG_T_TYPE>
        
        #define T_PI vgm::pi<VG_T_TYPE>()

        #define VGIZMO_BASE_CLASS virtualGizmoBaseClass<T>

        #include "vGizmoMath.h"
        using namespace vgm;

        using vec2 = Vec2<float>;
        using vec3 = Vec3<float>;
        using vec4 = Vec4<float>;
        using quat = Quat<float>;
        using mat3 = Mat3<float>;
        using mat4 = Mat4<float>;

        using dvec2 = Vec2<double>;;
        using dvec3 = Vec3<double>;;
        using dvec4 = Vec4<double>;;
        using dquat = Quat<double>;;
        using dmat3 = Mat3<double>;;
        using dmat4 = Mat4<double>;;
    #else
        #define TEMPLATE_TYPENAME_T

        //#define T float

        #define VEC2_T Vec2
        #define VEC3_T Vec3
        #define VEC4_T Vec4
        #define QUAT_T Quat
        #define MAT3_T Mat3
        #define MAT4_T Mat4

        #define VEC2_PRECISION Vec2
        #define VEC3_PRECISION Vec3
        #define VEC4_PRECISION Vec4
        #define QUAT_PRECISION Quat
        #define MAT3_PRECISION Mat3
        #define MAT4_PRECISION Mat4

        #define T_PI vgm::pi()

        #define VGIZMO_BASE_CLASS virtualGizmoBaseClass

        #include "vGizmoMath.h"
        using namespace vgm;

        using vec2 = Vec2;
        using vec3 = Vec3;
        using vec4 = Vec4;
        using quat = Quat;
        using mat3 = Mat3;
        using mat4 = Mat4;
    #endif

    using tVec2 = VEC2_PRECISION ;
    using tVec3 = VEC3_PRECISION ;
    using tVec4 = VEC4_PRECISION ;
    using tQuat = QUAT_PRECISION ;
    using tMat3 = MAT3_PRECISION ;
    using tMat4 = MAT4_PRECISION ;

    #undef VEC2_T
    #undef VEC3_T
    #undef VEC4_T
    #undef QUAT_T
    #undef MAT3_T
    #undef MAT4_T

    #undef VEC2_PRECISION
    #undef VEC3_PRECISION
    #undef VEC4_PRECISION
    #undef QUAT_PRECISION
    #undef MAT3_PRECISION
    #undef MAT4_PRECISION

#endif
