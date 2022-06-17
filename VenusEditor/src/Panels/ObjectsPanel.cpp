#include "ObjectsPanel.h"

#include "ImGui/UI.h"
#include <glm/gtc/type_ptr.hpp>

namespace Venus {

	extern const std::filesystem::path g_AssetPath;

	ObjectsPanel::ObjectsPanel()
	{
	}

	ObjectsPanel::ObjectsPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void ObjectsPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectedEntity = {};
		m_Context->SetEditorSelectedEntity(-1);
	}

	void ObjectsPanel::OnImGuiRender()
	{
		// Objcts Panel
		ImGui::Begin(ICON_FA_LIST  "  Objects");

		// Search Objects
		ImGui::TextDisabled(ICON_FA_SEARCH);
		ImGui::SameLine();
		static ImGuiTextFilter objectsFilter;
		objectsFilter.Draw("##Search", ImGui::GetWindowSize().x - 40.0f);
		ImGui::Separator();

		// Render Entities
		m_Context->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID , m_Context.get() };

			std::string name = entity.GetComponent<TagComponent>().Name;
			
			if (objectsFilter.PassFilter(name.c_str()))
			{
				RenderEntityNode(entity);
			}
		});

		// Deselect entity by clicking blank spaces
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectedEntity = {};
			m_Context->SetEditorSelectedEntity(-1);
		}

		// Create Menu by Right-Clicking at blank spaces
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{

			if (ImGui::MenuItem("Create Empty"))
			{
				auto entity = m_Context->CreateEntity();
				m_SelectedEntity = entity;
				m_Context->SetEditorSelectedEntity(entity);
			}

			if (ImGui::MenuItem("Create Camera"))
			{
				auto entity = m_Context->CreateEntity("Camera");
				entity.AddComponent<CameraComponent>();
				m_SelectedEntity = entity;
				m_Context->SetEditorSelectedEntity(entity);
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("3D Objects"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					auto entity = m_Context->CreateEntity("Cube");
					entity.AddComponent<MeshRendererComponent>(); // Default is cube
					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Sphere"))
				{
					auto entity = m_Context->CreateEntity("Sphere");
					auto& component = entity.AddComponent<MeshRendererComponent>();

					component.Model = Model::Create("Resources/Models/Sphere.fbx");
					component.ModelName = "Sphere";

					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Plane"))
				{
					auto entity = m_Context->CreateEntity("Plane");
					auto& component = entity.AddComponent<MeshRendererComponent>();

					component.Model = Model::Create("Resources/Models/Plane.fbx");
					component.ModelName = "Plane";

					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Capsule"))
				{
					auto entity = m_Context->CreateEntity("Capsule");
					auto& component = entity.AddComponent<MeshRendererComponent>();

					component.Model = Model::Create("Resources/Models/Capsule.fbx");
					component.ModelName = "Capsule";

					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Torus"))
				{
					auto entity = m_Context->CreateEntity("Torus");
					auto& component = entity.AddComponent<MeshRendererComponent>();

					component.Model = Model::Create("Resources/Models/Torus.fbx");
					component.ModelName = "Torus";

					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Cone"))
				{
					auto entity = m_Context->CreateEntity("Cone");
					auto& component = entity.AddComponent<MeshRendererComponent>();

					component.Model = Model::Create("Resources/Models/Cone.fbx");
					component.ModelName = "Cone";

					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Cylinder"))
				{
					auto entity = m_Context->CreateEntity("Cylinder");
					auto& component = entity.AddComponent<MeshRendererComponent>();

					component.Model = Model::Create("Resources/Models/Cylinder.fbx");
					component.ModelName = "Cylinder";

					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("2D Objects"))
			{
				if (ImGui::MenuItem("Sprite"))
				{
					auto entity = m_Context->CreateEntity("Sprite");
					entity.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Circle"))
				{
					auto entity = m_Context->CreateEntity("Circle");
					entity.AddComponent<CircleRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Sky Light"))
				{
					auto entity = m_Context->CreateEntity("Sky Light");
					entity.AddComponent<SkyLightComponent>();
					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Directional Light"))
				{
					auto entity = m_Context->CreateEntity("Directional Light");
					auto& transform = entity.GetComponent<TransformComponent>();
					transform.Rotation = glm::radians(glm::vec3({ 80.0f, 10.0f, 0.0f }));
					entity.AddComponent<DirectionalLightComponent>();
					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("Point Light"))
				{
					auto entity = m_Context->CreateEntity("Point Light");
					entity.AddComponent<PointLightComponent>();
					m_SelectedEntity = entity;
					m_Context->SetEditorSelectedEntity(entity);
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		// Properties Panel
		// TODO: Move to its own class
		ImGui::Begin(ICON_FA_INFO " Details");
		if (m_SelectedEntity)
		{
			RenderComponents(m_SelectedEntity);
		}
		else
		{
			std::string msg = "Select a Game Object to see details.";
			ImGui::PushStyleColor(ImGuiCol_Text, {0.30f, 0.30f, 0.30f, 0.95f});
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(msg.c_str()).x) * 0.5f);
			ImGui::Text(msg.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::End();
	}

	void ObjectsPanel::RenderEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>();

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
		//	ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		std::string nameWithIcon = ICON_FA_CUBE "   " + tag.Name;
		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, nameWithIcon.c_str());

		bool deleted = false;

		// Right Click Entity Options
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Duplicate Entity"))
			{
				m_SelectedEntity = m_Context->DuplicateEntity(entity);
				m_Context->SetEditorSelectedEntity(m_SelectedEntity);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Delete Entity"))
				deleted = true;

			ImGui::EndPopup();
		}

		// Drag and Drop Source
		if (ImGui::BeginDragDropSource())
		{
			uint64_t ID = entity.GetUUID();
			ImGui::SetDragDropPayload("OBJECT_PREFAB", &ID, sizeof(uint64_t));
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
			m_Context->SetEditorSelectedEntity(entity);
		}

		if (open)
		{
			ImGui::TreePop();
		}

		if (deleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
			{
				m_SelectedEntity = {};
				m_Context->SetEditorSelectedEntity(-1);
			}
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
	inline void ObjectsPanel::RenderComponent(const std::string& name, Entity entity, bool canDelete, UIFunction uiFunction, bool separator)
	{
		ImGui::PushID((void*)typeid(T).hash_code());

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
		
		if (separator)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
			ImGui::Separator();
		}

		// Tree Node
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();
		
		// Component Settings 
		ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
		if (ImGui::Button(ICON_FA_COG, {lineHeight, lineHeight}))
		{
			ImGui::OpenPopup("ComponentSettings");
		}
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Reset Component"))
				entity.AddOrReplaceComponent<T>();
		
			if (canDelete)
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;
			}

			ImGui::EndPopup();
		}


		// UI Function per specific component
		if (open)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 10.0f);
			uiFunction(component);
			ImGui::TreePop();
		}

		// Remove component if selected
		if (removeComponent && canDelete)
			entity.RemoveComponent<T>();

		ImGui::PopID();
	}

	void ObjectsPanel::RenderComponents(Entity entity)
	{
		// Tag Component
		ImGui::TextDisabled(ICON_FA_CUBE);
		ImGui::SameLine();
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

		// UUID 
		ImGui::SameLine();
		ImGui::TextDisabled(ICON_FA_INFO_CIRCLE);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(std::to_string((uint64_t)m_SelectedEntity.GetUUID()).c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		// Add Components Button
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 60.0f);
		if (ImGui::Button(" ADD " ICON_FA_PLUS))
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

				ImGui::Separator();
			}

			if (!m_SelectedEntity.HasComponent<MeshRendererComponent>())
			{
				if (ImGui::MenuItem("Mesh Renderer"))
				{
					m_SelectedEntity.AddComponent<MeshRendererComponent>();
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

			ImGui::Separator();

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

			ImGui::Separator();

			if (!m_SelectedEntity.HasComponent<SkyLightComponent>())
			{
				if (ImGui::MenuItem("Sky Light"))
				{
					m_SelectedEntity.AddComponent<SkyLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectedEntity.HasComponent<PointLightComponent>())
			{
				if (ImGui::MenuItem("Point Light"))
				{
					m_SelectedEntity.AddComponent<PointLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectedEntity.HasComponent<DirectionalLightComponent>())
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					m_SelectedEntity.AddComponent<DirectionalLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		// Search Components
		UI::ShiftPosY(20.0f);
		ImGui::TextDisabled(ICON_FA_SEARCH);
		ImGui::SameLine();
		static ImGuiTextFilter componentsFilter;
		componentsFilter.Draw("##Search", ImGui::GetWindowSize().x - 40.0f);
		UI::ShiftPosY(5.0f);

		// Transform Component
		if (entity.HasComponent<TransformComponent>() && componentsFilter.PassFilter("Transform"))
		{
			RenderComponent<TransformComponent>(ICON_FA_ARROWS   "  Transform", entity, false, [](auto& component)
			{
				RenderVec3Control("Position", component.Position);
				ImGui::Separator();

				glm::vec3 rotation = glm::degrees(component.Rotation);
				RenderVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				ImGui::Separator();

				RenderVec3Control("Scale", component.Scale, 1.0f);
			}, false);
		}

		// Mesh Renderer Component
		if (entity.HasComponent<MeshRendererComponent>() && componentsFilter.PassFilter("Mesh Renderer"))
		{
			RenderComponent<MeshRendererComponent>(ICON_FA_CUBES  "  Mesh Renderer", entity, true, [](auto& component)
			{
				// Texture
				ImGui::Columns(2);
				ImGui::Text("Model");
				ImGui::NextColumn();

				// Model Open Dialog
				if (ImGui::Button(component.ModelName.c_str(), ImVec2(100.0f, 0.0f)))
				{
					std::string filePath = FileDialogs::OpenFile("3D Object (*.obj, *.fbx)\0*.obj;*.fbx\0");
					if (!filePath.empty())
					{
						std::filesystem::path modelPath = filePath;
						component.Model = Model::Create(modelPath.string());
						component.ModelName = modelPath.stem().string();
						component.ModelPath = modelPath.string();
					}
				}

				ImGui::Columns(1);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
				ImGui::Separator();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

				for (const auto& [key, material] : component.Model->GetMaterials())
				{
					float nextCursorPos = ImGui::GetCursorPosX() + 20.0f;

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

					std::string label = ICON_FA_TINT " Material " + std::to_string(key);
					if (ImGui::CollapsingHeader(label.c_str()))
					{
						ImGui::SetCursorPosX(nextCursorPos);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.0f);

						// Albedo
						uint32_t albedoRenderID = material->GetAlbedoMap()->GetRendererID();
						auto& albedoColor = material->GetAlbedoColor();
						bool hasAlbedoMap = albedoRenderID != Renderer::GetDefaultTexture()->GetRendererID();
						std::string albedoLabel = "Albedo##" + std::to_string(key);
						if (ImGui::CollapsingHeader(albedoLabel.c_str()))
						{
							ImGui::SetCursorPosX(nextCursorPos + 5.0f);
							ImGui::Image(reinterpret_cast<void*>(albedoRenderID), ImVec2(64, 64));

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
								{
									const wchar_t* payloadPath = (const wchar_t*)payload->Data;
									std::filesystem::path filePath = std::filesystem::path(g_AssetPath / std::filesystem::path(payloadPath));
									TextureProperties props;
									props.Format = TextureFormat::SRGB;
									Ref<Texture2D> texture = Texture2D::Create(filePath.string(), props);
									if (texture->IsLoaded())
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
							std::string albedoColorLabel = "Color##Albedo" + std::to_string(key);
							ImGui::ColorEdit3(albedoColorLabel.c_str(), glm::value_ptr(albedoColor), ImGuiColorEditFlags_NoInputs);

							if (hasAlbedoMap)
							{
								std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetAlbedo" + std::to_string(key);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
								if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
								{
									material->ClearAlbedoMap();
								}
							}
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

						// Normal
						uint32_t normalRenderID = material->GetNormalMap()->GetRendererID();
						auto& useNormalMap = material->IsUsingNormalMap();
						bool hasNormalMap = normalRenderID != Renderer::GetDefaultTexture()->GetRendererID();
						std::string normalLabel = "Normal##" + std::to_string(key);
						if (ImGui::CollapsingHeader(normalLabel.c_str()))
						{
							ImGui::SetCursorPosX(nextCursorPos + 5.0f);
							ImGui::Image(reinterpret_cast<void*>(normalRenderID), ImVec2(64, 64));

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
								{
									const wchar_t* payloadPath = (const wchar_t*)payload->Data;
									std::filesystem::path filePath = std::filesystem::path(g_AssetPath / std::filesystem::path(payloadPath));
									Ref<Texture2D> texture = Texture2D::Create(filePath.string());
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
							std::string useNormalMapLabel = "Use##UseNormalCheck" + std::to_string(key);
							bool checkBox = useNormalMap;
							if (ImGui::Checkbox(useNormalMapLabel.c_str(), &checkBox))
							{
								material->SetUseNormalMap(checkBox);
							}

							if (hasNormalMap)
							{
								std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetNormal" + std::to_string(key);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
								if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
								{
									material->ClearNormalMap();
								}
							}
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

						// Metalness
						uint32_t metalnessRenderID = material->GetMetalnessMap()->GetRendererID();
						float& metalnessValue = material->GetMetalness();
						bool hasMetalnessMap = metalnessRenderID != Renderer::GetDefaultTexture()->GetRendererID();
						std::string metalnessLabel = "Metalness##" + std::to_string(key);
						if (ImGui::CollapsingHeader(metalnessLabel.c_str()))
						{
							ImGui::SetCursorPosX(nextCursorPos + 5.0f);
							ImGui::Image(reinterpret_cast<void*>(metalnessRenderID), ImVec2(64, 64));

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
								{
									const wchar_t* payloadPath = (const wchar_t*)payload->Data;
									std::filesystem::path filePath = std::filesystem::path(g_AssetPath / std::filesystem::path(payloadPath));
									Ref<Texture2D> texture = Texture2D::Create(filePath.string());
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
							std::string metalnessValueLabel = "##ValueMetalness" + std::to_string(key);
							ImGui::SliderFloat(metalnessValueLabel.c_str(), &metalnessValue, 0.0f, 1.0f);

							if (hasMetalnessMap)
							{
								std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetMetalness" + std::to_string(key);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
								if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
								{
									material->ClearMetalnessMap();
								}
							}
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

						// Roughness
						uint32_t roughnessRenderID = material->GetRoughnessMap()->GetRendererID();
						float& roughnessValue = material->GetRoughness();
						bool hasRoughnessMap = roughnessRenderID != Renderer::GetDefaultTexture()->GetRendererID();
						std::string roughnessLabel = "Roughness##" + std::to_string(key);
						if (ImGui::CollapsingHeader(roughnessLabel.c_str()))
						{
							ImGui::SetCursorPosX(nextCursorPos + 5.0f);
							ImGui::Image(reinterpret_cast<void*>(roughnessRenderID), ImVec2(64, 64));

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
								{
									const wchar_t* payloadPath = (const wchar_t*)payload->Data;
									std::filesystem::path filePath = std::filesystem::path(g_AssetPath / std::filesystem::path(payloadPath));
									Ref<Texture2D> texture = Texture2D::Create(filePath.string());
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
							std::string roughnessValueLabel = "##ValueRoughness" + std::to_string(key);
							ImGui::SliderFloat(roughnessValueLabel.c_str(), &roughnessValue, 0.0f, 1.0f);

							if (hasRoughnessMap)
							{
								std::string buttonLabel = ICON_FA_WINDOW_CLOSE "##ResetRoughness" + std::to_string(key);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
								if (ImGui::Button(buttonLabel.c_str(), ImVec2(18, 18)))
								{
									material->ClearRoughnessMap();
								}
							}
						}
					}
				}
			});
					}

		// Sprite Renderer Component
		if (entity.HasComponent<SpriteRendererComponent>() && componentsFilter.PassFilter("Sprite Renderer"))
		{
			RenderComponent<SpriteRendererComponent>(ICON_FA_FILE_IMAGE_O "  Sprite Renderer", entity, true, [](auto& component)
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
				ImGui::Separator();

				UI::ColorEdit4("Color", component.Color, true);
				UI::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 1.0f, 100.0f);
			});
		}

		// Circle Renderer Component
		if (entity.HasComponent<CircleRendererComponent>() && componentsFilter.PassFilter("Circle Renderer"))
		{
			RenderComponent<CircleRendererComponent>(ICON_FA_CIRCLE_O  "  Circle Renderer", entity, true, [](auto& component)
			{
				UI::ColorEdit4("Color", component.Color, true);
				UI::DragFloat("Thickness", &component.Thickness, 0.01f, 0.1f, 1.0f, true);
				UI::DragFloat("Fade", &component.Fade, 0.01f, 0.001f, 1.0f);
			});
		}

		// Camera Component
		if (entity.HasComponent<CameraComponent>() && componentsFilter.PassFilter("Camera"))
		{
			RenderComponent<CameraComponent>(ICON_FA_VIDEO_CAMERA "  Camera", entity, true, [](auto& component)
			{
				auto& camera = component.Camera;

				// Checkboxes
				UI::Checkbox("Primary Camera", &component.Primary, true);
				UI::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio, true);

				// Projection Type ComboBox
				const char* projectionType[] = { "Perspective", "Orthographic" };
				int current = (int)camera.GetProjectionType();
				if (UI::DropDown("Type", projectionType, 2, &current, true))
				{
					camera.SetProjectionType((SceneCamera::ProjectionType)current);
				}

				// Ortho Settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (UI::DragFloat("Size", &orthoSize, 0.1f, 0.1f, 1000.0f, true))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (UI::DragFloat("Near Clip", &orthoNear, 0.1f, 0.1f, 1000.0f, true))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (UI::DragFloat("Far Clip", &orthoFar, 0.1f, 0.1f, 1000.0f))
						camera.SetOrthographicFarClip(orthoFar);
				}

				// Perspective Settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (UI::DragFloat("Vertical FOV", &perspectiveFOV, 0.1f, 0.1f, 1000.0f, true))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveFOV));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (UI::DragFloat("Near Clip", &perspectiveNear, 0.1f, 0.1f, 1000.0f, true))
						camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (UI::DragFloat("Far Clip", &perspectiveFar, 0.1f, 0.1f, 1000.0f))
						camera.SetPerspectiveFarClip(perspectiveFar);
				}
			});
		}

		// Rididbody2D Component
		if (entity.HasComponent<Rigidbody2DComponent>() && componentsFilter.PassFilter("Rigidbody 2D"))
		{
			RenderComponent<Rigidbody2DComponent>(ICON_FA_ARROW_DOWN "  Rigidbody 2D", entity, true, [](auto& component)
			{
				// Projection Type ComboBox
				const char* bodyType[] = { "Static", "Dynamic", "Kinematic" };
				int current = (int)component.Type;

				if (UI::DropDown("Body Type", bodyType, 3, &current, true))
				{
					component.Type = (Rigidbody2DComponent::BodyType)current;
				}

				// Fixed rotation Checkbox
				UI::Checkbox("Fixed Rotation", &component.FixedRotation);
			});
		}

		// BoxCollider2D Component
		if (entity.HasComponent<BoxCollider2DComponent>() && componentsFilter.PassFilter("Box Collider 2D"))
		{
			RenderComponent<BoxCollider2DComponent>(ICON_FA_DROPBOX  " Box Collider 2D", entity, true, [](auto& component)
			{
				UI::DragFloat2("Offset", component.Offset, 0.1f, 0.1f, 100000.0f, true);
				UI::DragFloat2("Size", component.Size, 0.1f, 0.1f, 100000.0f, true);
				UI::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f, true);
				UI::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f, true);
				UI::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f, true);
				UI::DragFloat("Threshold", &component.RestitutionThreshold, 0.01f, 0.0f, 10.0f, true);
				UI::Checkbox("Show Area", &component.ShowArea);
			});
		}
		
		// CircleCollider2D Component
		if (entity.HasComponent<CircleCollider2DComponent>() && componentsFilter.PassFilter("Circle Collider 2D"))
		{
			RenderComponent<CircleCollider2DComponent>(ICON_FA_ARROW_CIRCLE_O_DOWN  "  Circle Collider 2D", entity, true, [](auto& component)
			{
				UI::DragFloat2("Offset", component.Offset, 0.1f, 0.1f, 100000.0f, true);
				UI::DragFloat("Radius", &component.Radius, 0.1f, 0.1f, 100000.0f, true);
				UI::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f, true);
				UI::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f, true);
				UI::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f, true);
				UI::DragFloat("Threshold", &component.RestitutionThreshold, 0.01f, 0.0f, 10.0f, true);
				UI::Checkbox("Show Area", &component.ShowArea);
			});
		}
		
		// PointLight Component 
		if (entity.HasComponent<PointLightComponent>() && componentsFilter.PassFilter("Point Light"))
		{
			RenderComponent<PointLightComponent>(ICON_FA_LIGHTBULB_O  "  Point Light", entity, true, [](auto& component)
			{
				UI::ColorEdit3("Color", component.Color, true);
				UI::DragFloat("Intensity", &component.Intensity, 0.1f, 1.0f, 100.0f, true);
				UI::DragFloat("Radius", &component.Radius, 0.1f, 0.1f, 100.0f, true);
				UI::DragFloat("Falloff", &component.Falloff, 0.1f, 0.1f, 100.0f);
			});
		}

		// DirectionalLight Component 
		if (entity.HasComponent<DirectionalLightComponent>() && componentsFilter.PassFilter("Directional Light"))
		{
			RenderComponent<DirectionalLightComponent>(ICON_FA_LIGHTBULB_O  "  Directional Light", entity, true, [](auto& component)
			{
				UI::ColorEdit3("Color", component.Color, true);
				UI::DragFloat("Intensity", &component.Intensity, 0.1f, 1.0f, 100.0f, true);
				UI::Checkbox("Casts Shadows", &component.CastsShadows);
;			});
		}

		// Sky Light Component 
		if (entity.HasComponent<SkyLightComponent>() && componentsFilter.PassFilter("Sky Light"))
		{
			RenderComponent<SkyLightComponent>(ICON_FA_LIGHTBULB_O  "  Sky Light", entity, true, [](auto& component)
			{
				// Sky Light HDR Path
				ImGui::Columns(2);
				ImGui::Text("Environment Map");
				ImGui::NextColumn();

				// EnvMap Open Dialog
				if (ImGui::Button(component.EnvironmentMapName.c_str(), ImVec2(200.0f, 0.0f)))
				{
					std::string filePath = FileDialogs::OpenFile("HDR Environment Map(*.hdr)\0*.hdr\0");
					if (!filePath.empty())
					{
						std::filesystem::path envPath = filePath;
						component.EnvironmentMap = Renderer::CreateEnvironmentMap(envPath.string());
						component.EnvironmentMapName = envPath.stem().string();
						component.EnvironmentMapPath = envPath.string();

						component.DinamicSky = false;
					}
				}
				ImGui::Columns(1);
				ImGui::Separator();

				if (component.EnvironmentMap)
				{
					// Intensity
					UI::DragFloat("Intensity", &component.Intensity, 0.01f, 0.0f, 100.0f, true);

					// Lod
					float maxLodLevel = static_cast<float>(component.EnvironmentMap->GetRadianceMap()->GetMipLevelCount());
					UI::DragFloat("LOD", &component.Lod, 0.01f, 0.0f, maxLodLevel, true);
				}

				// Dinamic Sky
				bool changedDS = UI::Checkbox("Dinamic Sky", &component.DinamicSky, component.DinamicSky);
				if (component.DinamicSky)
				{
					bool changed = UI::DragFloat("Turbidity", &component.TurbidityAzimuthInclination.x, 0.01f, 0.0f, 100.0f, true);
					changed |= UI::DragFloat("Azimuth", &component.TurbidityAzimuthInclination.y, 0.01f, 0.0f, 100.0f, true);
					changed |= UI::DragFloat("Inclination", &component.TurbidityAzimuthInclination.z, 0.01f, 0.0f, 100.0f);

					if (changed)
					{
						Ref<TextureCube> preethamSky = Renderer::CreatePreethamSky(component.TurbidityAzimuthInclination);
						component.EnvironmentMap = SceneEnvironment::Create(preethamSky, preethamSky);
					}
				}

				if (changedDS)
				{
					if (component.DinamicSky)
					{
						Ref<TextureCube> preethamSky = Renderer::CreatePreethamSky(component.TurbidityAzimuthInclination);
						component.EnvironmentMap = SceneEnvironment::Create(preethamSky, preethamSky);

						if (!component.EnvironmentMapPath.empty())
						{
							component.EnvironmentMapName = "None";
							component.EnvironmentMapPath = std::string();
						}
					}
					else
						component.EnvironmentMap = nullptr;
				}

			});
		}
	}
}