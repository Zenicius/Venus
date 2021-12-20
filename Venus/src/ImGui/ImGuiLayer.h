#pragma once

#include "Engine/Layer.h"

namespace Venus {

	class ImGuiLayer : public Layer
	{
		public:
			ImGuiLayer();
			~ImGuiLayer() = default;

			virtual void OnAttach() override;
			virtual void OnDetach() override;
			virtual void OnImGuiRender() override;
			virtual void OnEvent(Event& e) override;
		
			void Begin();
			void End();
			void BlockEvents(bool block) { m_BlockEvents = block; }
		private:
			bool m_BlockEvents = true;
	};

}

