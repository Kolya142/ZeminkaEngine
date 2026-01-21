#pragma once
#include <zeminka/main.h>

// I don't know should geometry module be separated from physics or algebra modules but i think it's simpler than spreading geometry function over different modules.

typedef struct {
    ZEVec3 center, dimensions;
} ZEGeomBBox;

bool ZEGeomAreBBoxIntersecting(ZEGeomBBox a, ZEGeomBBox b);
// TODO: convex collision https://github.com/lukesrw/separating-axis-theorem
