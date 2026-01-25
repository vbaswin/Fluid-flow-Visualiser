#pragma once
#include "physics/IVectorField.h"
#include <random>
#include <vector>

struct Particle {
    Vec3 pos;
    double age;
    bool active;
};

// Manages thousands of particles and their advection
class ParticleSystem {
public:
    ParticleSystem(int count, double radius, double length)
        : maxParticles(count), pipeRadius(radius), pipeLength(length) {
        particles.resize(count);
        reset();
    }

    void setFlowField(IVectorField *field) { flow = field; }

    // Runge-Kutta 4 Integration Step
    void update(double dt) {
        if (!flow)
            return;

        for (auto &p : particles) {
            double rSq = p.pos.x * p.pos.x + p.pos.y * p.pos.y;
            bool outsideRadius = rSq > pipeRadius * pipeRadius;
            bool pastOutlet = p.pos.z > pipeLength;
            bool beforeInlet = p.pos.z < 0.0;

            if (!p.active || outsideRadius || pastOutlet || beforeInlet) {
                respawn(p); // Recycle particle if out of bounds
                continue;   // skip RK4 for freshly respawned particles
            }

            // RK4 Algorithm for smooth curves
            Vec3 k1 = flow->getVelocity(p.pos);
            Vec3 k2 = flow->getVelocity(p.pos + k1 * (dt * 0.5));
            Vec3 k3 = flow->getVelocity(p.pos + k2 * (dt * 0.5));
            Vec3 k4 = flow->getVelocity(p.pos + k3 * dt);

            // Update position
            p.pos = p.pos + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
            p.age += dt;
        }
    }

    const std::vector<Particle> &getParticles() const { return particles; }

private:
    void respawn(Particle &p) {
        // Randomly place at the inlet (Z=0) inside the circle
        double angle = dis(gen) * 6.28318;
        double r =
            std::sqrt(dis(gen)) * pipeRadius; // Sqrt for uniform area distribution
        double z = dis(gen) * pipeLength;     // random z along entire pipe length
        p.pos = {r * std::cos(angle), r * std::sin(angle), z};
        p.age = 0.0;
        p.active = true;
    }

    void reset() {
        for (auto &p : particles) {
            respawn(p);
        }
    }

    int maxParticles;
    double pipeRadius, pipeLength;
    IVectorField *flow = nullptr;
    std::vector<Particle> particles;
    std::mt19937 gen{std::random_device{}()};
    std::uniform_real_distribution<> dis{0.0, 1.0};
};
