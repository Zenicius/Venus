#pragma once
#include <Venus.h>

namespace Venus {

	class ObjectsPanel
	{
		public:
			ObjectsPanel() = default;
			ObjectsPanel(const Ref<Scene>& context);

			void SetContext(const Ref<Scene>& context);

			void OnImGuiRender();
		private:
			void RenderEntityNode(Entity entity);
			void RenderVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
			void RenderComponents(Entity entity);

		private:
			Ref<Scene> m_Context;
			Entity m_SelectedEntity;
	};

}
