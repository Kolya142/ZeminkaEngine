#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

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

extern f64 NE_deltaTime, NE_systemTime;

#define PI (3.14159265358979323)
#define TAU (6.28318530717958646)

// Sorry, all Tsoding fans, for me not using la.h.

typedef struct {f64 x, y;} NE_Vec2;
typedef struct {f64 x, y, z;} NE_Vec3;
typedef struct {f64 x, y, z, w;} NE_Vec4;

typedef struct {f64 cos, sin;} NE_Rotation;

typedef NE_Vec3 NE_Vertex;

typedef struct {
    size_t a, b, c;
} NE_Face;

typedef struct {
    f64 // Why not?
    r, g, b, a;
} NE_Color;

typedef struct {
    NE_Vertex *verteces;
    NE_Color *colors; // 1 color per vertex.
    NE_Face *faces;
    // size_t vert_count; i think it'sn't something needed now.
    size_t face_count;
} NE_Model;

typedef struct {
    NE_Vec3 position, rotation, scale;
} NE_Transform;

typedef struct {
    NE_Vec3 position, scale;
    NE_Rotation crxy, crxz, cryz;
} NE_TransformW; // Cached version of NE_Transform. Shouldn't be modified if you doesn't understood what it does.

static inline NE_Vec2 NE_Vec2_From1(f64 x) {return (NE_Vec2){x,x};}
static inline NE_Vec2 NE_Vec2_From2(f64 x, f64 y) {return (NE_Vec2){x,y};}

static inline NE_Vec3 NE_Vec3_From1(f64 x) {return (NE_Vec3){x,x,x};}
static inline NE_Vec3 NE_Vec3_From2(f64 x, f64 y) {return (NE_Vec3){x,y,0};}
static inline NE_Vec3 NE_Vec3_From3(f64 x, f64 y, f64 z) {return (NE_Vec3){x,y,z};}

static inline NE_Vec4 NE_Vec4_From1(f64 x) {return (NE_Vec4){x,x,x,x};}
static inline NE_Vec4 NE_Vec4_From2(f64 x, f64 y) {return (NE_Vec4){x,y,0,0};}
static inline NE_Vec4 NE_Vec4_From3(f64 x, f64 y, f64 z) {return (NE_Vec4){x,y,z,0};}
static inline NE_Vec4 NE_Vec4_From4(f64 x, f64 y, f64 z, f64 w) {return (NE_Vec4){x,y,z,w};}

static inline NE_Rotation NE_Rotation_From_Rad(f64 a) {
    return (NE_Rotation) {cos(a), sin(a)};
}
NE_Vec2 NE_Vec2_Rotate(NE_Vec2 v, NE_Rotation r);
NE_Vec3 NE_Vec3_RotateXY(NE_Vec3 v, NE_Rotation r);
NE_Vec3 NE_Vec3_RotateYZ(NE_Vec3 v, NE_Rotation r);
NE_Vec3 NE_Vec3_RotateXZ(NE_Vec3 v, NE_Rotation r);

NE_Vec2 NE_Vec2_Add(NE_Vec2 a, NE_Vec2 b);
NE_Vec2 NE_Vec2_Sub(NE_Vec2 a, NE_Vec2 b);
NE_Vec2 NE_Vec2_Mul(NE_Vec2 a, NE_Vec2 b);
NE_Vec2 NE_Vec2_Div(NE_Vec2 a, NE_Vec2 b);
NE_Vec2 NE_Vec2_Scale(NE_Vec2 v, f64 s);

NE_Vec3 NE_Vec3_Add(NE_Vec3 a, NE_Vec3 b);
NE_Vec3 NE_Vec3_Sub(NE_Vec3 a, NE_Vec3 b);
NE_Vec3 NE_Vec3_Mul(NE_Vec3 a, NE_Vec3 b);
NE_Vec3 NE_Vec3_Div(NE_Vec3 a, NE_Vec3 b);
NE_Vec3 NE_Vec3_Scale(NE_Vec3 v, f64 s);

f64     NE_Vec3_MagSq(NE_Vec3 v);
f64     NE_Vec3_Mag(NE_Vec3 v);
NE_Vec3 NE_Vec3_Norm(NE_Vec3 v);

NE_Vec4 NE_Vec4_Add(NE_Vec4 a, NE_Vec4 b);
NE_Vec4 NE_Vec4_Sub(NE_Vec4 a, NE_Vec4 b);
NE_Vec4 NE_Vec4_Mul(NE_Vec4 a, NE_Vec4 b);
NE_Vec4 NE_Vec4_Div(NE_Vec4 a, NE_Vec4 b);
NE_Vec4 NE_Vec4_Scale(NE_Vec4 v, f64 s);

NE_TransformW NE_Transform_Cache(NE_Transform t);
NE_Vec3 NE_TransformW_Apply(NE_TransformW t, NE_Vec3 v);

#define natodo(c) {fprintf(stderr, ""__FILE__":%d: "c"\n", __LINE__);exit(1);}

