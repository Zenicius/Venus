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

			Entity GetSelectedEntity() const { return m_SelectedEntity; }

		private:
			void RenderEntityNode(Entity entity);
			void RenderComponents(Entity entity);
			static void RenderVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
			
			template<typename T, typename UIFunction>
			static void RenderComponent(const std::string& name, Entity entity, bool canDelete, UIFunction uiFunction);
		private:
			Ref<Scene> m_Context;
			Entity m_SelectedEntity;
	};

}
