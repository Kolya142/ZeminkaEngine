#pragma once
#include <zeminka/main.h>

typedef enum {
    ZEKEY_keyNULL = 0,
    ZEKEY_escape = '\033',
    ZEKEY_backtick = '`',
    ZEKEY_0 = '0',
    ZEKEY_1 = '1',
    ZEKEY_2 = '2',
    ZEKEY_3 = '3',
    ZEKEY_4 = '4',
    ZEKEY_5 = '5',
    ZEKEY_6 = '6',
    ZEKEY_7 = '7',
    ZEKEY_8 = '8',
    ZEKEY_9 = '9',
    ZEKEY_minus = '-',
    ZEKEY_equal = '=',
    ZEKEY_equals = ZEKEY_equal,
    ZEKEY_backSpace = '\b',
    ZEKEY_tab = '\t',
    ZEKEY_space = ' ',
    ZEKEY_a = 'a',
    ZEKEY_b = 'b',
    ZEKEY_c = 'c',
    ZEKEY_d = 'd',
    ZEKEY_e = 'e',
    ZEKEY_f = 'f',
    ZEKEY_g = 'g',
    ZEKEY_h = 'h',
    ZEKEY_i = 'i',
    ZEKEY_j = 'j',
    ZEKEY_k = 'k',
    ZEKEY_l = 'l',
    ZEKEY_m = 'm',
    ZEKEY_n = 'n',
    ZEKEY_o = 'o',
    ZEKEY_p = 'p',
    ZEKEY_q = 'q',
    ZEKEY_r = 'r',
    ZEKEY_s = 's',
    ZEKEY_t = 't',
    ZEKEY_u = 'u',
    ZEKEY_v = 'v',
    ZEKEY_w = 'w',
    ZEKEY_x = 'x',
    ZEKEY_y = 'y',
    ZEKEY_z = 'z',
    ZEKEY_period = '.',
    ZEKEY_comma = ',',
    ZEKEY_slash = '/',
    ZEKEY_bracket = '[',
    ZEKEY_closeBracket = ']',
    ZEKEY_semicolon = ';',
    ZEKEY_apostrophe = '\'',
    ZEKEY_backSlash = '\\',
    ZEKEY_return = '\n',
    ZEKEY_enter = ZEKEY_return,
    ZEKEY_delete = '\177', /* 127 */
    ZEKEY_F1,
    ZEKEY_F2,
    ZEKEY_F3,
    ZEKEY_F4,
    ZEKEY_F5,
    ZEKEY_F6,
    ZEKEY_F7,
    ZEKEY_F8,
    ZEKEY_F9,
    ZEKEY_F10,
    ZEKEY_F11,
    ZEKEY_F12,
    ZEKEY_F13,
    ZEKEY_F14,
    ZEKEY_F15,
    ZEKEY_F16,
    ZEKEY_F17,
    ZEKEY_F18,
    ZEKEY_F19,
    ZEKEY_F20,
    ZEKEY_F21,
    ZEKEY_F22,
    ZEKEY_F23,
    ZEKEY_F24,
    ZEKEY_F25,
    ZEKEY_capsLock,
    ZEKEY_shiftL,
    ZEKEY_controlL,
    ZEKEY_altL,
    ZEKEY_superL,
    ZEKEY_shiftR,
    ZEKEY_controlR,
    ZEKEY_altR,
    ZEKEY_superR,
    ZEKEY_up,
    ZEKEY_down,
    ZEKEY_left,
    ZEKEY_right,
    ZEKEY_insert,
    ZEKEY_menu,
    ZEKEY_end,
    ZEKEY_home,
    ZEKEY_pageUp,
    ZEKEY_pageDown,
    ZEKEY_numLock,
    ZEKEY_kpSlash,
    ZEKEY_kpMultiply,
    ZEKEY_kpPlus,
    ZEKEY_kpMinus,
    ZEKEY_kpEqual,
    ZEKEY_kpEquals = ZEKEY_kpEqual,
    ZEKEY_kp1,
    ZEKEY_kp2,
    ZEKEY_kp3,
    ZEKEY_kp4,
    ZEKEY_kp5,
    ZEKEY_kp6,
    ZEKEY_kp7,
    ZEKEY_kp8,
    ZEKEY_kp9,
    ZEKEY_kp0,
    ZEKEY_kpPeriod,
    ZEKEY_kpReturn,
    ZEKEY_scrollLock,
    ZEKEY_printScreen,
    ZEKEY_pause,
    ZEKEY_world1,
    ZEKEY_world2,
    ZEKEY_keyLast = 256
} ZEKey; // Sorry, RGFW authors.

#define DEG2RAD (PI/180)
#define RAD2DEG (180/PI)

void ZEScreen_init(u32 width, u32 height, f64 fov, const char *title);

bool ZEScreen_IsClosed();
bool ZEScreen_IsNtClosed(); // Finally isn't closed function!

void ZEScreen_BeginFrame(f64 *_Nullable mdx, f64 *_Nullable mdy); // mouse delta, not an absolute position.
void ZEScreen_TranslateCamera(ZEVec3 origin);
void ZEScreen_RotateCamera(f64 yaw, f64 pitch, f64 roll); // YZ,XZ,XY
void ZEScreen_EndFrame();

void ZEScreen_DrawCircle(ZEVec3 o, f64 r, ZEColor col);
void ZEScreen_DrawTriangle(ZEVec3 a, ZEVec3 b, ZEVec3 c, ZEColor col);
void ZEScreen_DrawTriangle_Ex(ZEVec3 a, ZEVec3 b, ZEVec3 c, ZEColor a_c, ZEColor b_c, ZEColor c_c);

bool ZEScreen_IsKeyPressed(ZEKey key);
bool ZEScreen_IsKeyDown(ZEKey key);
bool ZEScreen_IsKeyReleased(ZEKey key);

bool ZEScreen_GetDepthTest();
void ZEScreen_SetDepthTest(bool test);

void ZEScreen_RenderModel(ZEModel model, ZETransformW transform);

void *ZEScreen_GetSystemHandler();

#define ZERED ((ZEColor){1.,0,0,1.})
#define ZEMAGENTA ((ZEColor){1.,0,1.,1.})
#define ZEMYCOLOR ((ZEColor){.894,.878,0,1.})
#define ZEGREEN ((ZEColor){0,1.,0,1.})
#define ZECYAN ((ZEColor){0,1.,1.,1.})
#define ZEBLUE ((ZEColor){0,0,1.,1.})
#define ZEBLACK ((ZEColor){0,0,0,1.})
#define ZEWHITE ((ZEColor){1.,1.,1.,1.})
#define ZEGRAY ((ZEColor){.5,.5,.5,1.})
#define ZEORANGE ((ZEColor){1.,.5,0,1.})

// Small hack for the MSVC compiler.
#define ZERED_ {1.,0,0,1.}
#define ZEMAGENTA_ {1.,0,1.,1.}
#define ZEMYCOLOR_ {.894,.878,0,1.}
#define ZEGREEN_ {0,1.,0,1.}
#define ZECYAN_ {0,1.,1.,1.}
#define ZEBLUE_ {0,0,1.,1.}
#define ZEBLACK_ {0,0,0,1.}
#define ZEWHITE_ {1.,1.,1.,1.}
#define ZEGRAY_ {.5,.5,.5,1.}
#define ZEORANGE_ {1.,.5,0,1.}
 
