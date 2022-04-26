#pragma once

// Engine
#include "Engine/Base.h"
#include "Engine/Assert.h"
#include "Engine/Application.h"
#include "Engine/Log.h"
#include "Engine/Layer.h"
#include "Engine/Timestep.h"
#include "Engine/KeyCodes.h"
#include "Engine/MouseCodes.h"
#include "Engine/Input.h"
#include "ImGui/ImGuiLayer.h"

// Assets
#include "Assets/AssetSerializer.h"

// Events
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

// Scene
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneSerializer.h"

// Scripting
#include "Scene/Scripting/ScriptableEntity.h"

// Renderer
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/OrthographicCameraController.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Particle/ParticleSystem.h"

// Particles
#include "Renderer/Particle/Particle.h"

// Utils
#include "Utils/Random.h"
#include "Utils/PlatformUtils.h"
#include "Math/Math.h"
#include "ImGui/IconsFontAwesome.h"
