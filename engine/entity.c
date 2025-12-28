#include <neweng/engine.h>
#include <stdio.h>

typedef struct {
    void *data;
    bool is_freeless;
    NEnt_intf intf;
} Ent__Data;

Ent__Data *ent_arr = NULL;
size_t ent_count = 0, ent_cap = 0;

NEnt_ent NEnt_add(NEnt_intf intf) {
    size_t pos = 0;
    
    if (!ent_arr) {
        ent_cap = 1024;
        ent_count = 0;
        ent_arr = malloc(sizeof(Ent__Data)*ent_cap);
    }
    if (ent_count + 1 >= ent_cap) {
        for (size_t i = 0; i < ent_count; ++i) {
            if (!ent_arr[i].is_freeless) {
                pos = i;
                goto append;
            }
        }
        ent_cap <<= 1;
        ent_arr = realloc(ent_arr, sizeof(Ent__Data)*ent_cap);
    }
    pos = ent_count++;
  append:
    ent_arr[pos].data = intf.alloc(pos);
    ent_arr[pos].is_freeless = true;
    ent_arr[pos].intf = intf;
    return pos;
}

void NEnt_update(void) {
    bool isFixed = ((s32)(NE_deltaTime*10.)) % 3 == 0;
    for (size_t i = 0; i < ent_count; ++i) {
        if (ent_arr[i].is_freeless) {
            ent_arr[i].intf.onmsg(ent_arr[i].data, i, -1, NENT_MSG_UPDATE, NULL);
            if (isFixed) {
                ent_arr[i].intf.onmsg(ent_arr[i].data, i, -1, NENT_MSG_30Hz_UPDATE, NULL);
            }
            ent_arr[i].intf.onmsg(ent_arr[i].data, i, -1, NENT_MSG_RENDER, NULL);
        }
    }
}

void NEnt_send(NEnt_Msg_Kind msg_kind, NEnt_ent callee, void *msg_data) {

}

