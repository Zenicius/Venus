#pragma once

#include "Renderer/Mesh.h"

namespace Venus {

	class Factory
	{
		public:
			static Ref<Model> CreateCube(const glm::vec3& size);
			static Ref<Model> CreateSphere(float radius);
	};

}
