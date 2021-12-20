#include "pch.h"
#include "ParticleSystem.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/compatibility.hpp>

namespace Venus {

	ParticleSystem::ParticleSystem()
	{
		m_ParticlePool.resize(10000);
	}

	void ParticleSystem::SetPoolSize(uint32_t size)
	{
		m_ParticlePool.resize(size);
	}

	void ParticleSystem::Emit(const ParticleProps& props, uint32_t quantity)
	{
		for (int i = 0; i < quantity; i++)
		{
			Particle& particle = m_ParticlePool[m_PoolIndex];
			particle.Active = true;
			particle.Position = props.Position;
			particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();


			particle.Velocity = props.Velocity;
			particle.Velocity.x += props.VelocityVariation.x * (Random::Float() - 0.5f);
			particle.Velocity.y += props.VelocityVariation.y * (Random::Float() - 0.5f);

			particle.ColorBegin = props.ColorBegin;
			particle.ColorEnd = props.ColorEnd;

			particle.LifeTime = props.LifeTime;
			particle.LifeRemaining = props.LifeTime;

			particle.SizeBegin = props.SizeBegin + props.SizeVariation * (Random::Float() - 0.5f);
			particle.SizeEnd = props.SizeEnd;

			m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
		}
	}

	void ParticleSystem::OnUpdate(Timestep ts)
	{
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			if (particle.LifeRemaining <= 0.0f)
			{
				particle.Active = false;
				continue;
			}

			particle.LifeRemaining -= ts;
			particle.Position += particle.Velocity * (float) ts;
			particle.Rotation += 0.01f * ts;
		}
	}

	void ParticleSystem::OnRender(OrthographicCamera& ortographicCamera)
	{
		Renderer2D::BeginScene(ortographicCamera);
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			float life = particle.LifeRemaining / particle.LifeTime;
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);

			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

			Renderer2D::DrawRotatedQuad(particle.Position, { size, size }, particle.Rotation, color);
		}
		Renderer2D::EndScene();
	}
}