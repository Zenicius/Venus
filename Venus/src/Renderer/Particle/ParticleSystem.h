#pragma once

#include "Engine/Timestep.h"
#include "Renderer/Renderer2D.h"

#include "glm/glm.hpp"

namespace Venus {

	struct ParticleProps
	{
		glm::vec2 Position;
		glm::vec2 Velocity, VelocityVariation;
		glm::vec4 ColorBegin, ColorEnd;
		float SizeBegin, SizeEnd, SizeVariation;
		float LifeTime = 1.0f;
	};

	class ParticleSystem
	{
		public:
			ParticleSystem();

			void SetPoolSize(uint32_t size);

			void Emit(const ParticleProps& props, uint32_t quantity);
			void OnUpdate(Timestep ts);
			void OnRender(OrthographicCamera& ortographicCamera);

		private:
			struct Particle
			{
				glm::vec2 Position;
				glm::vec2 Velocity;
				glm::vec4 ColorBegin, ColorEnd;
				float Rotation = 0.0f;
				float SizeBegin, SizeEnd;

				float LifeTime = 1.0f;
				float LifeRemaining = 0.0f;

				bool Active = false;
			};

			std::vector<Particle> m_ParticlePool;
			uint32_t m_PoolIndex = 999;
	};

}

