#pragma once
#include <iostream>
#include <vector>
#include <random>
#include "..\..\Math\AwesomeMath\AwesomeMath.h"

class Particle {
public:
    AWEVector m_vcPosition;
    AWEVector m_vcSpeed;
    float m_lifetime;
    float m_scale;

    Particle(AWEVector vcPosition, AWEVector vcSpeed, float scale, float life);

    void Update(float deltaTime);

    bool IsAlive() const;
};

class ParticleSystemSparks {
public:
    ParticleSystemSparks() {};
    ~ParticleSystemSparks() {};

    std::vector<Particle> particles;

    void EmitSpark(AWEVector vcPosition);

    void Update(float deltaTime);

};

