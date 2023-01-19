#include "MaterialEditorPanel.h"

#include "Assets/AssetSerializer.h"
#include "ImGui/UI.h"

#include <glm/gtc/type_ptr.hpp>

namespace Venus {

	extern const std::filesystem::path g_AssetsPath;

	MaterialEditorPanel::MaterialEditorPanel()
	{
		m_SaveIcon = Texture2D::Create("Resources/Icons/Toolbar/save.png");
	}

	void MaterialEditorPanel::SetEditingMaterial(const std::string& path)
	{
		m_MaterialPath = path;
		m_EditingMaterial = AssetManager::GetAsset<MeshMaterial>(path);
		m_ShaderName = m_EditingMaterial->GetMaterial()->GetShaderName();
	}

	void MaterialEditorPanel::OnImGuiRender(bool& show)
	{
		if (!show)
			return;

		ImGui::Begin(ICON_FA_TINT " Material Editor", &show);

		// Top Bar
		auto& colors = ImGui::GetStyle().Colors;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImVec4 normalColor = { colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y, colors[ImGuiCol_Button].z, 0.0f };
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 1.10f, 0.10f, 0.10f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, normalColor);
		ImGui::PushStyleColor(ImGuiCol_Border, normalColor);
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, normalColor);


		ImGui::Columns(3);
		ImGui::SetColumnWidth(0, 40);

		if (ImGui::ImageButton((ImTextureID)m_SaveIcon->GetRendererID(), ImVec2{ 25, 22 }, ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			AssetImporter::Serialize(m_EditingMaterial);
		}

		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 1.0, 1.0, 0.5 });
		std::filesystem::path path = m_MaterialPath;
		ImGui::Text(path.stem().string().c_str());

		ImGui::SameLine();
		ImGui::NextColumn();
		std::string shader = "Shader: " + m_ShaderName;
		ImGui::Text(shader.c_str());
		ImGui::PopStyleColor();

		ImGui::Columns(1);
		ImGui::PopStyleColor(5);
		ImGui::PopStyleVar(2);

		UI::ShiftPosY(5.0f);
		ImGui::Separator();

		if (m_EditingMaterial)
			RenderMaterialProperties();
		else
		{
			std::string msg = "Invalid or Corrupted Material";
			UI::CenterText(msg);
			UI::Text(msg);
		}

		ImGui::End();
	}

	void MaterialEditorPanel::RenderMaterialProperties()
	{
		Ref<MeshMaterial> material = m_EditingMaterial;

		// Albedo
		uint32_t albedoRenderID = material->GetAlbedoMap()->GetRendererID();
		auto& albedoColor = material->GetAlbedoColor();
		auto& emission = material->GetEmission();
		bool hasAlbedoMap = albedoRenderID != Renderer::GetDefaultTexture()->GetRendererID();
		std::string albedoLabel = "Albedo";
		if (ImGui::CollapsingHeader(albedoLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Image(reinterpret_cast<void*>(albedoRenderID), ImVec2(64, 64));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				{
					const wchar_t* payloadPath = (const wchar_t*)payload->Data;

					// TODO: This texture must be in SRGB
					Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(payloadPath);
					if(texture->IsLoaded())
						material->SetAlbedoMap(texture);
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemHovered())
			{
				if (hasAlbedoMap)
				{
					ImGui::BeginTooltip();
					ImGui::Image(reinterpret_cast<void*>(albedoRenderID), ImVec2(384, 384));
					std::string path = material->GetAlbedoMap()->GetPath();
					ImGui::Text(path.c_str());
					ImGui::EndTooltip();
				}
			}

			ImGui::SameLine();
			std::string albedoColorLabel = "Color##Albedo";
			ImGui::ColorEdit3(albedoColorLabel.c_str(), glm::value_ptr(albedoColor), ImGuiColorEditFlags_NoInputs);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(120.0f);
			UI::ShiftPosX(10.0f);
			std::string emissionLabel = "Emission##Albedo";
			ImGui::DragFloat(emissionLabel.c_str(), &emission, 0.1f, 0.0f, 50.0f);

			if (hasAlbedoMap)
			{
				std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetAlbedo";
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
				if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
				{
					material->ClearAlbedoMap();
				}
			}
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

		// Normal
		uint32_t normalRenderID = material->GetNormalMap()->GetRendererID();
		auto& useNormalMap = material->IsUsingNormalMap();
		bool hasNormalMap = normalRenderID != Renderer::GetDefaultTexture()->GetRendererID();
		std::string normalLabel = "Normal";
		if (ImGui::CollapsingHeader(normalLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Image(reinterpret_cast<void*>(normalRenderID), ImVec2(64, 64));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				{
					const wchar_t* payloadPath = (const wchar_t*)payload->Data;

					Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(payloadPath);
					if (texture->IsLoaded())
						material->SetNormalMap(texture);
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemHovered())
			{
				if (hasNormalMap)
				{
					ImGui::BeginTooltip();
					ImGui::Image(reinterpret_cast<void*>(normalRenderID), ImVec2(384, 384));
					std::string path = material->GetNormalMap()->GetPath();
					ImGui::Text(path.c_str());
					ImGui::EndTooltip();
				}
			}

			ImGui::SameLine();
			std::string useNormalMapLabel = "Use##UseNormalCheck";
			bool checkBox = useNormalMap;
			if (ImGui::Checkbox(useNormalMapLabel.c_str(), &checkBox))
			{
				material->SetUseNormalMap(checkBox);
			}

			if (hasNormalMap)
			{
				std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetNormal";
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
				if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
				{
					material->ClearNormalMap();
				}
			}
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

		// Metalness
		uint32_t metalnessRenderID = material->GetMetalnessMap()->GetRendererID();
		float& metalnessValue = material->GetMetalness();
		bool hasMetalnessMap = metalnessRenderID != Renderer::GetDefaultTexture()->GetRendererID();
		std::string metalnessLabel = "Metalness";
		if (ImGui::CollapsingHeader(metalnessLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Image(reinterpret_cast<void*>(metalnessRenderID), ImVec2(64, 64));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				{
					const wchar_t* payloadPath = (const wchar_t*)payload->Data;

					Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(payloadPath);
					if (texture->IsLoaded())
						material->SetMetalnessMap(texture);
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemHovered())
			{
				if (hasMetalnessMap)
				{
					ImGui::BeginTooltip();
					ImGui::Image(reinterpret_cast<void*>(metalnessRenderID), ImVec2(384, 384));
					std::string path = material->GetMetalnessMap()->GetPath();
					ImGui::Text(path.c_str());
					ImGui::EndTooltip();
				}
			}

			ImGui::SameLine();
			std::string metalnessValueLabel = "##ValueMetalness";
			ImGui::SliderFloat(metalnessValueLabel.c_str(), &metalnessValue, 0.0f, 1.0f);

			if (hasMetalnessMap)
			{
				std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetMetalness";
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
				if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
				{
					material->ClearMetalnessMap();
				}
			}
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

		// Roughness
		uint32_t roughnessRenderID = material->GetRoughnessMap()->GetRendererID();
		float& roughnessValue = material->GetRoughness();
		bool hasRoughnessMap = roughnessRenderID != Renderer::GetDefaultTexture()->GetRendererID();
		std::string roughnessLabel = "Roughness";
		if (ImGui::CollapsingHeader(roughnessLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Image(reinterpret_cast<void*>(roughnessRenderID), ImVec2(64, 64));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				{
					const wchar_t* payloadPath = (const wchar_t*)payload->Data;

					Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(payloadPath);
					if (texture->IsLoaded())
						material->SetRoughnessMap(texture);
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemHovered())
			{
				if (hasRoughnessMap)
				{
					ImGui::BeginTooltip();
					ImGui::Image(reinterpret_cast<void*>(roughnessRenderID), ImVec2(384, 384));
					std::string path = material->GetRoughnessMap()->GetPath();
					ImGui::Text(path.c_str());
					ImGui::EndTooltip();
				}
			}

			ImGui::SameLine();
			std::string roughnessValueLabel = "##ValueRoughness";
			ImGui::SliderFloat(roughnessValueLabel.c_str(), &roughnessValue, 0.0f, 1.0f);

			if (hasRoughnessMap)
			{
				std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetRoughness";
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
				if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
				{
					material->ClearRoughnessMap();
				}
			}
		}

	}
}

