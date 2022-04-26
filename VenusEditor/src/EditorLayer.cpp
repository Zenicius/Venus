#include "EditorLayer.h"

#include <imgui/imgui.h>
#include "imgui/imgui_internal.h"
#include <ImGuizmo/ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Venus {

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		// Framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		// Scene
		m_EditorScene = CreateRef<Scene>();		
		m_ActiveScene = m_EditorScene;
		m_EditorCamera = EditorCamera(30.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);

		// Panels
		m_ObjectsPanel.SetContext(m_ActiveScene);
		m_AssetBrowserPanel.SetContext(m_ActiveScene);

		m_PlayIcon = Texture2D::Create("Resources/Icons/Toolbar/play.png");
		m_StopIcon = Texture2D::Create("Resources/Icons/Toolbar/stop.png");
		m_GizmosPositionIcon = Texture2D::Create("Resources/Icons/Toolbar/position.png");
		m_GizmosRotationIcon = Texture2D::Create("Resources/Icons/Toolbar/rotation.png");
		m_GizmosScaleIcon = Texture2D::Create("Resources/Icons/Toolbar/scale.png");
		m_SceneCameraIcon = Texture2D::Create("Resources/Icons/Scene/camera.png");

		UpdateWindowTitle(m_EditorScene->m_SceneName);
	}

	void EditorLayer::OnDetach() {}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();

		RenderCommand::SetClearColor(m_ClearColor);
		RenderCommand::Clear();
		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				m_EditorCamera.OnUpdate(ts);
				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}

		OnOverlayRender();
		UpdateHoveredEntity();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		VS_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = 350.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = 64.0f;

		// UI
		UI_ModalWelcome();
		UI_MenuBar();
		UI_ToolBar();
		UI_Viewport();
		UI_Settings();

		if(m_ShowWelcomeMessage)
			ImGui::OpenPopup("Welcome");

		// Panels
		m_ObjectsPanel.OnImGuiRender();
		m_AssetBrowserPanel.OnImGuiRender();
		m_RendererStatsPanel.OnImGuiRender();

		ImGui::End();
	}

	void EditorLayer::UI_ModalWelcome()
	{
		if (ImGui::BeginPopupModal("Welcome", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Welcome to Venus Engine!");
			ImGui::Text("This engine is currently in its initial stage of development.");
			ImGui::Text("It may contain bugs and led to loss of work.");
			ImGui::Text("But thank you for your interest in Venus!");
			ImGui::Separator();
			if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
			m_ShowWelcomeMessage = false;
		}
	}

	void EditorLayer::UI_MenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			// File
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FILE_O "  New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem(ICON_FA_FILE  "  Open...", "Ctrl+O"))
					OpenScene();;
				
				if (ImGui::MenuItem(ICON_FA_FLOPPY_O  "  Save", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem(ICON_FA_FLOPPY_O "  Save as...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();


				ImGui::EndMenu();
			}

			// Edit
			if (ImGui::BeginMenu("Edit", false))
			{

				ImGui::EndMenu();
			}

			// Create 
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Empty Object"))
				{
					auto entity = m_ActiveScene->CreateEntity();
					m_ObjectsPanel.SetSelectedEntity(entity);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Sprite"))
				{
					auto entity = m_ActiveScene->CreateEntity("Sprite");
					entity.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
					m_ObjectsPanel.SetSelectedEntity(entity);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Circle"))
				{
					auto entity = m_ActiveScene->CreateEntity("Circle");
					entity.AddComponent<CircleRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
					m_ObjectsPanel.SetSelectedEntity(entity);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Camera"))
				{
					auto entity = m_ActiveScene->CreateEntity("Camera");
					entity.AddComponent<CameraComponent>();
					m_ObjectsPanel.SetSelectedEntity(entity);
				}

				ImGui::EndMenu();
			}
			
			// View
			if (ImGui::BeginMenu("View", false))
			{
				ImGui::EndMenu();
			}

			// Help
			if (ImGui::BeginMenu("Help", false))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::UI_ToolBar()
	{
		// Toolbar settings
		auto& colors = ImGui::GetStyle().Colors;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y, 
							  colors[ImGuiCol_Button].z, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_ButtonHovered].x, colors[ImGuiCol_ButtonHovered].y,
			colors[ImGuiCol_ButtonHovered].z, 0.3f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

		// Play // Stop
		float size = ImGui::GetWindowHeight() - 10.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_PlayIcon : m_StopIcon;
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleColor(2);

		float startPos = 0.19f;

		// Gizmos Position TODO: Better way to handle button activation
		ImGui::SameLine();
		ImVec4 selectedColor = { 0.85f, 0.8505f, 0.851f, 1.0f };
		ImVec4 unselectedColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		ImVec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos));
		if (m_GizmoType == ImGuizmo::OPERATION::TRANSLATE)
			color = selectedColor;
		else
			color = unselectedColor;
		if (ImGui::ImageButton((ImTextureID)m_GizmosPositionIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, color))
		{
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		}

		// Gizmos Rotation
		ImGui::SameLine();
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			color = selectedColor;
		else
			color = unselectedColor;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos) + size * 1.5);
		if (ImGui::ImageButton((ImTextureID)m_GizmosRotationIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, color))
		{
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		}

		// Gizmos Scale
		ImGui::SameLine();
		if (m_GizmoType == ImGuizmo::OPERATION::SCALE)
			color = selectedColor;
		else
			color = unselectedColor;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos) + (size * 1.5) * 2);
		if (ImGui::ImageButton((ImTextureID)m_GizmosScaleIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, color))
		{
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
		}

		// Scene Name Temp
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y,
			colors[ImGuiCol_Button].z, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y,
			colors[ImGuiCol_Button].z, 1.0f));
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.9);
		ImGui::SetCursorPosY(0.0f);
		ImGui::Button(m_ActiveScene->m_SceneName.c_str());
		ImGui::PopStyleColor(2);

		ImGui::PopStyleVar(2);
		ImGui::End();
	}

	void EditorLayer::UI_Viewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		glm::vec2 viewportPanelSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
		m_ViewportSize = viewportPanelSize;

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// Viewport Drag and Drop
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCENE"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::string name = std::filesystem::path(path).stem().string();
				Entity entity = m_ActiveScene->CreateEntity(name);
				auto& src = entity.AddComponent<SpriteRendererComponent>();

				std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
				src.Texture = Texture2D::Create(texturePath.string());
				src.TextureName = texturePath.stem().string();
				src.TexturePath = texturePath.string();

				m_ObjectsPanel.SetSelectedEntity(entity);
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FREFAB"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				// TODO Maybe move to function
				SceneSerializer serializer(m_ActiveScene);
				Entity entity = serializer.DeserializePrefab((std::filesystem::path(g_AssetPath) / path).string());
				m_ObjectsPanel.SetSelectedEntity(entity);
			}
		}

		// Gizmos TEMP
		Entity selectedEntity = m_ObjectsPanel.GetSelectedEntity();
		if (m_SceneState == SceneState::Edit && selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity
			auto& transformCompenent = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = transformCompenent.GetTransform();

			// Snapping 
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				Math::DecomposeTransform(transform, position, rotation, scale);

				glm::vec3 deltaRotation = rotation - transformCompenent.Rotation;

				transformCompenent.Position = position;
				transformCompenent.Rotation += deltaRotation;
				transformCompenent.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::UI_Settings()
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::Begin(ICON_FA_COGS " Settings");

		// Render
		ImGui::PushFont(boldFont);
		ImGui::Text("Render");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
		ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

		// Editor
		ImGui::PushFont(boldFont);
		ImGui::Text("Editor");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
		ImGui::Checkbox("Show Camera Icon", &m_ShowCameraIcon);
		if (ImGui::Checkbox("Lock Camera Rotation", &m_CameraLocked))
			m_EditorCamera.SetCameraLocked(m_CameraLocked);
		ImGui::Checkbox("Show Physics Colliders in Editor", &m_ShowPhysicsColliderEditor);
		ImGui::Checkbox("Show Physics Colliders in Runtime", &m_ShowPhysicsColliderRuntime);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

		// Physics
		ImGui::PushFont(boldFont);
		ImGui::Text("Physics Iterations");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
		ImGui::DragInt("Velocity", (int*)&m_ActiveScene->m_VelocityIterations, 1.0f, 1.0f, 1000.0f);
		ImGui::DragInt("Position", (int*)&m_ActiveScene->m_PositionIterations, 1.0f, 1.0f, 1000.0f);

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VS_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(VS_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	void EditorLayer::NewScene()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		m_ObjectsPanel.SetContext(m_EditorScene);
		m_AssetBrowserPanel.SetContext(m_EditorScene);

		UpdateWindowTitle(m_EditorScene->m_SceneName);

		m_ScenePath = std::string();

		m_ActiveScene = m_EditorScene;
	}

	void EditorLayer::OpenScene()
	{
		std::string filePath = FileDialogs::OpenFile("Venus Scene (*.venus)\0*.venus\0");

		if (!filePath.empty())
		{
			OpenScene(filePath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		SceneSerializer serializer(m_EditorScene);
		serializer.Deserialize(path.string());

		m_ScenePath = path.string();

		UpdateWindowTitle(m_EditorScene->m_SceneName);

		m_ObjectsPanel.SetContext(m_EditorScene);
		m_AssetBrowserPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;	
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filePath = FileDialogs::SaveFile("Venus Scene (*.venus)\0*.venus\0");

		if (!filePath.empty())
		{
			int nameStart = filePath.find_last_of("\\") + 1;
			m_EditorScene->m_SceneName = filePath.substr(nameStart, filePath.size() - nameStart - 6);

			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(filePath);

			UpdateWindowTitle(m_EditorScene->m_SceneName);

			m_ScenePath = filePath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_ScenePath.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(m_ScenePath);
		}
		else
			SaveSceneAs();
	}

	void EditorLayer::OnScenePlay()
	{
		m_RuntimeScene = Scene::Copy(m_EditorScene);
		m_RuntimeScene->OnRuntimeStart();
		m_ObjectsPanel.SetContext(m_RuntimeScene);
		m_AssetBrowserPanel.SetContext(m_RuntimeScene);

		m_ActiveScene = m_RuntimeScene;

		m_SceneState = SceneState::Play;
	}

	void EditorLayer::OnSceneStop()
	{
		m_RuntimeScene->OnRuntimeStop();
		m_RuntimeScene = nullptr; 
		m_ObjectsPanel.SetContext(m_EditorScene);
		m_AssetBrowserPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;

		m_SceneState = SceneState::Edit;
	}

	void EditorLayer::OnOverlayRender()
	{
		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				Renderer2D::BeginScene(m_EditorCamera);
				break;
			}

			case SceneState::Play:
			{
				Entity cameraEntity = m_ActiveScene->GetPrimaryCamera();
				if (cameraEntity)
				{
					auto camera = cameraEntity.GetComponent<CameraComponent>().Camera;
					auto transform = cameraEntity.GetComponent<TransformComponent>().GetTransform();
					Renderer2D::BeginScene(camera, transform);
				}
				break;
			}
		}

		// Physics Colliders
		if ((m_SceneState == SceneState::Edit && m_ShowPhysicsColliderEditor) ||
			(m_SceneState == SceneState::Play && m_ShowPhysicsColliderRuntime))
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, collider] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Position + glm::vec3(collider.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(collider.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, { 0.0f, 1.0f, 0.0f, 1.0f });
				}
			}

			// Circles Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();

				float zIndex = 0.001f;
				glm::vec3 cameraForwardDirection = m_EditorCamera.GetForwardDirection();
				glm::vec3 projectionCollider = cameraForwardDirection * glm::vec3(zIndex);

				for (auto entity : view)
				{
					auto [tc, collider] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Position + glm::vec3(collider.Offset, -projectionCollider.z);
					glm::vec3 scale = tc.Scale * glm::vec3(collider.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.05f);
				}
			}
		}

		// Camera Icon in Editor
		if (m_SceneState != SceneState::Play && m_ShowCameraIcon)
		{
			auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				const auto& tc = view.get<TransformComponent>(entity);

				Renderer2D::DrawQuad(tc.GetTransform(), m_SceneCameraIcon, 1.0f, glm::vec4(1.0f), (int)entity);
			}
		}

		// Selected Entity in Editor
		if (m_SceneState == SceneState::Edit && m_ObjectsPanel.GetSelectedEntity())
		{
			Entity entity = m_ObjectsPanel.GetSelectedEntity();
			const auto& tc = entity.GetComponent<TransformComponent>();

			glm::vec3 translation = tc.Position;
			glm::vec3 scale = { tc.Scale.x + 0.1f, tc.Scale.y + 0.1f, tc.Scale.z };

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
				* glm::toMat4(glm::quat(tc.Rotation))
				* glm::scale(glm::mat4(1.0f), scale);

			Renderer2D::DrawRect(transform, { 0.2f, 0.3f, 0.9f, 1.0f });
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::UpdateWindowTitle(const std::string& sceneName)
	{
		std::string rendererApi = RendererAPI::GetAPIName();
		Application::Get().GetWindow().SetWindowTitle(sceneName + ".venus - Venus Editor - " + rendererApi);
	}

	void EditorLayer::UpdateHoveredEntity()
	{
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0 || m_SceneState == SceneState::Play)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
			// File Dialogs
			case Key::N:
			{
				if (control)
				{
					NewScene();
				}

				break;
			}

			case Key::O:
			{
				if (control)
				{
					OpenScene();
				}

				break;
			}

			case Key::S :
			{
				if (control && shift)
				{
					SaveSceneAs();
				}
				else if (control)
				{
					SaveScene();
				}

				break;
			}

			// Gizmos
			case Key::Q :
				m_GizmoType = -1;
				break;
			
			case Key::W :
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;

			case Key::E :
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;

			case Key::R :
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			

			// Entity 
			case Key::D :
			{
				if (control && m_ObjectsPanel.GetSelectedEntity())
				{
					Entity duplicated = m_ActiveScene->DuplicateEntity(m_ObjectsPanel.GetSelectedEntity());
					m_ObjectsPanel.SetSelectedEntity(duplicated);
					break;
				}
			}
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_SceneState == SceneState::Edit)
			{
				if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftShift))
				{
					m_ObjectsPanel.SetSelectedEntity(m_HoveredEntity);
				}
			}
		}

		return false;
	}
}