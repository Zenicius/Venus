#include "RendererStatsPanel.h"

#include "imgui/imgui.h"

namespace Venus {

	void RendererStatsPanel::OnImGuiRender()
	{
		ImGui::Begin("Renderer Stats");
		auto stats = Renderer2D::GetStats();
		auto apiName = RendererAPI::GetAPIName();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("%s", apiName.c_str());
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();
	}
}