#pragma once

#include "ParticleSystem.h"

namespace Venus {
	
	namespace Particles {

		struct DefaultParticle : ParticleProps
		{
			glm::vec2 Position = { 0.0f, 0.0f };
			glm::vec2 Velocity = { 0.0f, 0.0f };
			glm::vec2 VelocityVariation = { 3.0f, 1.0f };
			glm::vec4 ColorBegin = { 1.0f, 0.0f, 0.0f, 1.0f };
			glm::vec4 ColorEnd = { 0.0f, 0.0f, 1.0f, 0.0f };
			float SizeBegin = 0.2f, SizeEnd = 0.0f, SizeVariation = 0.1f;
			float LifeTime = 1.0f;
		};
	}
}