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

#include "vGizmoConfig.h"
#include <cmath>

namespace vgm {

TEMPLATE_TYPENAME_T class Vec4;
TEMPLATE_TYPENAME_T class Mat4;

#if !defined(VGIZMO_USES_TEMPLATE)
    #define T VG_T_TYPE
#endif

// Vec2
//////////////////////////
TEMPLATE_TYPENAME_T class Vec2 {
public:
    union {
        struct { T x, y; };
        struct { T u, v; };
    };

    Vec2() {}
    Vec2(T s) : x(s), y(s) {}
    Vec2(T x, T y) : x(x), y(y) {}

    const Vec2 operator-() const { return Vec2(-x, -y); }

    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    Vec2& operator*=(const Vec2& v) { x *= v.x; y *= v.y; return *this; }
    Vec2& operator/=(const Vec2& v) { x /= v.x; y /= v.y; return *this; }
    Vec2& operator*=(T s)           { x *= s  ; y *= s  ; return *this; }
    Vec2& operator/=(T s)           { x /= s  ; y /= s  ; return *this; }

    const Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    const Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    const Vec2 operator*(const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
    const Vec2 operator/(const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
    const Vec2 operator*(T s)           const { return Vec2(x * s  , y * s  ); }
    const Vec2 operator/(T s)           const { return Vec2(x / s  , y / s  ); }

    const T& operator[](int i) const { return *(&x + i); }
          T& operator[](int i)       { return *(&x + i); }

    operator const T *() const { return &x; }
    operator       T *()       { return &x; }
};
// Vec3
//////////////////////////
TEMPLATE_TYPENAME_T class Vec3 {
public:
    union {
        struct { T x, y, z; };
        struct { T r, g, b; };
    };

    Vec3() {}
    Vec3(T s) : x(s), y(s), z(s) {}
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    Vec3(T s, const VEC2_T& v)  : x(s), y(v.x), z(v.y) {}
    Vec3(const VEC2_T& v, T s = T(0))  : x(v.x), y(v.y), z(s) {}
    Vec3(const VEC4_T& v);

    const Vec3 operator-() const { return Vec3(-x, -y, -z); }

    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3& operator*=(const Vec3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
    Vec3& operator/=(const Vec3& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
    Vec3& operator*=(T s)           { x *= s  ; y *= s  ; z *= s  ; return *this; }
    Vec3& operator/=(T s)           { x /= s  ; y /= s  ; z /= s  ; return *this; }

    const Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    const Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    const Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    const Vec3 operator/(const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }
    const Vec3 operator*(T s)           const { return Vec3(x * s  , y * s  , z * s  ); }
    const Vec3 operator/(T s)           const { return Vec3(x / s  , y / s  , z / s  ); }

    const T& operator[](int i) const { return *(&x + i); }
          T& operator[](int i)       { return *(&x + i); }

    operator const T *() const { return &x; }
    operator       T *()       { return &x; }
};
// Vec4
//////////////////////////
TEMPLATE_TYPENAME_T class Vec4 {
public:
    union {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
    };

    Vec4() {}
    Vec4(T s)                          : x(s),   y(s),   z(s),   w(s) {}
    Vec4(T x, T y, T z, T w)           : x(x),   y(y),   z(z),   w(w) {}
    Vec4(const VEC3_T& v, T s = T(0))  : x(v.x), y(v.y), z(v.z), w(s) {}

    const Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }
    
    Vec4& operator+=(const Vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    Vec4& operator-=(const Vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    Vec4& operator*=(const Vec4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
    Vec4& operator/=(const Vec4& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
    Vec4& operator*=(T s)           { x *= s  ; y *= s  ; z *= s  ; w *= s  ; return *this; }
    Vec4& operator/=(T s)           { x /= s  ; y /= s  ; z /= s  ; w /= s  ; return *this; }

    const Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    const Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    const Vec4 operator*(const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
    const Vec4 operator/(const Vec4& v) const { return Vec4(x / v.x, y / v.y, z / v.z, w / v.w); }
    const Vec4 operator*(T s)           const { return Vec4(x * s  , y * s  , z * s  , w * s  ); }
    const Vec4 operator/(T s)           const { return Vec4(x / s  , y / s  , z / s  , w / s  ); }

    const T& operator[](int i) const { return *(&x + i); }
          T& operator[](int i)       { return *(&x + i); }

    operator const T *() const { return &x; }
    operator       T *()       { return &x; }
};
// Quat
//////////////////////////
TEMPLATE_TYPENAME_T class Quat {
public:
    T x, y, z, w;

    Quat() {}
    Quat(T w, T x, T y, T z)   : x(x),   y(y),   z(z),   w(w)   {}
    Quat(const QUAT_T& q)      : x(q.x), y(q.y), z(q.z), w(q.w) {}
    Quat(T s, const VEC3_T& v) : x(v.x), y(v.y), z(v.z), w(s)   {}

    const Quat operator-() const { return Quat(-w, -x, -y, -z); }

    Quat& operator+=(const Quat& q)  { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
    Quat& operator-=(const Quat& q)  { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
    Quat& operator*=(const Quat& q)  { return *this = *this * q; }
    Quat& operator*=(T s)            { x *= s  ; y *= s  ; z *= s  ; w *= s  ; return *this; }
    Quat& operator/=(T s)            { x /= s  ; y /= s  ; z /= s  ; w /= s  ; return *this; }

    const Quat operator+(const Quat& q) const { return Quat(w + q.w, x + q.x, y + q.y, z + q.z); }
    const Quat operator-(const Quat& q) const { return Quat(w - q.w, x - q.x, y - q.y, z - q.z); }
    const Quat operator*(const Quat& q) const { return Quat(w * q.w - x * q.x - y * q.y - z * q.z,
                                                            w * q.x + x * q.w + y * q.z - z * q.y,
                                                            w * q.y + y * q.w + z * q.x - x * q.z,
                                                            w * q.z + z * q.w + x * q.y - y * q.x); }
                                                
    const Quat operator*(T s) const { return Quat(w * s, x * s  , y * s  , z * s); }
    const Quat operator/(T s) const { return Quat(w / s, x / s  , y / s  , z / s); }

    const T& operator[](int i) const { return *(&x + i); }
          T& operator[](int i)       { return *(&x + i); }

    operator const T *() const { return &x; }
    operator       T *()       { return &x; }
};
// Mat3
//////////////////////////
TEMPLATE_TYPENAME_T class Mat3 {
public:
    union {
        struct { VEC3_T v0 , v1 , v2 ; };
        struct {      T m00, m01, m02,
                        m10, m11, m12,
                        m20, m21, m22; };
    };

    Mat3() {}
    Mat3(T s) : v0(s, 0, 0), v1(0, s, 0), v2(0, 0, s) {}
    Mat3(const VEC3_T& v0, const VEC3_T& v1, const VEC3_T& v2) : v0(v0), v1(v1), v2(v2) {}
    Mat3(const MAT4_T& m);
    Mat3(T v0x, T v0y, T v0z,
         T v1x, T v1y, T v1z,
         T v2x, T v2y, T v2z) : v0(v0x, v0y, v0z), v1(v1x, v1y, v1z), v2(v2x, v2y, v2z) {}

    const VEC3_T& operator[](int i) const { return *(&v0 + i); }
          VEC3_T& operator[](int i)       { return *(&v0 + i); }

    const Mat3 operator-() const { return Mat3(-v0, -v1, -v2); }
    
    Mat3& operator+=(const Mat3& m) { v0 += m.v0; v1 += m.v1; v2 += m.v2; return *this; }
    Mat3& operator-=(const Mat3& m) { v0 -= m.v0; v1 -= m.v1; v2 -= m.v2; return *this; }
    Mat3& operator*=(const Mat3& m) { return *this = *this * m;  }
    Mat3& operator/=(const Mat3& m) { v0 /= m.v0; v1 /= m.v1; v2 /= m.v2; return *this; }
    Mat3& operator*=(T s)           { v0 *= s;    v1 *= s;    v2 *= s;    return *this; }
    Mat3& operator/=(T s)           { v0 /= s;    v1 /= s;    v2 /= s;    return *this; }

    const Mat3 operator+(const Mat3& m) const { return Mat3(v0 + m.v0, v1 + m.v1, v2 + m.v2); }
    const Mat3 operator-(const Mat3& m) const { return Mat3(v0 - m.v0, v1 - m.v1, v2 - m.v2); }
    const Mat3 operator*(const Mat3& m) const { return Mat3( m00 * m.m00 + m10 * m.m01 + m20 * m.m02,
                                                             m01 * m.m00 + m11 * m.m01 + m21 * m.m02,
                                                             m02 * m.m00 + m12 * m.m01 + m22 * m.m02,
                                                             m00 * m.m10 + m10 * m.m11 + m20 * m.m12,
                                                             m01 * m.m10 + m11 * m.m11 + m21 * m.m12,
                                                             m02 * m.m10 + m12 * m.m11 + m22 * m.m12,
                                                             m00 * m.m20 + m10 * m.m21 + m20 * m.m22,
                                                             m01 * m.m20 + m11 * m.m21 + m21 * m.m22,
                                                             m02 * m.m20 + m12 * m.m21 + m22 * m.m22); }

    const Mat3 operator*(T s) const { return Mat3(v0 * s   , v1 * s   , v2 * s   ); }
    const Mat3 operator/(T s) const { return Mat3(v0 / s   , v1 / s   , v2 / s   ); }

    const VEC3_T operator*(const VEC3_T& v) const { return VEC3_T(m00 * v.x + m10 * v.y + m20 * v.z,
                                                                  m01 * v.x + m11 * v.y + m21 * v.z,
                                                                  m02 * v.x + m12 * v.y + m22 * v.z); }
    operator const T *() const { return &m00; }
    operator       T *()       { return &m00; }
};
// Mat4
//////////////////////////
TEMPLATE_TYPENAME_T class Mat4 {
public:
    union {
        struct { VEC4_T v0 , v1 , v2 , v3 ; };
        struct {      T m00, m01, m02, m03,
                        m10, m11, m12, m13,
                        m20, m21, m22, m23,
                        m30, m31, m32, m33; };
    };

    Mat4() {} 
    Mat4(T s) : v0(s, 0, 0, 0), v1(0, s, 0, 0), v2(0, 0, s, 0), v3(0, 0, 0, s) {}
    Mat4(const VEC4_T& v0, const VEC4_T& v1, const VEC4_T& v2, const VEC4_T& v3) : v0(v0), v1(v1), v2(v2), v3(v3) {}
    Mat4(const MAT3_T& m) : v0(m.v0), v1(m.v1), v2(m.v2), v3(0, 0, 0, 1) {}
    Mat4(T v0x, T v0y, T v0z, T v0w,
         T v1x, T v1y, T v1z, T v1w,
         T v2x, T v2y, T v2z, T v2w,
         T v3x, T v3y, T v3z, T v3w) : v0(v0x, v0y, v0z, v0w), v1(v1x, v1y, v1z, v1w), v2(v2x, v2y, v2z, v2w), v3(v3x, v3y, v3z, v3w) {}

    const VEC4_T& operator[](int i) const { return *(&v0 + i); }
          VEC4_T& operator[](int i)       { return *(&v0 + i); }

    const Mat4 operator-() const { return Mat4(-v0, -v1, -v2, -v3); }

    Mat4& operator+=(const Mat4& m) { v0 += m.v0; v1 += m.v1; v2 += m.v2; v3 += m.v3; return *this; }
    Mat4& operator-=(const Mat4& m) { v0 -= m.v0; v1 -= m.v1; v2 -= m.v2; v3 -= m.v3; return *this; }
    Mat4& operator*=(const Mat4& m) { return *this = *this * m; }
    Mat4& operator/=(const Mat4& m) { v0 /= m.v0; v1 /= m.v1; v2 /= m.v2; v3 /= m.v3; return *this; }
    Mat4& operator*=(T s)           { v0 *= s;    v1 *= s;    v2 *= s;    v3 *= s;    return *this; }
    Mat4& operator/=(T s)           { v0 /= s;    v1 /= s;    v2 /= s;    v3 /= s;    return *this; }

    const Mat4 operator+(const Mat4& m) const { return Mat4(v0 + m.v0, v1 + m.v1, v2 + m.v2, v3 + m.v3); }
    const Mat4 operator-(const Mat4& m) const { return Mat4(v0 - m.v0, v1 - m.v1, v2 - m.v2, v3 - m.v3); }
    const Mat4 operator*(T s)           const { return Mat4(v0 * s   , v1 * s   , v2 * s   , v3 * s   ); }
    const Mat4 operator/(T s)           const { return Mat4(v0 / s   , v1 / s   , v2 / s   , v3 / s   ); }

    const Mat4 operator*(const Mat4& m) const { return Mat4( m00 * m.m00 + m10 * m.m01 + m20 * m.m02 + m30 * m.m03,
                                                             m01 * m.m00 + m11 * m.m01 + m21 * m.m02 + m31 * m.m03,
                                                             m02 * m.m00 + m12 * m.m01 + m22 * m.m02 + m32 * m.m03,
                                                             m03 * m.m00 + m13 * m.m01 + m23 * m.m02 + m33 * m.m03,
                                                             m00 * m.m10 + m10 * m.m11 + m20 * m.m12 + m30 * m.m13,
                                                             m01 * m.m10 + m11 * m.m11 + m21 * m.m12 + m31 * m.m13,
                                                             m02 * m.m10 + m12 * m.m11 + m22 * m.m12 + m32 * m.m13,
                                                             m03 * m.m10 + m13 * m.m11 + m23 * m.m12 + m33 * m.m13,
                                                             m00 * m.m20 + m10 * m.m21 + m20 * m.m22 + m30 * m.m23,
                                                             m01 * m.m20 + m11 * m.m21 + m21 * m.m22 + m31 * m.m23,
                                                             m02 * m.m20 + m12 * m.m21 + m22 * m.m22 + m32 * m.m23,
                                                             m03 * m.m20 + m13 * m.m21 + m23 * m.m22 + m33 * m.m23,
                                                             m00 * m.m30 + m10 * m.m31 + m20 * m.m32 + m30 * m.m33,
                                                             m01 * m.m30 + m11 * m.m31 + m21 * m.m32 + m31 * m.m33,
                                                             m02 * m.m30 + m12 * m.m31 + m22 * m.m32 + m32 * m.m33,
                                                             m03 * m.m30 + m13 * m.m31 + m23 * m.m32 + m33 * m.m33);  }

    const VEC4_T operator*(const VEC4_T& v) const { return VEC4_T(m00 * v.x + m10 * v.y + m20 * v.z + m30 * v.w,
                                                                  m01 * v.x + m11 * v.y + m21 * v.z + m31 * v.w,
                                                                  m02 * v.x + m12 * v.y + m22 * v.z + m32 * v.w,
                                                                  m03 * v.x + m13 * v.y + m23 * v.z + m33 * v.w); }
    operator const T *() const { return &m00; }
    operator       T *()       { return &m00; }
};
// cast / conversion
//////////////////////////
TEMPLATE_TYPENAME_T inline VEC3_T::Vec3(const VEC4_T& v) : x(v.x), y(v.y), z(v.z) {}
TEMPLATE_TYPENAME_T inline MAT3_T::Mat3(const MAT4_T& m) : v0(m.v0), v1(m.v1), v2(m.v2) {}
TEMPLATE_TYPENAME_T inline MAT3_T mat3_cast(QUAT_T const& q) {
    T xx(q.x * q.x); T yy(q.y * q.y); T zz(q.z * q.z);
    T xz(q.x * q.z); T xy(q.x * q.y); T yz(q.y * q.z);
    T wx(q.w * q.x); T wy(q.w * q.y); T wz(q.w * q.z);

    return MAT3_T( T(1) - T(2) * (yy + zz),         T(2) * (xy + wz),         T(2) * (xz - wy),
                          T(2) * (xy - wz),  T(1) - T(2) * (xx + zz),         T(2) * (yz + wx),
                          T(2) * (xz + wy),         T(2) * (yz - wx),  T(1) - T(2) * (xx + yy)); }
TEMPLATE_TYPENAME_T inline MAT4_T mat4_cast(QUAT_T const& q) { return MAT4_T(mat3_cast(q)); }
// dot
//////////////////////////
TEMPLATE_TYPENAME_T inline T dot(const VEC2_T& v0, const VEC2_T& v1) { return v0.x * v1.x + v0.y * v1.y; }
TEMPLATE_TYPENAME_T inline T dot(const VEC3_T& v0, const VEC3_T& v1) { return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z; }
TEMPLATE_TYPENAME_T inline T dot(const VEC4_T& v0, const VEC4_T& v1) { return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w; }
TEMPLATE_TYPENAME_T inline T dot(const QUAT_T& q0, const QUAT_T& q1) { return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w; }
// cross
//////////////////////////
TEMPLATE_TYPENAME_T inline const T cross(const VEC2_T& u, const VEC2_T& v) { return u.x * v.y - v.x * u.y; }
TEMPLATE_TYPENAME_T inline const VEC3_T cross(const VEC3_T& u, const VEC3_T& v) { return VEC3_T(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x); }
// length
//////////////////////////
TEMPLATE_TYPENAME_T inline T length(const VEC2_T& v) { return sqrt(dot(v, v)); }
TEMPLATE_TYPENAME_T inline T length(const VEC3_T& v) { return sqrt(dot(v, v)); }
TEMPLATE_TYPENAME_T inline T length(const VEC4_T& v) { return sqrt(dot(v, v)); }
TEMPLATE_TYPENAME_T inline T length(const QUAT_T& q) { return sqrt(dot(q, q)); }
//abs
//////////////////////////
TEMPLATE_TYPENAME_T inline const T tAbs(T x) { return x>=T(0) ? x : -x; }
TEMPLATE_TYPENAME_T inline const VEC2_T abs(const VEC2_T& v) { return VEC2_T(tAbs(v.x), tAbs(v.y)); }
TEMPLATE_TYPENAME_T inline const VEC3_T abs(const VEC3_T& v) { return VEC3_T(tAbs(v.x), tAbs(v.y), tAbs(v.z)); }
TEMPLATE_TYPENAME_T inline const VEC4_T abs(const VEC4_T& v) { return VEC4_T(tAbs(v.x), tAbs(v.y), tAbs(v.z), tAbs(v.w)); }
TEMPLATE_TYPENAME_T inline const QUAT_T abs(const QUAT_T& q) { return QUAT_T(tAbs(q.w), tAbs(q.x), tAbs(q.y), tAbs(q.z)); }
// normalize
//////////////////////////
TEMPLATE_TYPENAME_T inline const VEC2_T normalize(const VEC2_T& v) { return v / length(v); }
TEMPLATE_TYPENAME_T inline const VEC3_T normalize(const VEC3_T& v) { return v / length(v); }
TEMPLATE_TYPENAME_T inline const VEC4_T normalize(const VEC4_T& v) { return v / length(v); }
TEMPLATE_TYPENAME_T inline const QUAT_T normalize(const QUAT_T& q) { return q / length(q); }
// value_ptr
//////////////////////////
TEMPLATE_TYPENAME_T inline T *value_ptr(const VEC2_T &v) { return const_cast<T *>(&v.x); }
TEMPLATE_TYPENAME_T inline T *value_ptr(const VEC3_T &v) { return const_cast<T *>(&v.x); }
TEMPLATE_TYPENAME_T inline T *value_ptr(const VEC4_T &v) { return const_cast<T *>(&v.x); }
TEMPLATE_TYPENAME_T inline T *value_ptr(const QUAT_T &q) { return const_cast<T *>(&q.x); }
TEMPLATE_TYPENAME_T inline T *value_ptr(const MAT3_T &m) { return const_cast<T *>(&m.m00); }
TEMPLATE_TYPENAME_T inline T *value_ptr(const MAT4_T &m) { return const_cast<T *>(&m.m00); }
// transpose
//////////////////////////
TEMPLATE_TYPENAME_T inline const MAT3_T transpose(MAT3_T m) {
    return MAT3_T (m.m00, m.m10, m.m20,
                   m.m01, m.m11, m.m21,
                   m.m02, m.m12, m.m22); }
TEMPLATE_TYPENAME_T inline const MAT4_T transpose(MAT4_T m) {
    return MAT4_T (m.m00, m.m10, m.m20, m.m30,
                   m.m01, m.m11, m.m21, m.m31,
                   m.m02, m.m12, m.m22, m.m32,
                   m.m03, m.m13, m.m23, m.m33); }
// inverse
//////////////////////////
TEMPLATE_TYPENAME_T inline const QUAT_T inverse(QUAT_T q) { return QUAT_T(q.w, -q.x, -q.y, -q.z) / dot(q, q); }
TEMPLATE_TYPENAME_T inline const MAT3_T inverse(MAT3_T m) {
    T invDet = T(1) / (   m.m00 * (m.m11 * m.m22 - m.m21 * m.m12)
                        - m.m10 * (m.m01 * m.m22 - m.m21 * m.m02)
                        + m.m20 * (m.m01 * m.m12 - m.m11 * m.m02));

    return MAT3_T(  (m.m11 * m.m22 - m.m21 * m.m12), - (m.m01 * m.m22 - m.m21 * m.m02),   (m.m01 * m.m12 - m.m11 * m.m02),
                  - (m.m10 * m.m22 - m.m20 * m.m12),   (m.m00 * m.m22 - m.m20 * m.m02), - (m.m00 * m.m12 - m.m10 * m.m02),
                    (m.m10 * m.m21 - m.m20 * m.m11), - (m.m00 * m.m21 - m.m20 * m.m01),   (m.m00 * m.m11 - m.m10 * m.m01)) * invDet; }
// external operators
//////////////////////////
TEMPLATE_TYPENAME_T inline VEC3_T operator*(const QUAT_T& q, const VEC3_T& v) {
    const VEC3_T qV(q.x, q.y, q.z), uv(cross(qV, v));
    return v + ((uv * q.w) + cross(qV, uv)) * T(2); }
TEMPLATE_TYPENAME_T inline  VEC3_T operator*(const VEC3_T& v, const QUAT_T& q) {  return inverse(q) * v; }
// translate
//////////////////////////
TEMPLATE_TYPENAME_T inline const MAT4_T translate(MAT4_T const& m, VEC3_T const& v) {
    MAT4_T r(m); r[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3]; 
    return r; }
// quat angle/axis
//////////////////////////
TEMPLATE_TYPENAME_T inline QUAT_T angleAxis(T const &a, VEC3_T const &v) {	return QUAT_T(cos(a * T(0.5)), v * sin(a * T(0.5))); }
TEMPLATE_TYPENAME_T inline T angle(QUAT_T const& q) { return acos(q.w) * T(2); }
TEMPLATE_TYPENAME_T inline VEC3_T axis(QUAT_T const& q) {
    const T t1 = T(1) - q.w * q.w; if(t1 <= T(0)) return VEC3_T(0, 0, 1);
    const T t2 = T(1) / sqrt(t1);  return VEC3_T(q.x * t2, q.y * t2, q.z * t2); }
// trigonometric
//////////////////////////
TEMPLATE_TYPENAME_T inline T pi() { return T(3.1415926535897932384626433832795029); }
TEMPLATE_TYPENAME_T inline T radians(T d) { return d * T(0.0174532925199432957692369076849); }
TEMPLATE_TYPENAME_T inline T degrees(T r) { return r * T(57.295779513082320876798154814105); }
// lookAt
//////////////////////////
TEMPLATE_TYPENAME_T inline const MAT4_T lookAt(const VEC3_T& pov, const VEC3_T& tgt, const VEC3_T& up)
{
#ifdef VGIZMO_USES_LEFT_HAND_AXES
    VEC3_T k = normalize(tgt - pov), i = normalize(cross(up, k)), j = cross(k, i);
#else
    VEC3_T k = normalize(tgt - pov), i = normalize(cross(k, up)), j = cross(i, k);   k = -k;
#endif
    return MAT4_T(     i.x,          j.x,          k.x,     T(0),
                       i.y,          j.y,          k.y,     T(0),
                       i.z,          j.z,          k.z,     T(0),
                  -dot(i, pov), -dot(j, pov), -dot(k, pov), T(1)); }
// ortho
//////////////////////////
TEMPLATE_TYPENAME_T inline const MAT4_T ortho(T l, T r, T b, T t, T n, T f)
{
#ifdef VGIZMO_USES_LEFT_HAND_AXES
    const T v = T(2);
#else
    const T v = T(-2);
#endif
    return MAT4_T(  T(2)/(r-l),     T(0),         T(0),     T(0),
                      T(0),       T(2)/(t-b),     T(0),     T(0),
                      T(0),         T(0),        v/(f-n),   T(0),
                  -(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), T(1)); }
// perspective
//////////////////////////
TEMPLATE_TYPENAME_T inline const MAT4_T perspective(T fovy, T a, T n, T f)
{
#ifdef VGIZMO_USES_LEFT_HAND_AXES
    const T v = T(1), f_n = (f+n)/(f-n);
#else
    const T v = T(-1), f_n = -(f+n)/(f-n);
#endif
    const T hFovy = tan(fovy / T(2));
    return MAT4_T(  T(1)/(a*hFovy),  T(0),           T(0),      T(0),
                      T(0),        T(1)/(hFovy),     T(0),      T(0),
                      T(0),          T(0),            f_n,        v ,
                      T(0),          T(0),   -(T(2)*f*n)/(f-n), T(0)); }
// perspectiveFov
//////////////////////////
TEMPLATE_TYPENAME_T inline const MAT4_T perspectiveFov(T fovy, T w, T h, T n, T f) { return perspective(fovy, w/h, n, f); }
// frustrum
//////////////////////////
TEMPLATE_TYPENAME_T inline const MAT4_T frustrum(T l, T r, T b, T t, T n, T f)
{
#ifdef VGIZMO_USES_LEFT_HAND_AXES
    const T v = T(1),  f_n =  (f+n)/(f-n);
#else
    const T v = T(-1), f_n = -(f+n)/(f-n);
#endif
    return MAT4_T((T(2)*n)/(r-l),       T(0),         T(0),         T(0),
                        T(0),     (T(2)*n)/(t-b),     T(0),         T(0),
                     (r+l)/(r-l),    (t+b)/(t-b),      f_n,           v ,
                        T(0),           T(0),    -(T(2)*f*n)/(f-n), T(0)); }

} // end namespace vg::

#undef T // if used T as #define, undef it