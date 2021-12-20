#include "pch.h"
#include "RenderCommand.h"

namespace Venus {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}