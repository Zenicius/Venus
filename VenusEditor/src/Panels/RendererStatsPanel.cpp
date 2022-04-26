#include "RendererStatsPanel.h"

#include "imgui/imgui.h"

namespace Venus {

	void RendererStatsPanel::OnImGuiRender()
	{
		ImGui::Begin(ICON_FA_SIGNAL " Statistics");
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		auto stats = Renderer2D::GetStats();
		auto apiName = RendererAPI::GetAPIName();
		
		// API
		ImGui::PushFont(boldFont);
		ImGui::Text("API:");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Text("%s", apiName.c_str());

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15.0f);

		// Renderer 2D
		ImGui::PushFont(boldFont);
		ImGui::Text("Renderer2D Stats:");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();
	}
}