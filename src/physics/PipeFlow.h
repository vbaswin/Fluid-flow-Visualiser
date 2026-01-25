#pragma once
#include "physics/IVectorField.h"
#include <algorithm>
#include <cmath>
#include <vector>

struct Perturbation {
    Vec3 pos;
    double strength;
    double decay;
};

// Implements Laminar Pipe Flow + Interactive Sources
class PipeFlow : public IVectorField {
public:
    PipeFlow(double radius) : R(radius), baseSpeed(1.0) {}

    Vec3 getVelocity(const Vec3 &p) const override {
        Vec3 v = {0, 0, 0};

        // 1. Base Poiseuille Flow (Parabolic profile along Z)
        double rSq = p.x * p.x + p.y * p.y;
        if (rSq < R * R) {
            v.z = baseSpeed * (1.0 - rSq / (R * R));
        }

        // 2. Add Interactive Perturbations (Sources/Sinks)
        for (const auto &pert : perturbations) {
            double dx = p.x - pert.pos.x;
            double dy = p.y - pert.pos.y;
            double dz = p.z - pert.pos.z;
            double distSq = dx * dx + dy * dy + dz * dz;
            double dist = std::sqrt(distSq);

            // Avoid division by zero
            if (dist > 0.1) {
                // Inverse square law influence
                double factor = pert.strength / (1.0 + pert.decay * distSq);
                v.x += (dx / dist) * factor;
                v.y += (dy / dist) * factor;
                v.z += (dz / dist) * factor;
            }
        }
        return v;
    }

    void addPerturbation(const Vec3 &pos, double strength) override {
        // Add transient push/pull; keep list small for performance
        perturbations.push_back({pos, strength, 0.5});
        if (perturbations.size() > 5)
            perturbations.erase(perturbations.begin());
    }

    void update(double dt) override {
        constexpr double DECAY_RATE = 2.0;   // Perturbations halves in ~0.35 seconds
        constexpr double MIN_STRENGTH = 0.5; // Remove when negligible

        // Exponential decay: strength *= e^(-rate * dt)
        for (auto &pert : perturbations) {
            pert.strength *= std::exp(-DECAY_RATE * dt);
        }

        // Remove dead perturbations (strength below threshold)
        perturbations.erase(
            std::remove_if(perturbations.begin(), perturbations.end(), [](const Perturbation &p) { return p.strength < MIN_STRENGTH; }), perturbations.end());
    }

    void setFlowSpeed(double speed) override { baseSpeed = speed; }

private:
    double R;
    double baseSpeed;
    std::vector<Perturbation> perturbations;
};
