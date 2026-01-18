#include <zeminka/engine.h>

// Thanks Aitor Lopera Toledo for https://github.com/atlotl/CubeIntersectionCalculator.
bool ZEGeomAreBBoxIntersecting(ZEGeomBBox a, ZEGeomBBox b) {
    return !(a.center.x + a.dimensions.x / 2 < b.center.x - b.dimensions.x / 2 ||
             a.center.x - a.dimensions.x / 2 > b.center.x + b.dimensions.x / 2 ||
             a.center.y + a.dimensions.y / 2 < b.center.y - b.dimensions.y / 2 ||
             a.center.y - a.dimensions.y / 2 > b.center.y + b.dimensions.y / 2 ||
             a.center.z + a.dimensions.z / 2 < b.center.z - b.dimensions.z / 2 ||
             a.center.z - a.dimensions.z / 2 > b.center.z + b.dimensions.z / 2);
}
