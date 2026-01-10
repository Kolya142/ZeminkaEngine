#include <zeminka/engine.h>

#define RGFW_IMPLEMENTATION
#define RGFW_OPENGL
#include "../thirdparty/RGFW.h"

#include <GL/gl.h>

static RGFW_window *rwin;
static RGFW_event rev;

static f64 rwidth, rheight;
static f64 rfov;

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

void ZEScreen_init(u32 width, u32 height, f64 fov, const char *title) {
    rfov = fov;
    rwidth = width;
    rheight = height;
    // TODO: unhardcode it.
    bool is_fullscreen = (width == 1920) && (height == 1080); // My laptop has screen 1920x1080.
    rwin = RGFW_createWindow(title, 0, 0, width, height, (is_fullscreen ? RGFW_windowFullscreen | RGFW_windowOpenGL : RGFW_windowCenter) | RGFW_windowOpenGL);
    RGFW_window_setIcon(rwin, icon, 16, 16, RGFW_formatBGR8);
    RGFW_window_makeCurrentContext_OpenGL(rwin);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

static bool is_closed = false;
static bool isnt_closed = true;

bool ZEScreen_IsClosed() {
    return is_closed;
}

bool ZEScreen_IsNtClosed() {
    return isnt_closed;
}

#include <GL/glu.h>

void ZEScreen_BeginFrame(f64 *omx, f64 *omy) {
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    f64 aspect = ((f64)rwidth)/((f64)rheight);
    { // stolen from GLu.
        f32 m[4][4] = {0};
        f32 radians = rfov / 2 * PI / 180;
        f32 zNear = .01, zFar = 100.;
        f32 deltaZ = zFar-zNear;
        f32 sine = sin(radians);
        f32 cotangent = cos(radians)/sine;
        m[0][0] = cotangent / aspect;
        m[1][1] = cotangent;
        m[2][2] = -(zFar + zNear) / deltaZ;
        m[2][3] = -1;
        m[3][2] = -2 * zNear * zFar / deltaZ;
        
        glLoadMatrixf((f32 *)m);
    }
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

    // It doesn't work on MS Windows.
    if (omx) *omx = 0;
    if (omy) *omy = 0;
    // i32 mx_ = 0, my_ = 0;
    // RGFW_window_getMouse(rwin, &mx_, &my_);
    // {
    //     f32 mx = mx_;
    //     f32 my = my_;
    //     mx -= rwidth*.5;
    //     my -= rheight*.5;
    //     mx /= rwidth*.5;
    //     my /= rheight*.5;
    //     if (omx) *omx = mx;
    //     if (omy) *omy = my;
    //     RGFW_window_moveMouse(rwin, rwin->x+rwidth*.5, rwin->y+rheight*.5);
    // }
}

void ZEScreen_TranslateCamera(ZEVec3 origin) {
    glMatrixMode(GL_PROJECTION);
    glTranslated(-origin.x, -origin.y, -origin.z);
}

void ZEScreen_RotateCamera(f64 yaw, f64 pitch, f64 roll) {
    glMatrixMode(GL_PROJECTION);
    glRotated(roll*RAD2DEG, 0, 0, 1);
    glRotated(pitch*RAD2DEG, 1, 0, 0);
    glRotated(yaw*RAD2DEG, 0, 1, 0);
}

void ZEScreen_EndFrame() {
    RGFW_window_swapBuffers_OpenGL(rwin);
    glFlush();
}

void ZEScreen_DrawCircle(ZEVec3 o, f64 r, ZEColor col) {
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

void ZEScreen_DrawTriangle(ZEVec3 a, ZEVec3 b, ZEVec3 c, ZEColor col) {
    glBegin(GL_TRIANGLES);
    glColor4d(col.r, col.g, col.b, col.a);
    glVertex3d(a.x, a.y, a.z);
    glVertex3d(b.x, b.y, b.z);
    glVertex3d(c.x, c.y, c.z);
    glEnd();
}

void ZEScreen_DrawTriangle_Ex(ZEVec3 a, ZEVec3 b, ZEVec3 c, ZEColor a_c, ZEColor b_c, ZEColor c_c) {
    glBegin(GL_TRIANGLES);
    glColor4d(a_c.r, a_c.g, a_c.b, a_c.a);
    glVertex3d(a.x, a.y, a.z);
    glColor4d(b_c.r, b_c.g, b_c.b, b_c.a);
    glVertex3d(b.x, b.y, b.z);
    glColor4d(c_c.r, c_c.g, c_c.b, c_c.a);
    glVertex3d(c.x, c.y, c.z);
    glEnd();
}

bool ZEScreen_IsKeyPressed(ZEKey key) {
    return RGFW_isKeyPressed(key);
}

bool ZEScreen_IsKeyDown(ZEKey key) {
    return RGFW_isKeyDown(key);
}

bool ZEScreen_IsKeyReleased(ZEKey key) {
    return RGFW_isKeyReleased(key);
}

static bool _depth_test = true;

bool ZEScreen_GetDepthTest() {
    return _depth_test;
}

void ZEScreen_SetDepthTest(bool test) {
    _depth_test = test;
    glDepthFunc(test ? GL_LESS : GL_ALWAYS);
}

void ZEScreen_RenderModel(ZEModel model, ZETransformW transform) {
    for (size_t i = 0; i < model.face_count; ++i) {
        ZEFace face = model.faces[i];
        ZEScreen_DrawTriangle_Ex(
            ZETransformW_Apply(transform, model.verteces[face.a]),
            ZETransformW_Apply(transform, model.verteces[face.b]),
            ZETransformW_Apply(transform, model.verteces[face.c]),
            model.colors[face.a],
            model.colors[face.b],
            model.colors[face.c]
            );
    }
}

void *ZEScreen_GetSystemHandler() {
    return rwin;
}
