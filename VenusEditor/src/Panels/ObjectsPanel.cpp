#include "ObjectsPanel.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <glm/gtc/type_ptr.hpp>

namespace Venus {

	extern const std::filesystem::path g_AssetPath;

	ObjectsPanel::ObjectsPanel()
	{
		m_AddIcon = Texture2D::Create("Resources/Icons/Properties/Add.png");
	}

	ObjectsPanel::ObjectsPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void ObjectsPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectedEntity = {};
	}

	void ObjectsPanel::OnImGuiRender()
	{
		// Game Objcts Panel
		ImGui::Begin("Game Objects");
		m_Context->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID , m_Context.get() };

			RenderEntityNode(entity);
		});

		// Deselect entity by clicking blank spaces
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		// Create Menu by Right-Clicking at blank spaces
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			// TEMP
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Empty Object"))
				{
					auto entity = m_Context->CreateEntity();
					m_SelectedEntity = entity;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Sprite"))
				{
					auto entity = m_Context->CreateEntity("Sprite");
					entity.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
					m_SelectedEntity = entity;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Circle"))
				{
					auto entity = m_Context->CreateEntity("Circle");
					entity.AddComponent<CircleRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
					m_SelectedEntity = entity;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Camera"))
				{
					auto entity = m_Context->CreateEntity("Camera");
					entity.AddComponent<CameraComponent>();
					m_SelectedEntity = entity;
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		// Properties Panel
		// TODO: Move to its own class
		ImGui::Begin("Properties");
		if (m_SelectedEntity)
		{
			RenderComponents(m_SelectedEntity);
		}
		ImGui::End();

	}

	void ObjectsPanel::RenderEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>();

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		
		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.Name.c_str());

		bool deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Duplicate Entity"))
				m_SelectedEntity = m_Context->DuplicateEntity(entity);

			ImGui::Separator();

			if (ImGui::MenuItem("Delete Entity"))
				deleted = true;

			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		if (open)
		{
			ImGui::TreePop();
		}

		if (deleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}

	void ObjectsPanel::RenderVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// X
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.5f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.25f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.45f, 0.1f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.1f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.4f, 0.1f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		//Z
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.05f, 0.1f, 0.5f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.05f, 0.1f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.05f, 0.1f, 0.5f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	inline void ObjectsPanel::RenderComponent(const std::string& name, Entity entity, bool canDelete, UIFunction uiFunction)
	{
		if (entity.HasComponent<T>())
		{
			// Component
			auto& component = entity.GetComponent<T>();
			bool removeComponent = false;

			// Style
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | 
												ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | 
												ImGuiTreeNodeFlags_FramePadding;
			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();


			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();

			// Tree Node
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			
			// Component Settings TEMP: Delete only
			if (canDelete)
			{
				ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
				if (ImGui::Button("-", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}
				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component"))
						removeComponent = true;

					ImGui::EndPopup();
				}
			}

			// UI Function per specific component
			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			// Remove component if selected
			if (removeComponent && canDelete)
				entity.RemoveComponent<T>();
		}
	}

	void ObjectsPanel::RenderComponents(Entity entity)
	{
		// Tag Component
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>();

			char nameBuffer[256];
			memset(nameBuffer, 0, sizeof(nameBuffer));
			strcpy_s(nameBuffer, sizeof(nameBuffer), tag.Name.c_str());
			if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
			{
				tag = std::string(nameBuffer);
			}
		}

		// Add Components Button
		ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
		ImGui::SameLine(contentRegionAvail.x - 24.0f);

		if (ImGui::ImageButton((ImTextureID)m_AddIcon->GetRendererID(), ImVec2{ 24.0f, 24.0f }))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!m_SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectedEntity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectedEntity.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectedEntity.HasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					m_SelectedEntity.AddComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectedEntity.HasComponent<Rigidbody2DComponent>())
			{
				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					m_SelectedEntity.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::MenuItem("Box Collider 2D"))
				{
					m_SelectedEntity.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectedEntity.HasComponent<CircleCollider2DComponent>())
			{
				if (ImGui::MenuItem("Circle Collider 2D"))
				{
					m_SelectedEntity.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}


			ImGui::EndPopup();
		}

		// Transform Component
		RenderComponent<TransformComponent>("Transform", entity, false, [](auto& component)
		{
			RenderVec3Control("Position", component.Position);

			glm::vec3 rotation = glm::degrees(component.Rotation);
			RenderVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);

			RenderVec3Control("Scale", component.Scale, 1.0f);
		});


		// Sprite Renderer Component
		RenderComponent<SpriteRendererComponent>("Sprite Renderer", entity, true, [](auto& component) 
		{
			// Texture
			ImGui::Columns(2);
			ImGui::Text("Texture");
			ImGui::NextColumn();
			
			// Texture Open Dialog
			if (ImGui::Button(component.TextureName.c_str(), ImVec2(100.0f, 0.0f)))
			{
				std::string filePath = FileDialogs::OpenFile("Images (*.png, *.jpg)\0*.png;*.jpg\0");
				if (!filePath.empty())
				{
					std::filesystem::path texturePath = filePath;
					component.Texture = Texture2D::Create(texturePath.string());
					component.TextureName = texturePath.stem().string();
					component.TexturePath = texturePath.string();
				}
			}
			
			// Texture Drag and Drop
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					component.Texture = Texture2D::Create(texturePath.string());
					component.TextureName = texturePath.stem().string();
					component.TexturePath = texturePath.string();
				}
			}
			ImGui::Columns(1);

			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 1.0f, 100.0f);
		});

		// CircleRendererComponent
		RenderComponent<CircleRendererComponent>("Circle Renderer", entity, true, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.01f, 0.1f, 1.0f);
			ImGui::DragFloat("Fade", &component.Fade, 0.01f, 0.001f, 1.0f);
		});


		// Camera Component
		RenderComponent<CameraComponent>("Camera", entity, true, [](auto& component)
		{
			auto& camera = component.Camera;

			// Checkboxes
			ImGui::Checkbox("Primary Camera", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

			// Projection Type ComboBox
			const char* projectionType[] = { "Perspective", "Orthographic" };
			const char* currentProjectionType =
			projectionType[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Type", currentProjectionType))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionType == projectionType[i];
					if (ImGui::Selectable(projectionType[i], isSelected))
					{
						currentProjectionType = projectionType[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// Ortho Settings
			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near Clip", &orthoNear))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far Clip", &orthoFar))
					camera.SetOrthographicFarClip(orthoFar);
			}

			// Perspective Settings
			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveFOV))
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveFOV));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near Clip", &perspectiveNear))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far Clip", &perspectiveFar))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}
		});

		// Rididbody2D Component
		RenderComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, true, [](auto& component)
		{
			// Projection Type ComboBox
			const char* bodyType[] = { "Static", "Dynamic", "Kinematic"};
			const char* currentBodyType = bodyType[(int)component.Type];

			if (ImGui::BeginCombo("Body Type", currentBodyType))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentBodyType == bodyType[i];
					if (ImGui::Selectable(bodyType[i], isSelected))
					{
						currentBodyType = bodyType[i];
						component.Type = (Rigidbody2DComponent::BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// Fixed rotation Checkbox
			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);

		});

		// BoxCollider2D Component
		RenderComponent<BoxCollider2DComponent>("Box Collider 2D", entity, true, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.1f, 0.0f);
		});
		
		// CircleCollider2D Component
		RenderComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, true, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat("Radius", &component.Radius);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.1f, 0.0f);
		});
	}
}