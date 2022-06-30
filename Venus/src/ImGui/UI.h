#pragma once

#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGui/IconsFontAwesome.h"
#include <glm/glm/gtc/type_ptr.hpp>

#define TEXT_COLOR { 1.0, 1.0, 1.0, 1.0 }

namespace Venus::UI {

	//- Aux.-----------------------------------------------------------------------------------

	static void ShiftPosX(float offset)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	}

	static void ShiftPosY(float offset)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset);
	}

	static void ShiftPos(float x, float y)
	{
		ShiftPosX(x);
		ShiftPosY(y);
	}

	static void SetPosX(float pos)
	{
		ImGui::SetCursorPosX(pos);
	}

	static void SetPosY(float pos)
	{
		ImGui::SetCursorPosY(pos);
	}

	static void CenterText(const std::string& text)
	{
		float textSizeX = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSizeX) * 0.5f);
	}


	//- Widgets--------------------------------------------------------------------------------

	static void Text(const std::string& text, bool bold = false, bool separator = false)
	{
		if (bold)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
		}

		ImGui::Text(text.c_str());

		if (bold)
			ImGui::PopFont();

		if (separator)
			ImGui::Separator();
	}

	static bool InputText(const std::string& label, std::string& value, bool separator = false)
	{
		std::string inputID = "##" + label;
		bool changed = false;

		ImGui::Columns(2);
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		char buffer[256];
		strcpy_s<256>(buffer, value.c_str());

		if (ImGui::InputText(inputID.c_str(), buffer, 256))
		{
			value = buffer;
			changed = true;
		}

		ImGui::Columns(1);

		if (separator)
			ImGui::Separator();

		return changed;
	}

	static bool SliderInt(const std::string& label, int* value, int min, int max, bool separator = false)
	{
		std::string sliderID = "##" + label;
		bool changed = false;

		ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR);
		ImGui::Columns(2);
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::NextColumn();
		changed = ImGui::SliderInt(sliderID.c_str(), value, min, max);
		ImGui::Columns(1);
		if (separator)
			ImGui::Separator();
		ImGui::PopStyleColor();

		return changed;
	}

	static bool Checkbox(const std::string& label, bool* value, bool separator = false)
	{
		std::string checkboxID = "##" + label;
		bool checked = false;

		ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR);
		ImGui::Columns(2);
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::NextColumn();
		checked = ImGui::Checkbox(checkboxID.c_str(), value);
		ImGui::Columns(1);
		if(separator)
			ImGui::Separator();
		ImGui::PopStyleColor();

		return checked;
	}

	static bool DragFloat(const std::string& label, float* value, float speed = 0.1f, float min = 0.0f, float max = 10000.0f, bool separator = false)
	{
		std::string dragfloatID = "##" + label;
		bool changed = false;
		
		ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR);
		ImGui::Columns(2);
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::NextColumn();
		changed = ImGui::DragFloat(dragfloatID.c_str(), value, speed, min, max);
		ImGui::Columns(1);
		if (separator)
			ImGui::Separator();
		ImGui::PopStyleColor();

		return changed;
	}

	static bool DragFloat2(const std::string& label, glm::vec2& value, float speed = 0.1f, float min = 0.0f, float max = 10000.0f, bool separator = false)
	{
		std::string dragfloatID = "##" + label;
		bool changed = false;

		ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR);
		ImGui::Columns(2);
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::NextColumn();
		changed = ImGui::DragFloat2(dragfloatID.c_str(), glm::value_ptr(value), speed, min, max);
		ImGui::Columns(1);
		if (separator)
			ImGui::Separator();
		ImGui::PopStyleColor();

		return changed;
	}

	static bool ColorEdit3(const std::string& label, glm::vec3& color, bool separator = false)
	{
		std::string colorEditID = "##" + label;
		bool changed = false;


		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs;
		ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR);
		ImGui::Columns(2);
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::NextColumn();
		changed = ImGui::ColorEdit3(colorEditID.c_str(), glm::value_ptr(color), flags);
		ImGui::Columns(1);
		if (separator)
			ImGui::Separator();
		ImGui::PopStyleColor();

		return changed;
	}

	static bool ColorEdit4(const std::string& label, glm::vec4& color, bool separator = false)
	{
		std::string colorEditID = "##" + label;
		bool changed = false;

		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs;
		ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR);
		ImGui::Columns(2);
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		ImGui::NextColumn();
		changed = ImGui::ColorEdit4(colorEditID.c_str(), glm::value_ptr(color), flags);
		ImGui::Columns(1);
		if (separator)
			ImGui::Separator();
		ImGui::PopStyleColor();

		return changed;
	}

	static bool DropDown(const std::string& label, const char** options, int count, int* selected, bool separator = false, bool drawLabel = true, bool noDefaultArrow = false)
	{
		const char* current = options[*selected];
		std::string preview = current;
		std::string dropdownID = "##" + label;
		bool changed = false;

		if (drawLabel)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COLOR);
			ImGui::Columns(2);
			ImGui::Text(label.c_str());
			ImGui::SameLine();
			ImGui::NextColumn();
		}

		ImGuiComboFlags flags = 0;
		if (noDefaultArrow)
		{
			flags |= ImGuiComboFlags_NoArrowButton;
			preview += " " ICON_FA_CARET_DOWN;
		}

		if (ImGui::BeginCombo(dropdownID.c_str(), preview.c_str(), flags))
		{
			for (int i = 0; i < count; i++)
			{
				const bool is_selected = (current == options[i]);
				if (ImGui::Selectable(options[i], is_selected))
				{
					current = options[i];
					*selected = i;
					changed = true;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (drawLabel)
		{
			ImGui::Columns(1);
			ImGui::PopStyleColor();
		}

		if (separator)
			ImGui::Separator();

		return changed;
	}
}