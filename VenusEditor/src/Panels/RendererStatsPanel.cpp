#include "RendererStatsPanel.h"

#include "imgui/imgui.h"

namespace Venus {

	void RendererStatsPanel::OnImGuiRender()
	{
		ImGui::Begin(ICON_FA_SIGNAL " Statistics");
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		
		auto apiName = RendererAPI::GetAPIName();
		auto stats = Renderer::GetStats();
		auto stats2D = Renderer2D::GetStats();

		// API
		ImGui::PushFont(boldFont);
		ImGui::Text("API:");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Text("%s", apiName.c_str());

		// Frametime
		/*
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
		ImGui::PushFont(boldFont);
		ImGui::Text("Frametime:");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Text("%.3fms", Renderer::GetFrameTime());
		*/

		// Renderer
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
		ImGui::PushFont(boldFont);
		ImGui::Text("Renderer Stats:");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Vertices: %d", stats.VertexCount);
		ImGui::Text("Indices: %d", stats.IndexCount);

		// Renderer 2D
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);
		ImGui::PushFont(boldFont);
		ImGui::Text("Renderer2D Stats:");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Text("Draw Calls: %d", stats2D.DrawCalls);
		ImGui::Text("Quads: %d", stats2D.QuadCount);
		ImGui::Text("Vertices: %d", stats2D.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats2D.GetTotalIndexCount());

		ImGui::End();
	}
}