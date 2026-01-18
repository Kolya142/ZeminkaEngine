#include <zeminka/physics.h>

void ZEPhysics_cmsolver(f64 m1, ZEVec3 *v1, f64 m2, ZEVec3 *v2, f64 friction) {
    // WTF?
    
    // m1, m2 - masses.
    // v1, v2 - velocities.

    ZEVec3 bv1 = *v1,
        bv2 = *v2;

    f64 F = pow(friction, ZEdeltaTime);
    *v1 = ZEVec3_Scale(*v1, F);
    *v2 = ZEVec3_Scale(*v2, F);

    *v1 = ZEVec3_Add(ZEVec3_Scale(ZEVec3_Sub(bv2, *v1), m2/m1), *v1);
    *v2 = ZEVec3_Add(ZEVec3_Scale(ZEVec3_Sub(bv1, *v2), m1/m2), *v2);

    // TODO: Detect collision stuck.

    // F = frictionᵗ
    // v₁=v₁*F
    // v₂=v₂*F
    // v₁' = (v₂-v₁)/m₁*m₂+v₁
    // v₂' = (v₁-v₂)/m₂*m₁+v₂
    
    // C=m1*v1+m2*v2
}
