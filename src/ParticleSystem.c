#include "ParticleSystem.h"

static float RandomFloat(float min, float max)
{
	return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

void ParticleSystemInit(ParticleSystem* ps, const ParticleProps* props, uint32_t maxParticles)
{
	srand(time(NULL));

	ps->Props = *props;
	ps->MaxParticles = maxParticles;

	ps->Particles = malloc(sizeof(Particle) * maxParticles);

	for (size_t i = 0; i < maxParticles; i++)
		ps->Particles[i].Active = false;
}

void ParticleSystemEmit(ParticleSystem* ps)
{
	for (uint32_t i = 0; i < ps->MaxParticles; i++)
	{
		if (!ps->Particles[i].Active)
		{

			ps->Particles[i] = (Particle){
				.Position = ps->Props.Position,
				.LifeTime = RandomFloat(ps->Props.LifeTimeMin, ps->Props.LifeTimeMax),
				.Size = RandomFloat(ps->Props.SizeMin, ps->Props.SizeMax),
				.Velocity = {
					RandomFloat(ps->Props.VelocityMin.X, ps->Props.VelocityMax.X),
					RandomFloat(ps->Props.VelocityMin.Y, ps->Props.VelocityMax.Y)
				},
				.Active = true
			};
			return;
		}
	}

	printf("ParticleSystemEmit: All particle slots are active\n");
}

void ParticleSystemUpdate(ParticleSystem* ps, float deltaTime)
{
	if (!ps || !ps->Particles)
		return;

	for (size_t i = 0; i < ps->MaxParticles; i++)
	{
		Particle* particle = &ps->Particles[i];

		if (!particle->Active)
			continue;

		particle->LifeTime -= deltaTime;

		if (particle->LifeTime <= 0.0f)
		{
			particle->Active = false;
			continue;
		}

		particle->Velocity.X += ps->Props.Gravity.X * deltaTime;
		particle->Velocity.Y += ps->Props.Gravity.Y * deltaTime;

		particle->Position.X += particle->Velocity.X * deltaTime;
		particle->Position.Y += particle->Velocity.Y * deltaTime;
	}
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