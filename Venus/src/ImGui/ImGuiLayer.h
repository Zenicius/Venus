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
		
			void Begin();
			void End();

		private:
	};

}

