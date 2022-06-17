#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace Venus {

	class ComputePipeline
	{
		public:
			virtual void Begin() = 0;
			virtual void Execute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ, bool LogExecuteTime = true) = 0;

			virtual void BindImage(uint32_t binding, Ref<Texture> texture, uint32_t level = 0, uint32_t layer = 0) = 0;
			virtual void BindReadOnlyImage(uint32_t binding, Ref<Texture> texture, uint32_t level = 0, uint32_t layer = 0) = 0;
			virtual void BindWriteOnlyImage(uint32_t binding, Ref<Texture> texture, uint32_t level = 0, uint32_t layer = 0) = 0;

			virtual Ref<Shader> GetShader() = 0;

			static Ref<ComputePipeline> Create(Ref<Shader> computeShader);
	};

}
