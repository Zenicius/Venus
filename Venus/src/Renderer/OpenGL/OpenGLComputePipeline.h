#pragma once

#include "Renderer/ComputePipeline.h"
#include "Renderer/Shader.h"

namespace Venus {

	class OpenGLComputePipeline : public ComputePipeline
	{
		public:
			OpenGLComputePipeline(Ref<Shader> computeShader);
			virtual ~OpenGLComputePipeline();

			virtual void Begin() override;
			virtual void Execute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ, bool LogExecuteTime = true) override;

			virtual void BindImage(uint32_t binding, Ref<Texture> texture, uint32_t level = 0, uint32_t layer = 0) override;
			virtual void BindReadOnlyImage(uint32_t binding, Ref<Texture> texture, uint32_t level = 0, uint32_t layer = 0) override;
			virtual void BindWriteOnlyImage(uint32_t binding, Ref<Texture> texture, uint32_t level = 0, uint32_t layer = 0) override;

			virtual Ref<Shader> GetShader() override { return m_Shader; }

		private:
			Ref<Shader> m_Shader;

	};

}

