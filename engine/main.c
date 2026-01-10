#include <zeminka/engine.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

static double getSystemTime() { // TODO: public functions
#if defined(_WIN32)
    FILETIME system_time;
    ULARGE_INTEGER large;

    GetSystemTimePreciseAsFileTime(&system_time);
    large.u.LowPart = system_time.dwLowDateTime;
    large.u.HighPart = system_time.dwHighDateTime;
    const u64 scale_factor = 1000000;
    u64 q = large.QuadPart/10;
    return (f64)q/(f64)scale_factor;
#elif defined(__APPLE__)
    zetodo("Apple MacOSX");
#else
#include <time.h> // Stolen from RGFW and rewrotten with floats.
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

#define INTF_BBALL 0
#define INTF_PLAYER 1

f64 ZEdeltaTime = 0, ZEdeltaTime30Hz = 0, ZEsystemTime = 0;

typedef struct {
    ZEVec2 pos, vel;
} BBall_Data;

static void *bball_alloc(ZEEnt_ent id) {
    BBall_Data *d = malloc(sizeof(BBall_Data));
    d->pos = ZEVec2_From2(.1, .2);
    d->vel = ZEVec2_From2(.3, .1);
    return d;
}

static void bball_onmsg(void *_ent, ZEEnt_ent ent_id, ZEEnt_ent caller, ZEEnt_Msg_Kind msg_kind, void *msg_data) {
    BBall_Data *ent = _ent;
    switch (msg_kind) {
    case ZEENT_MSG_UPDATE: {
        // MB i'll put something here.
    } break;
    case ZEENT_MSG_RENDER: {
        ZEScreen_DrawCircle(ZEVec3_From3(ent->pos.x, ent->pos.y, 4.-4.*sqrt(ent->pos.x*ent->pos.x+ent->pos.y*ent->pos.y)), .01, ZERED);
    } break;
    case ZEENT_MSG_30Hz_UPDATE: {
        ent->pos = ZEVec2_Add(ent->pos, ZEVec2_Scale(ent->vel, ZEdeltaTime));
        const static double bs = .5f;
        if (ent->pos.x < -bs) {ent->pos.x = -bs;ent->vel.x *= -1;}
        if (ent->pos.x > bs) {ent->pos.x = bs;ent->vel.x *= -1;}
        if (ent->pos.y < -bs) {ent->pos.y = -bs;ent->vel.y *= -1;}
        if (ent->pos.y > bs) {ent->pos.y = bs;ent->vel.y *= -1;}
    } break;
    case ZEENT_MSG_USR1: {
        memcpy(&ent->pos, msg_data, sizeof(ent->pos));
    } break;
    default: {
        zetodo("WTF just now is happened???");
    } break;
    }
}

static void bball_dealloc(void *ent, ZEEnt_ent id) {
    free(ent);
}

ZEEnt_intf bball_intf = {
    bball_alloc,
    bball_dealloc,
    bball_onmsg,
    INTF_BBALL
};

static const ZEVertex player_model_v[] = {
    {-1, -1, -1},
    {-1, -1, +1},
    {-1, +1, +1},
    {-1, +1, -1},
    {+1, -1, -1},
    {+1, -1, +1},
    {+1, +1, +1},
    {+1, +1, -1}
};

static const ZEColor player_model_c[] = {
    ZERED_,
    ZEMAGENTA_,
    ZEGREEN_,
    ZECYAN_,
    ZEBLUE_,
    ZEBLACK_,
    ZEWHITE_,
    ZEORANGE_
};

static const ZEFace player_model_f[] = {   
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

static const ZEModel player_model = {
    player_model_v,
    player_model_c,
    player_model_f,
    12 // 6 rectangles and every rectangle is 2 triangle
};

typedef struct {
    ZEVec3 pos, vel;
} Player_Data;

static void *player_alloc(ZEEnt_ent id) {
    Player_Data *d = malloc(sizeof(Player_Data));
    d->pos = ZEVec3_From3(0,0,1.);
    d->vel = ZEVec3_From3(0,0,0);
    return d;
}

static void player_onmsg(void *_ent, ZEEnt_ent ent_id, ZEEnt_ent caller, ZEEnt_Msg_Kind msg_kind, void *msg_data) {
    Player_Data *ent = _ent;
    switch (msg_kind) {
    case ZEENT_MSG_UPDATE: {
        if (ent->pos.y <= -5) ZEEnt_destroy(ent_id);
    } break;
    case ZEENT_MSG_RENDER: {
        // ZEScreen_DrawCircle(ent->pos, .01, ZEMYCOLOR);
        ZETransformW tw = ZETransform_Cache((ZETransform) {
                ent->pos,
                {0,ZEsystemTime,0},
                ZEVec3_From1(.1),
            });
        ZEScreen_RenderModel(player_model, tw);
    } break;
    case ZEENT_MSG_30Hz_UPDATE: {
        ent->pos = ZEVec3_Add(ent->pos, ZEVec3_Scale(ent->vel, ZEdeltaTime));
        ent->vel.y -= GRAV*ZEdeltaTime;
        ent->vel = ZEVec3_Add(ent->vel, ZEVec3_Scale(ent->vel, ZEdeltaTime*(.997-1.)));
        if (ent->pos.y < -2.1) {
            ent->vel.y *= -1;
            ent->vel.x *= .8;
            ent->vel.z *= .8;
            ent->pos.y = -2.1;
        }
        for (ZEEnt_ent id = ent_id+1;;++id) {
            s32 iid = ZEEnt_get_intfid(id);
            if (iid == -1) break;
            if (iid == INTF_PLAYER) { // TODO: i don't know to find collisions between two rotated cubes.
                Player_Data other_ent;
                ZEEnt_send(ZEENT_MSG_USR2, id, &other_ent);
                f64 d = ZEVec3_Mag(ZEVec3_Sub(ent->pos, other_ent.pos));
                ZEVec3 z = ZEVec3_Scale(ZEVec3_Add(ent->pos, other_ent.pos), .5);
                if (d <= .17) {
                    const float m1 = 1, m2 = 1;
                    ZEVec3 v1 = ent->vel, v2 = other_ent.vel;
                    printf("%d(%2.2f;%2.2f;%2.2f)\n", ent_id, ent->pos.x, ent->pos.y, ent->pos.z);
                    printf("%d(%2.2f;%2.2f;%2.2f)\n", id, other_ent.pos.x, other_ent.pos.y, other_ent.pos.z);
                    ZEPhysics_cmsolver(m1, &v1, m2, &v2, ZEVec3_Sub(ent->pos, z), ZEVec3_Sub(other_ent.pos, z));
                    ent->vel = v1;
                    other_ent.vel = v2;
                    ZEEnt_send(ZEENT_MSG_USR1, id, &other_ent);
                }
            }
        }
        // ent->pos = ZEVec3_From3(cos(ZEsystemTime),.5*sin(2*ZEsystemTime), 1.);
    } break;
    case ZEENT_MSG_USR1: {
        memcpy(ent, msg_data, sizeof(*ent));
    } break;
    case ZEENT_MSG_USR2: {
        memcpy(msg_data, ent, sizeof(*ent));
    } break;
    default: {
        zetodo("WTF just now is happened???");
    } break;
    }
}

static void player_dealloc(void *ent, ZEEnt_ent id) {
    free(ent);
}

ZEEnt_intf player_intf = {
    player_alloc,
    player_dealloc,
    player_onmsg,
    INTF_PLAYER
};

f64 ZEmousedX = 0, ZEmousedY = 0;

int main() {
    ZEScreen_init(1280, 720, 120., "Zeminka engine v" ZEMINKAENG_VER " test code");

    ZEVec3 pos = ZEVec3_From3(0, 0, 1.);
    
    ZEEnt_add(bball_intf);
    
    ZEEnt_add(player_intf);
    
    f64 c_yaw = 0;
    f64 c_pitch = 0;
    ZEVec3 c_pos = {0};
    
    while (ZEScreen_IsNtClosed()) {
        ZEdeltaTime = getDeltaTime();
        ZEsystemTime = getSystemTime();
        pos.y += ZEdeltaTime*.13;
        pos.x += ZEdeltaTime*.14;
        if (pos.x < -1.f) pos.x = 1.f;
        if (pos.x > 1.f) pos.x = -1.f;
        if (pos.y < -1.f) pos.y = 1.f;
        if (pos.y > 1.f) pos.y = -1.f;
        if (ZEScreen_IsKeyPressed(ZEKEY_r))
            ZEEnt_add(bball_intf);
        ZEScreen_BeginFrame(&ZEmousedX, &ZEmousedY);
        {
            ZEVec3 vel = {0};
            if (ZEScreen_IsKeyDown(ZEKEY_w)) vel.z += 1.;
            if (ZEScreen_IsKeyDown(ZEKEY_a)) vel.x -= 1.;
            if (ZEScreen_IsKeyDown(ZEKEY_s)) vel.z -= 1.;
            if (ZEScreen_IsKeyDown(ZEKEY_d)) vel.x += 1.;
            if (ZEScreen_IsKeyDown(ZEKEY_q)) vel.y += 1.;
            if (ZEScreen_IsKeyDown(ZEKEY_e)) vel.y -= 1.;
            vel = ZEVec3_Norm(vel);
            vel = ZEVec3_Scale(vel, ZEdeltaTime);
            vel = ZEVec3_Scale(vel, .5);
            if (ZEScreen_IsKeyDown(ZEKEY_altL)) vel = ZEVec3_Scale(vel, .15);
            if (ZEScreen_IsKeyDown(ZEKEY_shiftL)) vel = ZEVec3_Scale(vel, 4.);
            ZETransformW tw = ZETransform_Cache((ZETransform) {{0}, {-c_pitch, c_yaw, 0}, {1,1,1}});
            vel = ZETransformW_Apply(tw, vel);
            c_pos = ZEVec3_Add(vel, c_pos);
        }
        if (ZEScreen_IsKeyPressed(ZEKEY_z)) {
            Player_Data pd;
            ZETransformW tw = ZETransform_Cache((ZETransform) {{0}, {-c_pitch, c_yaw, 0}, {1,1,1}});
            ZEVec3 fd = ZETransformW_Apply(tw, ZEVec3_From3(0,0,1));
            pd.pos = ZEVec3_Add(c_pos, ZEVec3_Scale(fd, .4));
            pd.vel = ZEVec3_Scale(fd, .8);
            ZEEnt_send(ZEENT_MSG_USR1, ZEEnt_add(player_intf), &pd);
        }
        if (ZEScreen_IsKeyDown(ZEKEY_up))
            c_pitch += ZEdeltaTime;
        if (ZEScreen_IsKeyDown(ZEKEY_down))
            c_pitch -= ZEdeltaTime;
        if (ZEScreen_IsKeyDown(ZEKEY_left))
            c_yaw += ZEdeltaTime;
        if (ZEScreen_IsKeyDown(ZEKEY_right))
            c_yaw -= ZEdeltaTime;
        // c_pitch -= ZEmousedY;
        // c_yaw -= ZEmousedX;
        ZEScreen_RotateCamera(c_yaw, c_pitch, 0);
        ZEScreen_TranslateCamera(c_pos);
        {
            ZETransformW tw = ZETransform_Cache((ZETransform) {
                    {0,-2,0},
                    {0,0,PI},
                    {100, .1, 100}
                });
            ZEScreen_RenderModel(player_model, tw);
        }
        ZERotation r = ZERotation_From_Rad(ZEsystemTime);
        ZEScreen_DrawTriangle_Ex(
            ZEVec3_Add(pos, ZEVec3_RotateXY(ZEVec3_From2(0, .1f),r)),
            ZEVec3_Add(pos, ZEVec3_RotateXZ(ZEVec3_From2(-.1f, -.1f),r)),
            ZEVec3_Add(pos, ZEVec3_RotateXZ(ZEVec3_From2(.1f, -.1f),r)),
            ZEGREEN,
            ZERED,
            ZEBLUE
            );
        pos.z = sinf(ZEsystemTime)*.5f+1.f;
        ZEEnt_update();
        ZEScreen_EndFrame();
    }
}

ZEVec2 ZEVec2_Rotate(ZEVec2 v, ZERotation r) {return ZEVec2_From2(v.x*r.cos-v.y*r.sin,v.x*r.sin+v.y*r.cos);}
ZEVec3 ZEVec3_RotateXY(ZEVec3 v, ZERotation r) {
    f64 dx = v.x*r.cos-v.y*r.sin, dy = v.x*r.sin+v.y*r.cos;
    v.x = dx;
    v.y = dy;
    return v;
}
ZEVec3 ZEVec3_RotateYZ(ZEVec3 v, ZERotation r) {
    f64 dy = v.y*r.cos-v.z*r.sin, dz = v.y*r.sin+v.z*r.cos;
    v.y = dy;
    v.z = dz;
    return v;
}
ZEVec3 ZEVec3_RotateXZ(ZEVec3 v, ZERotation r) {
    f64 dx = v.x*r.cos-v.z*r.sin, dz = v.x*r.sin+v.z*r.cos;
    v.x = dx;
    v.z = dz;
    return v;
}

ZEVec2 ZEVec2_Add(ZEVec2 a, ZEVec2 b) {return ZEVec2_From2(a.x+b.x,a.y+b.y);}
ZEVec2 ZEVec2_Sub(ZEVec2 a, ZEVec2 b) {return ZEVec2_From2(a.x-b.x,a.y-b.y);}
ZEVec2 ZEVec2_Mul(ZEVec2 a, ZEVec2 b) {return ZEVec2_From2(a.x*b.x,a.y*b.y);}
ZEVec2 ZEVec2_Div(ZEVec2 a, ZEVec2 b) {return ZEVec2_From2(a.x/b.x,a.y/b.y);}
ZEVec2 ZEVec2_Scale(ZEVec2 v, f64 s) {return ZEVec2_From2(v.x*s,v.y*s);}

ZEVec3 ZEVec3_Add(ZEVec3 a, ZEVec3 b) {return ZEVec3_From3(a.x+b.x,a.y+b.y,a.z+b.z);}
ZEVec3 ZEVec3_Sub(ZEVec3 a, ZEVec3 b) {return ZEVec3_From3(a.x-b.x,a.y-b.y,a.z-b.z);}
ZEVec3 ZEVec3_Mul(ZEVec3 a, ZEVec3 b) {return ZEVec3_From3(a.x*b.x,a.y*b.y,a.z*b.z);}
ZEVec3 ZEVec3_Div(ZEVec3 a, ZEVec3 b) {return ZEVec3_From3(a.x/b.x,a.y/b.y,a.z/b.z);}
ZEVec3 ZEVec3_Scale(ZEVec3 v, f64 s) {return ZEVec3_From3(v.x*s,v.y*s,v.z*s);}

f64    ZEVec3_MagSq(ZEVec3 v) {return v.x*v.x+v.y*v.y+v.z*v.z;}
f64    ZEVec3_Mag(ZEVec3 v) {return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
ZEVec3 ZEVec3_Norm(ZEVec3 v) {
    if (v.x == 0 && v.y == 0 && v.z == 0) return v;
    f64 m = ZEVec3_Mag(v);
    return ZEVec3_From3(v.x/m,v.y/m,v.z/m);
}

ZEVec4 ZEVec4_Add(ZEVec4 a, ZEVec4 b) {return ZEVec4_From4(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);}
ZEVec4 ZEVec4_Sub(ZEVec4 a, ZEVec4 b) {return ZEVec4_From4(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);}
ZEVec4 ZEVec4_Mul(ZEVec4 a, ZEVec4 b) {return ZEVec4_From4(a.x*b.x,a.y*b.y,a.z*b.z,a.w*b.w);}
ZEVec4 ZEVec4_Div(ZEVec4 a, ZEVec4 b) {return ZEVec4_From4(a.x/b.x,a.y/b.y,a.z/b.z,a.w/b.w);}
ZEVec4 ZEVec4_Scale(ZEVec4 v, f64 s) {return ZEVec4_From4(v.x*s,v.y*s,v.z*s,v.w*s);}

ZETransformW ZETransform_Cache(ZETransform t) {
    return (ZETransformW) {
        t.position,
        t.scale,
        ZERotation_From_Rad(t.rotation.z),
        ZERotation_From_Rad(t.rotation.y),
        ZERotation_From_Rad(t.rotation.x),
    };
}

ZEVec3 ZETransformW_Apply(ZETransformW t, ZEVec3 v) {
    // RST
    v = ZEVec3_RotateYZ(v, t.cryz);
    v = ZEVec3_RotateXZ(v, t.crxz);
    v = ZEVec3_RotateXY(v, t.crxy);

    v = ZEVec3_Mul(v, t.scale);
    
    v = ZEVec3_Add(v, t.position);

    return v; 
}
