#pragma once
#include <zeminka/main.h>

#ifndef DOT3
#define DOT3(x1,y1,z1,x2,y2,z2) (((x1)*(x2))+((y1)*(y2))+((z1)*(z2)))
#endif

// Parameter friction = F₁*F₂ and it's applies to v1 and v2 identically,
//  its value works this way: friction = 1 is cleanest ice, friction = 0 is dirtiest ground.
//  Values like friction=2 or friction=-1 allowed but they looks strange.
//  In this function friction made this way:
//    v₁´ = v₁*F
//    v₂´ = v₂*F
void ZEPhysics_cmsolver(f64 m1, ZEVec3 *v1, f64 m2, ZEVec3 *v2, f64 friction);
// TODO: convex collision https://github.com/lukesrw/separating-axis-theorem

