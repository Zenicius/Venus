#include "pch.h"
#include "ConsolePanel.h"

#include "ImGui/UI.h"

namespace Venus {

	static ConsolePanel* s_Instance = nullptr;

	ConsolePanel::ConsolePanel()
	{
		s_Instance = this;
	}

	ConsolePanel::~ConsolePanel()
	{
		s_Instance = nullptr;
	}

	void ConsolePanel::OnImGuiRender(bool& open)
	{
		if (!open)
			return;

		ImGui::Begin(ICON_FA_TERMINAL " Console", &open);
		RenderMenu();
		ImGui::End();
	}

	void ConsolePanel::OnScenePlay()
	{
		if(m_ClearOnPlay)
			m_MessageBufferIndex = 0;
	}

	void ConsolePanel::RenderMenu()
	{
		// Category Count
		uint32_t infoCount = 0, warningCount = 0, errorCount = 0;
		for (uint32_t i = 0; i < m_MessageBufferIndex; i++)
		{
			const auto& msg = m_MessageBuffer[i];
			switch (msg.GetCategory())
			{
				case Category::Info: infoCount++; break;
				case Category::Warning: warningCount++; break;
				case Category::Error: errorCount++; break;
			}
		}
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 0.5f });
		UI::ShiftPosX(15.0f);
		UI::Text(ICON_FA_INFO ": " + std::to_string(infoCount));
		ImGui::SameLine();
		UI::ShiftPosX(5.0f);
		UI::Text(ICON_FA_EXCLAMATION_TRIANGLE ": " + std::to_string(warningCount));
		ImGui::SameLine();
		UI::ShiftPosX(5.0f);
		UI::Text(ICON_FA_BUG ": " + std::to_string(errorCount));
		ImGui::PopStyleColor();

		// Filter
		ImGui::SameLine();
		UI::ShiftPosX(25.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 0.5f });
		UI::Text(ICON_FA_FILTER " Filter");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		UI::ShiftPosX(5.0f);
		ImGui::SetNextItemWidth(140.0f);
		const char* filter[] = { "None", ICON_FA_INFO " Info", ICON_FA_EXCLAMATION_TRIANGLE " Warning", ICON_FA_BUG " Error" };
		UI::DropDown("ConsoleFilter", filter, 4, &m_SelectedFilter, false, false, false);

		// Options
		ImGui::SameLine();
		UI::ShiftPosX(20.0f);
		if (ImGui::Button("Clear"))
			m_MessageBufferIndex = 0;

		ImGui::SameLine();
		UI::ShiftPosX(20.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 0.5f });
		UI::Text("Clear on Play");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Checkbox("##ClearOnPlay", &m_ClearOnPlay);

		// Search
		ImGui::SameLine();
		UI::SetPosX(ImGui::GetWindowContentRegionMax().x - 300.0f);
		ImGui::TextDisabled(ICON_FA_SEARCH);
		ImGui::SameLine();
		static ImGuiTextFilter consoleFilter;
		consoleFilter.Draw("##Search", 270.0f);

		ImGui::Separator();
		RenderConsole(consoleFilter);
	}

	void ConsolePanel::RenderConsole(ImGuiTextFilter filter)
	{
		ImGui::BeginChild("Messages");

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

		for (uint32_t i = 0; i < m_MessageBufferIndex; i++)
		{
			const auto& msg = m_MessageBuffer[i];

			if (!filter.PassFilter(msg.GetMessage().c_str()) || (m_SelectedFilter != 0 && (Category)m_SelectedFilter != msg.GetCategory()))
				continue;

			std::string finalMsg;
			std::string icon;
			ImVec4 categoryColor;
			switch (msg.GetCategory())
			{
				case Category::Info: 
				{
					icon = ICON_FA_INFO; 
					categoryColor = { 1.0f, 1.0f, 1.0f, 0.75f };
					break;
				}

				case Category::Warning: 
				{
					icon = ICON_FA_EXCLAMATION_TRIANGLE; 
					categoryColor = { 0.7f, 0.7f, 0.3f, 0.75f };
					break;
				}

				case Category::Error:
				{
					icon = ICON_FA_BUG;
					categoryColor = { 0.7f, 0.1f, 0.1f, 0.75f };
					break;
				}
			}
			finalMsg = icon + " " + msg.GetMessage();

			ImGui::PushStyleColor(ImGuiCol_Text, categoryColor);
			bool open = ImGui::TreeNodeEx((void*)i, flags, finalMsg.c_str());
			ImGui::PopStyleColor();

			// Right Click Menu
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Copy"))
					ImGui::SetClipboardText(msg.GetMessage().c_str());

				ImGui::EndPopup();
			}

			ImGui::Separator();

			if(open)
				ImGui::TreePop();
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}

	void ConsolePanel::PushMessage(const ConsoleMessage& message)
	{
		if (s_Instance == nullptr || message.GetCategory() == Category::None)
			return;

		s_Instance->m_MessageBuffer[s_Instance->m_MessageBufferIndex++] = message;

		if (s_Instance->m_MessageBufferIndex == s_MessageBufferCapacity)
			s_Instance->m_MessageBufferIndex = 0;
	}

}