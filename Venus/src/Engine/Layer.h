#pragma once

#include "pch.h"
#include "Engine/Timestep.h"

namespace Venus {

	class Layer
	{
		public :
			Layer(const std::string& name = "Layer") : m_DebugName(name) {}
			virtual ~Layer() = default;

			virtual void OnAttach() {}
			virtual void OnDetach() {}
			virtual void OnUpdate(Timestep ts) {}
			virtual void OnImGuiRender() {}

			inline const std::string& GetName() const { return m_DebugName; }

		private:
			std::string m_DebugName;

	};

}