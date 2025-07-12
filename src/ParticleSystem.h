#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "VectorTypes.h"

typedef struct
{
	Vec2 Position;
	float LifeTime;
	Vec2 Size;

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
} ParticleProps;


typedef struct
{
	Particle* Particles;
	ParticleProps Props;
	uint32_t MaxParticles;
	uint32_t ParticleCount;
} ParticleSystem;


void ParticleSystemInit(ParticleSystem* ps, const ParticleProps* props, uint32_t maxParticles);
void ParticleSystemSpawnParticle(ParticleSystem* ps);
void ParticleSystemShutdown(ParticleSystem* ps);