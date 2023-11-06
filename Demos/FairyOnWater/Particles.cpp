#include "Particles.h"
#include <ppl.h>
#include <algorithm>


Particle::Particle(AWEVector vcPosition, AWEVector vcSpeed, float scale, float life)
    : m_vcPosition(vcPosition), m_vcSpeed(vcSpeed), m_scale(scale), m_lifetime(life) {}

void Particle::Update(float deltaTime) {
    m_vcPosition.SetX(m_vcPosition.GetX() + m_vcSpeed.GetX() * deltaTime);
    m_vcPosition.SetY(m_vcPosition.GetY() + m_vcSpeed.GetY() * deltaTime);
    m_vcPosition.SetZ(m_vcPosition.GetZ() + m_vcSpeed.GetZ() * deltaTime);
    m_lifetime -= deltaTime;
}

bool Particle::IsAlive() const {
    return m_lifetime > 0.0f;
}

void ParticleSystemSparks::EmitSpark(AWEVector vcPosition) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < 10; ++i) {
        AWEVector vcSpeed;

        vcSpeed.SetX(dist(gen) * 2.0f);
        vcSpeed.SetY(dist(gen) * 2.0f);
        vcSpeed.SetZ(dist(gen) * 2.0f);
        float lifetime = std::abs(dist(gen));
        float scale = std::abs(dist(gen));

        particles.emplace_back(vcPosition, vcSpeed, scale, lifetime);
    }
}

void ParticleSystemSparks::Update(float deltaTime) {

    for (Particle& particle : particles) {
        particle.Update(deltaTime);
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& particle) {
                return !particle.IsAlive();
            }),
        particles.end());
}