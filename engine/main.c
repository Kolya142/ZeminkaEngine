#include <neweng/engine.h>

#include <stdlib.h>
#include <stdio.h>

static double getSystemTime() { // TODO: public functions
#if defined(_WIN32)
    natodo("MS Windows");
#elif defined(__APPLE__)
    natodo("Apple MacOSX");
#else
#include <time.h> // Stolen from RGFW and rewritten for floats
    struct timespec ts;
    const u64 scale_factor = 1000000000;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (f64)ts.tv_sec + (f64)ts.tv_nsec / (f64)scale_factor;
#endif
}

static double getDeltaTime() {
    static double latest = 0;
    double platest = latest;
    latest = getSystemTime();
    if (latest == 0)
        return 0;
    return latest-platest;
}

f64 NE_deltaTime = 0, NE_systemTime = 0;

typedef struct {
    NE_Vec2 pos, vel;
} BBall_Data;

static void *bball_alloc(NEnt_ent id) {
    BBall_Data *d = malloc(sizeof(BBall_Data));
    d->pos = NE_Vec2_From2(.1, .2);
    d->vel = NE_Vec2_From2(.3, .1);
    return d;
}

static void bball_onmsg(void *_ent, NEnt_ent ent_id, NEnt_ent caller, NEnt_Msg_Kind msg_kind, void *msg_data) {
    BBall_Data *ent = _ent;
    switch (msg_kind) {
    case NENT_MSG_UPDATE: {
        // MB i'll put something here.
    } break;
    case NENT_MSG_RENDER: {
        NScreen_DrawCircle(NE_Vec3_From3(ent->pos.x, ent->pos.y, 4.-4.*sqrt(ent->pos.x*ent->pos.x+ent->pos.y*ent->pos.y)), .01, NE_RED);
    } break;
    case NENT_MSG_30Hz_UPDATE: {
        ent->pos = NE_Vec2_Add(ent->pos, NE_Vec2_Scale(ent->vel, NE_deltaTime));
        const static double bs = .5f;
        if (ent->pos.x < -bs) {ent->pos.x = -bs;ent->vel.x *= -1;}
        if (ent->pos.x > bs) {ent->pos.x = bs;ent->vel.x *= -1;}
        if (ent->pos.y < -bs) {ent->pos.y = -bs;ent->vel.y *= -1;}
        if (ent->pos.y > bs) {ent->pos.y = bs;ent->vel.y *= -1;}
    } break;
    default: {
        natodo("WTF just now is happened???");
    } break;
    }
}

static void bball_dealloc(void *ent, NEnt_ent id) {
    free(ent);
}

NEnt_intf bball_intf = {
    bball_alloc,
    bball_dealloc,
    bball_onmsg
};

static const NE_Vertex player_model_v[] = {
    {-1, -1, -1},
    {-1, -1, +1},
    {-1, +1, +1},
    {-1, +1, -1},
    {+1, -1, -1},
    {+1, -1, +1},
    {+1, +1, +1},
    {+1, +1, -1}
};

static const NE_Color player_model_c[] = {
    NE_RED,
    NE_GREEN,
    NE_RED,
    NE_GREEN,
    NE_RED,
    NE_GREEN,
    NE_RED,
    NE_GREEN,
};

static const NE_Face player_model_f[] = {   
    {0, 1, 2},
    {2, 3, 0},
    
    {0, 3, 4},
    {4, 7, 3},
    
    {3, 7, 6},
    {6, 2, 3},

    
    {0+4, 1+4, 2+4},
    {2+4, 3+4, 0+4},

    
    {0+1, 3-1, 4+1},
    {4+1, 7-1, 3-1},
    
    {3-3, 7-3, 6-1},
    {6-1, 2-1, 3-3},
};

static const NE_Model player_model = {
    player_model_v,
    player_model_c,
    player_model_f,
    12
};

typedef struct {
    NE_Vec3 pos;
} Player_Data;

static void *player_alloc(NEnt_ent id) {
    Player_Data *d = malloc(sizeof(Player_Data));
    d->pos = NE_Vec3_From3(0,0,1.);
    return d;
}

static void player_onmsg(void *_ent, NEnt_ent ent_id, NEnt_ent caller, NEnt_Msg_Kind msg_kind, void *msg_data) {
    Player_Data *ent = _ent;
    switch (msg_kind) {
    case NENT_MSG_UPDATE: {
        // MB i'll put something here.
    } break;
    case NENT_MSG_RENDER: {
        // NScreen_DrawCircle(ent->pos, .01, NE_MYCOLOR);
        NE_TransformW tw = NE_Transform_Cache((NE_Transform) {
                ent->pos,
                {0,0,0},
                NE_Vec3_From1(.01),
            });
        NScreen_RenderModel(player_model, tw);
    } break;
    case NENT_MSG_30Hz_UPDATE: {
        NE_Vec3 vel = {0};
        if (NScreen_IsKeyDown(NE_KEY_w)) vel.y += 1.;
        if (NScreen_IsKeyDown(NE_KEY_a)) vel.x -= 1.;
        if (NScreen_IsKeyDown(NE_KEY_s)) vel.y -= 1.;
        if (NScreen_IsKeyDown(NE_KEY_d)) vel.x += 1.;
        if (NScreen_IsKeyDown(NE_KEY_q)) vel.z += 1.;
        if (NScreen_IsKeyDown(NE_KEY_e)) vel.z -= 1.;
        vel = NE_Vec3_Norm(vel);
        ent->pos = NE_Vec3_Add(ent->pos, NE_Vec3_Scale(vel, NE_deltaTime));
        const static double bs = .5f;
        if (ent->pos.x < -bs) ent->pos.x = -bs;
        if (ent->pos.x > bs) ent->pos.x = bs;
        if (ent->pos.y < -bs) ent->pos.y = -bs;
        if (ent->pos.y > bs) ent->pos.y = bs;
    } break;
    default: {
        natodo("WTF just now is happened???");
    } break;
    }
}

static void player_dealloc(void *ent, NEnt_ent id) {
    free(ent);
}

NEnt_intf player_intf = {
    player_alloc,
    player_dealloc,
    player_onmsg
};

int main() {
    NScreen_init(800, 600, "New Engine");

    NE_Vec3 pos = NE_Vec3_From3(0, 0, 1.);

    NEnt_add(bball_intf);
    NEnt_add(player_intf);
    
    while (NScreen_IsNtClosed()) {
        NE_deltaTime = getDeltaTime();
        NE_systemTime = getSystemTime();
        pos.y += NE_deltaTime*.13;
        pos.x += NE_deltaTime*.14;
        if (pos.x < -1.f) pos.x = 1.f;
        if (pos.x > 1.f) pos.x = -1.f;
        if (pos.y < -1.f) pos.y = 1.f;
        if (pos.y > 1.f) pos.y = -1.f;
        if (NScreen_IsKeyPressed(NE_KEY_r))
            NEnt_add(bball_intf);
        NScreen_BeginFrame();
        {
            bool dt = NScreen_GetDepthTest();
            NScreen_SetDepthTest(false);
            NE_Rotation r = NE_Rotation_From_Rad(NE_systemTime);
            NScreen_DrawTriangle_Ex(
                NE_Vec3_Add(pos, NE_Vec3_RotateXY(NE_Vec3_From2(0, .1f),r)),
                NE_Vec3_Add(pos, NE_Vec3_RotateXY(NE_Vec3_From2(-.1f, -.1f),r)),
                NE_Vec3_Add(pos, NE_Vec3_RotateXY(NE_Vec3_From2(.1f, -.1f),r)),
                NE_GREEN,
                NE_RED,
                NE_BLUE
                );
            NScreen_DrawCircle(
                pos,
                .03f,
                NE_BLACK
                );
            NScreen_DrawCircle(
                pos,
                .01f,
                NE_WHITE
                );
            NScreen_SetDepthTest(dt);
        }
        pos.z = sinf(NE_systemTime)*.5f+1.f;
        NEnt_update();
        NScreen_EndFrame();
    }
}

NE_Vec2 NE_Vec2_Rotate(NE_Vec2 v, NE_Rotation r) {return NE_Vec2_From2(v.x*r.cos-v.y*r.sin,v.x*r.sin+v.y*r.cos);}
NE_Vec3 NE_Vec3_RotateXY(NE_Vec3 v, NE_Rotation r) {
    f64 dx = v.x*r.cos-v.y*r.sin, dy = v.x*r.sin+v.y*r.cos;
    v.x = dx;
    v.y = dy;
    return v;
}
NE_Vec3 NE_Vec3_RotateYZ(NE_Vec3 v, NE_Rotation r) {
    f64 dy = v.y*r.cos-v.z*r.sin, dz = v.y*r.sin+v.z*r.cos;
    v.y = dy;
    v.z = dz;
    return v;
}
NE_Vec3 NE_Vec3_RotateXZ(NE_Vec3 v, NE_Rotation r) {
    f64 dx = v.x*r.cos-v.z*r.sin, dz = v.x*r.sin+v.z*r.cos;
    v.x = dx;
    v.z = dz;
    return v;
}

NE_Vec2 NE_Vec2_Add(NE_Vec2 a, NE_Vec2 b) {return NE_Vec2_From2(a.x+b.x,a.y+b.y);}
NE_Vec2 NE_Vec2_Sub(NE_Vec2 a, NE_Vec2 b) {return NE_Vec2_From2(a.x-b.x,a.y-b.y);}
NE_Vec2 NE_Vec2_Mul(NE_Vec2 a, NE_Vec2 b) {return NE_Vec2_From2(a.x*b.x,a.y*b.y);}
NE_Vec2 NE_Vec2_Div(NE_Vec2 a, NE_Vec2 b) {return NE_Vec2_From2(a.x/b.x,a.y/b.y);}
NE_Vec2 NE_Vec2_Scale(NE_Vec2 v, f64 s) {return NE_Vec2_From2(v.x*s,v.y*s);}

NE_Vec3 NE_Vec3_Add(NE_Vec3 a, NE_Vec3 b) {return NE_Vec3_From3(a.x+b.x,a.y+b.y,a.z+b.z);}
NE_Vec3 NE_Vec3_Sub(NE_Vec3 a, NE_Vec3 b) {return NE_Vec3_From3(a.x-b.x,a.y-b.y,a.z-b.z);}
NE_Vec3 NE_Vec3_Mul(NE_Vec3 a, NE_Vec3 b) {return NE_Vec3_From3(a.x*b.x,a.y*b.y,a.z*b.z);}
NE_Vec3 NE_Vec3_Div(NE_Vec3 a, NE_Vec3 b) {return NE_Vec3_From3(a.x/b.x,a.y/b.y,a.z/b.z);}
NE_Vec3 NE_Vec3_Scale(NE_Vec3 v, f64 s) {return NE_Vec3_From3(v.x*s,v.y*s,v.z*s);}

f64     NE_Vec3_MagSq(NE_Vec3 v) {return v.x*v.x+v.y*v.y+v.z*v.z;}
f64     NE_Vec3_Mag(NE_Vec3 v) {return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
NE_Vec3 NE_Vec3_Norm(NE_Vec3 v) {
    if (v.x == 0 && v.y == 0 && v.z == 0) return v;
    f64 m = NE_Vec3_Mag(v);
    return NE_Vec3_From3(v.x/m,v.y/m,v.z/m);
}

NE_Vec4 NE_Vec4_Add(NE_Vec4 a, NE_Vec4 b) {return NE_Vec4_From4(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);}
NE_Vec4 NE_Vec4_Sub(NE_Vec4 a, NE_Vec4 b) {return NE_Vec4_From4(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);}
NE_Vec4 NE_Vec4_Mul(NE_Vec4 a, NE_Vec4 b) {return NE_Vec4_From4(a.x*b.x,a.y*b.y,a.z*b.z,a.w*b.w);}
NE_Vec4 NE_Vec4_Div(NE_Vec4 a, NE_Vec4 b) {return NE_Vec4_From4(a.x/b.x,a.y/b.y,a.z/b.z,a.w/b.w);}
NE_Vec4 NE_Vec4_Scale(NE_Vec4 v, f64 s) {return NE_Vec4_From4(v.x*s,v.y*s,v.z*s,v.w*s);}

NE_TransformW NE_Transform_Cache(NE_Transform t) {
    return (NE_TransformW) {
        t.position,
        t.scale,
        NE_Rotation_From_Rad(t.rotation.z),
        NE_Rotation_From_Rad(t.rotation.y),
        NE_Rotation_From_Rad(t.rotation.x),
    };
}

NE_Vec3 NE_TransformW_Apply(NE_TransformW t, NE_Vec3 v) {
    // RST
    v = NE_Vec3_RotateYZ(v, t.cryz);
    v = NE_Vec3_RotateXZ(v, t.crxz);
    v = NE_Vec3_RotateXY(v, t.crxy);

    v = NE_Vec3_Mul(v, t.scale);
    
    v = NE_Vec3_Add(v, t.position);

    return v; 
}
