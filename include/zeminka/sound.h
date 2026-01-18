#pragma once
#include <zeminka/main.h>
// TODO: remove miniaudio include from this header
#include "../thirdparty/miniaudio.h"

typedef struct {
    ma_sound ms;
    f32 length;
    bool playing;
} ZESndSound;

void ZESndInit();
void ZESndUnLoad(ZESndSound *snd);
ZESndSound *ZESndLoad(const char *path);
void ZESndPlay(ZESndSound *snd);
void ZESndStop(ZESndSound *snd);
void ZESndSetVolume(ZESndSound *snd, f64 vol);
void ZESndSetPos(ZESndSound *snd, ZEVec3 pos);
void ZESndSetDir(ZESndSound *snd, ZEVec3 dir);
void ZESndSetVel(ZESndSound *snd, ZEVec3 vel);
