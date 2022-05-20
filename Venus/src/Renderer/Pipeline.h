#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"

namespace Venus {

	enum class PrimitiveTopology
	{
		None = 0,
		Points,
		Lines, 
		Triangles,
		Patches,
		LineStrip,
		TriangleStrip,
		TriangleFan
	};

	struct PipelineSpecification
	{
		Ref<Shader> Shader;
		Ref<Framebuffer> Framebuffer;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		bool DepthTest = true;
		bool StencilTest = false;
		float LineWidth = 1.0f;
	};

	class Pipeline
	{
		public:
			Pipeline(const PipelineSpecification& spec);

			PipelineSpecification& GetSpecification() { return m_Specification; }
			const PipelineSpecification& GetSpecification() const { return m_Specification; }

			Ref<Shader> GetShader() { return m_Specification.Shader; }
			Ref<Framebuffer> GetFramebuffer() { return m_Specification.Framebuffer; }

		private:
			PipelineSpecification m_Specification;
	};

}

