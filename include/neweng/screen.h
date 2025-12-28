#pragma once
#include <neweng/main.h>

typedef enum {
    NE_KEY_keyNULL = 0,
    NE_KEY_escape = '\033',
    NE_KEY_backtick = '`',
    NE_KEY_0 = '0',
    NE_KEY_1 = '1',
    NE_KEY_2 = '2',
    NE_KEY_3 = '3',
    NE_KEY_4 = '4',
    NE_KEY_5 = '5',
    NE_KEY_6 = '6',
    NE_KEY_7 = '7',
    NE_KEY_8 = '8',
    NE_KEY_9 = '9',
    NE_KEY_minus = '-',
    NE_KEY_equal = '=',
    NE_KEY_equals = NE_KEY_equal,
    NE_KEY_backSpace = '\b',
    NE_KEY_tab = '\t',
    NE_KEY_space = ' ',
    NE_KEY_a = 'a',
    NE_KEY_b = 'b',
    NE_KEY_c = 'c',
    NE_KEY_d = 'd',
    NE_KEY_e = 'e',
    NE_KEY_f = 'f',
    NE_KEY_g = 'g',
    NE_KEY_h = 'h',
    NE_KEY_i = 'i',
    NE_KEY_j = 'j',
    NE_KEY_k = 'k',
    NE_KEY_l = 'l',
    NE_KEY_m = 'm',
    NE_KEY_n = 'n',
    NE_KEY_o = 'o',
    NE_KEY_p = 'p',
    NE_KEY_q = 'q',
    NE_KEY_r = 'r',
    NE_KEY_s = 's',
    NE_KEY_t = 't',
    NE_KEY_u = 'u',
    NE_KEY_v = 'v',
    NE_KEY_w = 'w',
    NE_KEY_x = 'x',
    NE_KEY_y = 'y',
    NE_KEY_z = 'z',
    NE_KEY_period = '.',
    NE_KEY_comma = ',',
    NE_KEY_slash = '/',
    NE_KEY_bracket = '[',
    NE_KEY_closeBracket = ']',
    NE_KEY_semicolon = ';',
    NE_KEY_apostrophe = '\'',
    NE_KEY_backSlash = '\\',
    NE_KEY_return = '\n',
    NE_KEY_enter = NE_KEY_return,
    NE_KEY_delete = '\177', /* 127 */
    NE_KEY_F1,
    NE_KEY_F2,
    NE_KEY_F3,
    NE_KEY_F4,
    NE_KEY_F5,
    NE_KEY_F6,
    NE_KEY_F7,
    NE_KEY_F8,
    NE_KEY_F9,
    NE_KEY_F10,
    NE_KEY_F11,
    NE_KEY_F12,
    NE_KEY_F13,
    NE_KEY_F14,
    NE_KEY_F15,
    NE_KEY_F16,
    NE_KEY_F17,
    NE_KEY_F18,
    NE_KEY_F19,
    NE_KEY_F20,
    NE_KEY_F21,
    NE_KEY_F22,
    NE_KEY_F23,
    NE_KEY_F24,
    NE_KEY_F25,
    NE_KEY_capsLock,
    NE_KEY_shiftL,
    NE_KEY_controlL,
    NE_KEY_altL,
    NE_KEY_superL,
    NE_KEY_shiftR,
    NE_KEY_controlR,
    NE_KEY_altR,
    NE_KEY_superR,
    NE_KEY_up,
    NE_KEY_down,
    NE_KEY_left,
    NE_KEY_right,
    NE_KEY_insert,
    NE_KEY_menu,
    NE_KEY_end,
    NE_KEY_home,
    NE_KEY_pageUp,
    NE_KEY_pageDown,
    NE_KEY_numLock,
    NE_KEY_kpSlash,
    NE_KEY_kpMultiply,
    NE_KEY_kpPlus,
    NE_KEY_kpMinus,
    NE_KEY_kpEqual,
    NE_KEY_kpEquals = NE_KEY_kpEqual,
    NE_KEY_kp1,
    NE_KEY_kp2,
    NE_KEY_kp3,
    NE_KEY_kp4,
    NE_KEY_kp5,
    NE_KEY_kp6,
    NE_KEY_kp7,
    NE_KEY_kp8,
    NE_KEY_kp9,
    NE_KEY_kp0,
    NE_KEY_kpPeriod,
    NE_KEY_kpReturn,
    NE_KEY_scrollLock,
    NE_KEY_printScreen,
    NE_KEY_pause,
    NE_KEY_world1,
    NE_KEY_world2,
    NE_KEY_keyLast = 256
} NE_Key; // Sorry, RGFW authors.


void NScreen_init(u32 width, u32 height, const char *title);

bool NScreen_IsClosed();
bool NScreen_IsNtClosed(); // Finally isn't closed function!

void NScreen_BeginFrame();
void NScreen_EndFrame();

void NScreen_DrawCircle(NE_Vec3 o, f64 r, NE_Color col);
void NScreen_DrawTriangle(NE_Vec3 a, NE_Vec3 b, NE_Vec3 c, NE_Color col);
void NScreen_DrawTriangle_Ex(NE_Vec3 a, NE_Vec3 b, NE_Vec3 c, NE_Color a_c, NE_Color b_c, NE_Color c_c);

bool NScreen_IsKeyPressed(NE_Key key);
bool NScreen_IsKeyDown(NE_Key key);
bool NScreen_IsKeyReleased(NE_Key key);

bool NScreen_GetDepthTest();
void NScreen_SetDepthTest(bool test);

void NScreen_RenderModel(NE_Model model, NE_TransformW transform);

void *NScreen_GetSystemHandler();

#define NE_RED ((NE_Color){1.,0,0,1.})
#define NE_MAGENTA ((NE_Color){1.,0,1.,1.})
#define NE_MYCOLOR ((NE_Color){.894,.878,0,1.})
#define NE_GREEN ((NE_Color){0,1.,0,1.})
#define NE_BLUE ((NE_Color){0,0,1.,1.})
#define NE_BLACK ((NE_Color){0,0,0,1.})
#define NE_WHITE ((NE_Color){1.,1.,1.,1.})
