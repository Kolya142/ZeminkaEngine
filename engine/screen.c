#include <neweng/engine.h>

#define RGFW_IMPLEMENTATION
#define RGFW_OPENGL
#include "../thirdparty/RGFW.h"

#include <GL/gl.h>
#include <GL/glu.h>

static RGFW_window *rwin;
static RGFW_event rev;

static u8 icon[16 * 16 * 3 + 1] = // Silly Placeholder
    "\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000"
    "\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000J\000\377J"
    "\000\377\060\377\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000J\000\377J\000\377J\000"
    "\377J\000\377J\000\377J\000\377J\000\377\377\000\000J\000\377J\000\377J\000\377\060\377\000\377"
    "\000\000\377\000\000\377\000\000\377\000\000J\000\377\060\377\000\060\377\000\060\377\000\060\377\000\060"
    "\377\000\060\377\000\377\000\000J\000\377\060\377\000J\000\377J\000\377\060\377\000\377\000\000\377"
    "\000\000\377\000\000J\000\377\060\377\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377"
    "\000\000J\000\377\060\377\000\060\377\000J\000\377\060\377\000\377\000\000\377\000\000\377\000\000J\000"
    "\377\060\377\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000J\000\377\060\377"
    "\000\377\000\000J\000\377J\000\377\060\377\000\377\000\000\377\000\000J\000\377\060\377\000\377\000\000"
    "\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000J\000\377\060\377\000\377\000\000\060\377\000"
    "J\000\377\060\377\000\377\000\000\377\000\000J\000\377\060\377\000\377\000\000\377\000\000\377\000\000"
    "\377\000\000\377\000\000\377\000\000J\000\377\060\377\000\377\000\000\060\377\000J\000\377J\000\377\060"
    "\377\000\377\000\000J\000\377J\000\377J\000\377J\000\377J\000\377J\000\377J\000\377\377\000\000J"
    "\000\377\060\377\000\377\000\000\377\000\000\060\377\000J\000\377\060\377\000\377\000\000J\000\377\060"
    "\377\000\060\377\000\060\377\000\060\377\000\060\377\000\060\377\000\377\000\000J\000\377\060\377"
    "\000\377\000\000\377\000\000\060\377\000J\000\377J\000\377\060\377\000J\000\377\060\377\000\377\000"
    "\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000J\000\377\060\377\000\377\000\000\377\000"
    "\000\377\000\000\060\377\000J\000\377\060\377\000J\000\377\060\377\000\377\000\000\377\000\000\377\000"
    "\000\377\000\000\377\000\000\377\000\000J\000\377\060\377\000\377\000\000\377\000\000\377\000\000\060\377"
    "\000J\000\377J\000\377J\000\377\060\377\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000"
    "\377\000\000J\000\377\060\377\000\377\000\000\377\000\000\377\000\000\377\000\000\060\377\000J\000\377"
    "J\000\377\060\377\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000J\000\377\060"
    "\377\000\377\000\000\377\000\000\377\000\000\377\000\000\060\377\000J\000\377J\000\377\060\377\000\377"
    "\000\000\377\000\000\377\000\000\377\000\000\377\000\000\377\000\000J\000\377\060\377\000\377\000\000\377"
    "\000\000\377\000\000\377\000\000\377\000\000\060\377\000J\000\377J\000\377J\000\377J\000\377J\000\377"
    "J\000\377J\000\377\377\000\000\060\377\000\060\377\000\377\000\000\377\000\000\377\000\000\377\000\000"
    "\377\000\000\377\000\000\060\377\000\060\377\000\060\377\000\060\377\000\060\377\000\060\377\000\060"
    "\377\000";

void NScreen_init(u32 width, u32 height, const char *title) {
    rwin = RGFW_createWindow(title, 0, 0, width, height, RGFW_windowCenter | RGFW_windowOpenGL);
    RGFW_window_setIcon(rwin, icon, 16, 16, RGFW_formatBGR8);
    RGFW_window_makeCurrentContext_OpenGL(rwin);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    f64 aspect = ((f64)width)/((f64)height);
    gluPerspective(60., aspect, .1, 100.);

    // Something is wrong with GLu.
    {
        f32 m[16] = {0};
        m[0] = 1.f;
        m[5] = 1.f;
        m[10] = -1.f;
        m[15] = 1.f;
        glMultMatrixf(m);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static bool is_closed = false;
static bool isnt_closed = true;

bool NScreen_IsClosed() {
    return is_closed;
}

bool NScreen_IsNtClosed() {
    return isnt_closed;
}

void NScreen_BeginFrame() {
    is_closed = RGFW_window_shouldClose(rwin);
    isnt_closed = !is_closed;
    if (is_closed) return;
    while (RGFW_window_checkEvent(rwin, &rev)) {
        if (rev.type == RGFW_quit) {
            is_closed = true;
            isnt_closed = false;
        }
    }
    if (is_closed) return;
    
    glClearColor(0.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void NScreen_EndFrame() {
    RGFW_window_swapBuffers_OpenGL(rwin);
    glFlush();
}

void NScreen_DrawCircle(NE_Vec3 o, f64 r, NE_Color col) {
    glBegin(GL_TRIANGLES);
    glColor4d(col.r, col.g, col.b, col.a);

#define CIRCLE_SEGMENTS 16
    f64 lx = 0, ly = r;
    for (f64 i = 0; i <= TAU; i += TAU/CIRCLE_SEGMENTS) {
        f64 cx = sin(i)*r,
            cy = cos(i)*r;
        glVertex3d(o.x, o.y, o.z);
        glVertex3d(o.x+lx, o.y+ly, o.z);
        glVertex3d(o.x+cx, o.y+cy, o.z);
        lx = cx;
        ly = cy;
    }

    glVertex3d(o.x, o.y, o.z);
    glVertex3d(o.x+lx, o.y+ly, o.z);
    glVertex3d(o.x, o.y+r, o.z);
    
    glEnd();
}

void NScreen_DrawTriangle(NE_Vec3 a, NE_Vec3 b, NE_Vec3 c, NE_Color col) {
    glBegin(GL_TRIANGLES);
    glColor4d(col.r, col.g, col.b, col.a);
    glVertex3d(a.x, a.y, a.z);
    glVertex3d(b.x, b.y, b.z);
    glVertex3d(c.x, c.y, c.z);
    glEnd();
}

void NScreen_DrawTriangle_Ex(NE_Vec3 a, NE_Vec3 b, NE_Vec3 c, NE_Color a_c, NE_Color b_c, NE_Color c_c) {
    glBegin(GL_TRIANGLES);
    glColor4d(a_c.r, a_c.g, a_c.b, a_c.a);
    glVertex3d(a.x, a.y, a.z);
    glColor4d(b_c.r, b_c.g, b_c.b, b_c.a);
    glVertex3d(b.x, b.y, b.z);
    glColor4d(c_c.r, c_c.g, c_c.b, c_c.a);
    glVertex3d(c.x, c.y, c.z);
    glEnd();
}

bool NScreen_IsKeyPressed(NE_Key key) {
    return RGFW_isKeyPressed(key);
}

bool NScreen_IsKeyDown(NE_Key key) {
    return RGFW_isKeyDown(key);
}

bool NScreen_IsKeyReleased(NE_Key key) {
    return RGFW_isKeyReleased(key);
}

static bool _depth_test = true;

bool NScreen_GetDepthTest() {
    return _depth_test;
}

void NScreen_SetDepthTest(bool test) {
    _depth_test = test;
    glDepthFunc(test ? GL_LESS : GL_ALWAYS);
}

void NScreen_RenderModel(NE_Model model, NE_TransformW transform) {
    for (size_t i = 0; i < model.face_count; ++i) {
        NE_Face face = model.faces[i];
        NScreen_DrawTriangle_Ex(
            NE_TransformW_Apply(transform, model.verteces[face.a]),
            NE_TransformW_Apply(transform, model.verteces[face.b]),
            NE_TransformW_Apply(transform, model.verteces[face.c]),
            model.colors[face.a],
            model.colors[face.b],
            model.colors[face.c]
            );
    }
}

void *NScreen_GetSystemHandler() {
    return rwin;
}
