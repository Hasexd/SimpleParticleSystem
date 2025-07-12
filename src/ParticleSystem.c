#include "ParticleSystem.h"


void ParticleSystemInit(ParticleSystem* ps, const ParticleProps* props, uint32_t maxParticles)
{
	srand(time(NULL));

	ps->Props = *props;
	ps->MaxParticles = maxParticles;

	ps->Particles = malloc(sizeof(Particle) * maxParticles);
	ps->ParticleCount = 0;
}

void ParticleSystemSpawnParticle(ParticleSystem* ps)
{
	if (ps->ParticleCount >= ps->MaxParticles)
	{
		printf("ParticleSystemSpawnParticle: Maximum particle count reached (%u)\n", ps->MaxParticles);
		return;
	}

	const Particle particle =
	{
		.Position = ps->Props.Position,
		.LifeTime = ps->Props.LifeTimeMin + (rand() / (float)RAND_MAX) * (ps->Props.LifeTimeMax - ps->Props.LifeTimeMin),
		.Size = 
		{
			ps->Props.SizeMin.X + (rand() / (float)RAND_MAX) * (ps->Props.SizeMax.X - ps->Props.SizeMin.X),
			ps->Props.SizeMin.Y + (rand() / (float)RAND_MAX) * (ps->Props.SizeMax.Y - ps->Props.SizeMin.Y)
		}
	};

	ps->Particles[ps->ParticleCount++] = particle;
}


void ParticleSystemShutdown(ParticleSystem* ps)
{
	if (ps->Particles) 
	{
		free(ps->Particles);
		ps->Particles = NULL;
	}
	ps->MaxParticles = 0;
	ps->Props = (ParticleProps){0};
}