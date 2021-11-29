#include "pch.h"
#include "OrthographicCameraController.h"

#include "Engine/Application.h"
#include "GLFW/glfw3.h"

namespace Venus {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		// CHANGE TO VENUS INPUT

		if (glfwGetKey(Application::Get().GetWindow().GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		{
			//CORE_LOG_TRACE("A");
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		}
		else if (glfwGetKey(Application::Get().GetWindow().GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		{
			//CORE_LOG_TRACE("D");
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;
		}

		if (glfwGetKey(Application::Get().GetWindow().GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		{
			//CORE_LOG_TRACE("W");
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		}
		else if (glfwGetKey(Application::Get().GetWindow().GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		{
			//CORE_LOG_TRACE("S");
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
		}

		if (m_Rotation)
		{
			if (glfwGetKey(Application::Get().GetWindow().GetWindow(), GLFW_KEY_Q) == GLFW_PRESS)
			{
				//CORE_LOG_TRACE("Q");
				m_CameraRotation += m_CameraRotationSpeed * ts;
			}

			if (glfwGetKey(Application::Get().GetWindow().GetWindow(), GLFW_KEY_E) == GLFW_PRESS)
			{
				//CORE_LOG_TRACE("E");
				m_CameraRotation -= m_CameraRotationSpeed * ts;
			}

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_CameraTranslationSpeed = m_ZoomLevel;
	}

}