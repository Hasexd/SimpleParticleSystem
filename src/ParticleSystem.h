#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "VectorTypes.h"

typedef struct
{
	Vec2 Position;
	Vec2 Velocity;
	float LifeTime;
	Vec2 Size;
	bool Active;
} Particle;


typedef struct
{
	Vec2 Position;
	Vec2 VelocityMin;
	Vec2 VelocityMax;
	Vec2 SizeMin;
	Vec2 SizeMax;
	float LifeTimeMin;
	float LifeTimeMax;
	Vec3 BirthColor;
	Vec3 DeathColor;
	Vec2 Gravity;
} ParticleProps;


typedef struct
{
	Particle* Particles;
	ParticleProps Props;
	uint32_t MaxParticles;
} ParticleSystem;


void ParticleSystemInit(ParticleSystem* ps, const ParticleProps* props, uint32_t maxParticles);
void ParticleSystemEmit(ParticleSystem* ps);
void ParticleSystemUpdate(ParticleSystem* ps, float deltaTime);
void ParticleSystemShutdown(ParticleSystem* ps);