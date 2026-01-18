#include <zeminka/sound.h>
#define MA_IMPLEMENTATION
#include "../thirdparty/miniaudio.h"
#include <stdio.h>

static ma_engine audio_engine;

void ZESndInit() {
    if (ma_engine_init(NULL, &audio_engine) != MA_SUCCESS) {
        ZELog(ZELOG_FATAL, "Failed to initialize miniaudio audio engine.");
        exit(1);
    }
}

void ZESndUnLoad(ZESndSound *snd) {
    free(snd);
}

ZESndSound *ZESndLoad(const char *path) {
    ZESndSound *snd = malloc(sizeof(ma_sound));
    ma_sound_init_from_file(&audio_engine, path, 0, NULL, NULL, &snd->ms);
    ma_sound_get_length_in_seconds(&snd->ms, &snd->length);
    return snd;
}

void ZESndPlay(ZESndSound *snd) {
    ma_sound_start(&snd->ms);
}

void ZESndStop(ZESndSound *snd) {
    ma_sound_stop(&snd->ms);
}

void ZESndSetVolume(ZESndSound *snd, f64 vol) {
    ma_sound_set_volume(&snd->ms, vol);
}

void ZESndSetPos(ZESndSound *snd, ZEVec3 pos) {
    ma_sound_set_position(&snd->ms, pos.x, pos.y, pos.z);
}

void ZESndSetDir(ZESndSound *snd, ZEVec3 dir) {
    ma_sound_set_direction(&snd->ms, dir.x, dir.y, dir.z);
}

void ZESndSetVel(ZESndSound *snd, ZEVec3 vel) {
    ma_sound_set_velocity(&snd->ms, vel.x, vel.y, vel.z);
}
