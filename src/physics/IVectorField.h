#pragma once
#include "common/Types.h"

// Abstract interface for any fluid velocity field
class IVectorField {
public:
    virtual ~IVectorField() = default;

    // Returns velocity vector at a specific 3D position
    virtual Vec3 getVelocity(const Vec3 &pos) const = 0;

    // Add a perturbation (source/sink) at a location (Interactive feature)
    virtual void addPerturbation(const Vec3 &pos, double strength) = 0;

    // Set base speed of the main flow
    virtual void setFlowSpeed(double speed) = 0;

    virtual void update(double dt) {}
};
