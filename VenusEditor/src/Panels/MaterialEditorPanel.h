#pragma once

#include <Venus.h>

namespace Venus {

	class MaterialEditorPanel
	{
		public:
			MaterialEditorPanel();

			void SetEditingMaterial(const std::string& path);

			void OnImGuiRender(bool& show);
			void RenderMaterialProperties();

		private:
			Ref<Texture2D> m_SaveIcon;

			Ref<MeshMaterial> m_EditingMaterial;
			std::string m_MaterialPath;
			std::string m_ShaderName;
	};

}

