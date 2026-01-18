#pragma once
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef __STR
#define __STR0(x) #x
#define __STR(x) __STR0(x)
#endif

// Major - when backward compatibility is broken
// Minor - when forward compatibility is broken
// Patch - when neither backward compatibility nor forward compatibility is broken

#define ZEMINKAENG_VER_MAJOR 0
#define ZEMINKAENG_VER_MINOR 2
#define ZEMINKAENG_VER_PATCH 0
#define ZEMINKAENG_VER __STR(ZEMINKAENG_VER_MAJOR)"."__STR(ZEMINKAENG_VER_MINOR)"."__STR(ZEMINKAENG_VER_PATCH)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef uintptr_t uptr;
typedef intptr_t sptr;

extern f64 ZEdeltaTime, ZEdeltaTime30Hz, ZEsystemTime;

#ifndef _Nullable
#define _Nullable
#endif

#define PI (3.14159265358979323)
#define TAU (6.28318530717958646)

// I spent 30 minutes to find this value. Never ask me for Math.
#define GRAV (4./3.)

// Sorry, all Tsoding fans, for me not using la.h.

typedef struct {f64 x, y;} ZEVec2;
typedef struct {f64 x, y, z;} ZEVec3;
typedef struct {f64 x, y, z, w;} ZEVec4;

typedef struct {f64 cos, sin;} ZERotation;

typedef ZEVec3 ZEVertex;

typedef struct {
    size_t a, b, c;
} ZEFace;

typedef struct {
    f64 // Why not?
    r, g, b, a;
} ZEColor;

typedef struct {
    const ZEVertex *verteces;
    const ZEColor *colors; // 1 color per vertex.
    const ZEFace *faces;
    // size_t vert_count; i think it'sn't something needed now.
    size_t face_count;
} ZEModel;

typedef struct {
    ZEVec3 position, rotation, scale;
} ZETransform;

typedef struct {
    ZEVec3 position, scale;
    ZERotation crxy, crxz, cryz;
} ZETransformW; // Cached version of ZETransform. Shouldn't be modified if you doesn't understood what it does.

static inline ZEVec2 ZEVec2_From1(f64 x) {return (ZEVec2){x,x};}
static inline ZEVec2 ZEVec2_From2(f64 x, f64 y) {return (ZEVec2){x,y};}

static inline ZEVec3 ZEVec3_From1(f64 x) {return (ZEVec3){x,x,x};}
static inline ZEVec3 ZEVec3_From2(f64 x, f64 y) {return (ZEVec3){x,y,0};}
static inline ZEVec3 ZEVec3_From3(f64 x, f64 y, f64 z) {return (ZEVec3){x,y,z};}

static inline ZEVec4 ZEVec4_From1(f64 x) {return (ZEVec4){x,x,x,x};}
static inline ZEVec4 ZEVec4_From2(f64 x, f64 y) {return (ZEVec4){x,y,0,0};}
static inline ZEVec4 ZEVec4_From3(f64 x, f64 y, f64 z) {return (ZEVec4){x,y,z,0};}
static inline ZEVec4 ZEVec4_From4(f64 x, f64 y, f64 z, f64 w) {return (ZEVec4){x,y,z,w};}

static inline ZERotation ZERotation_From_Rad(f64 a) {
    return (ZERotation) {cos(a), sin(a)};
}
ZEVec2 ZEVec2_Rotate(ZEVec2 v, ZERotation r);
ZEVec3 ZEVec3_RotateXY(ZEVec3 v, ZERotation r);
ZEVec3 ZEVec3_RotateYZ(ZEVec3 v, ZERotation r);
ZEVec3 ZEVec3_RotateXZ(ZEVec3 v, ZERotation r);

ZEVec2 ZEVec2_Add(ZEVec2 a, ZEVec2 b);
ZEVec2 ZEVec2_Sub(ZEVec2 a, ZEVec2 b);
ZEVec2 ZEVec2_Mul(ZEVec2 a, ZEVec2 b);
ZEVec2 ZEVec2_Div(ZEVec2 a, ZEVec2 b);
ZEVec2 ZEVec2_Scale(ZEVec2 v, f64 s);

ZEVec3 ZEVec3_Add(ZEVec3 a, ZEVec3 b);
ZEVec3 ZEVec3_Sub(ZEVec3 a, ZEVec3 b);
ZEVec3 ZEVec3_Mul(ZEVec3 a, ZEVec3 b);
ZEVec3 ZEVec3_Div(ZEVec3 a, ZEVec3 b);
ZEVec3 ZEVec3_Scale(ZEVec3 v, f64 s);

f64     ZEVec3_MagSq(ZEVec3 v);
f64     ZEVec3_Mag(ZEVec3 v);
ZEVec3  ZEVec3_Norm(ZEVec3 v);
f64     ZEVec3_Dist(ZEVec3 a, ZEVec3 b);

ZEVec4 ZEVec4_Add(ZEVec4 a, ZEVec4 b);
ZEVec4 ZEVec4_Sub(ZEVec4 a, ZEVec4 b);
ZEVec4 ZEVec4_Mul(ZEVec4 a, ZEVec4 b);
ZEVec4 ZEVec4_Div(ZEVec4 a, ZEVec4 b);
ZEVec4 ZEVec4_Scale(ZEVec4 v, f64 s);

ZETransformW ZETransform_Cache(ZETransform t);
ZEVec3 ZETransformW_Apply(ZETransformW t, ZEVec3 v);

#define zetodo(c) {fprintf(stderr, ""__FILE__":%d: "c"\n", __LINE__);exit(1);}

typedef enum {
    ZELOG_INFO,
    ZELOG_TODO,
    ZELOG_WARNING,
    ZELOG_ERROR,
    ZELOG_FATAL  // Automaticly stops the program when reported.
} ZELogLevel;


void ZELog(ZELogLevel ll, const char *fmt, ...);
