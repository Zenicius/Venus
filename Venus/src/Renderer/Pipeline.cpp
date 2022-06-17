#include "pch.h"
#include "Pipeline.h"

namespace Venus {

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		return CreateRef<Pipeline>(spec);
	}

	Pipeline::Pipeline(const PipelineSpecification& spec)
		: m_Specification(spec)
	{
	}

}