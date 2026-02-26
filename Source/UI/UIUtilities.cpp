#include "UIUtilities.h"

namespace UI
{
	void DrawFloatControl(const std::string& _label, float& _val, float _dragWidth, ImVec4 labelColor)
	{
        ImGui::BeginGroup();
        float textWidth = ImGui::CalcTextSize(_label.c_str()).x;
        float buttonWidth = textWidth + 10.0f;

        ImGui::PushStyleColor(ImGuiCol_Button, labelColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, labelColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, labelColor);
        ImGui::Button(_label.c_str(), ImVec2(buttonWidth, 0));
        ImGui::PopStyleColor(3);

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::PushItemWidth(_dragWidth);
        ImGui::DragFloat(("##" + _label).c_str(), &_val, 0.1f);
        ImGui::PopItemWidth();
        ImGui::EndGroup();

	}

	void DrawVec3Control(const std::string& label, GW::MATH::GVECTORF& values, float resetValue, float columnWidth)
	{
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        float fullWidth = ImGui::GetContentRegionAvail().x;
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        // Calculate button widths for each label
        float buttonX = ImGui::CalcTextSize("X").x + 10.0f;
        float buttonY = ImGui::CalcTextSize("Y").x + 10.0f;
        float buttonZ = ImGui::CalcTextSize("Z").x + 10.0f;

        // Remaining width for drags
        float dragWidth = (fullWidth - buttonX - buttonY - buttonZ - spacing * 2.0f) / 3.0f;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, 0));
        DrawFloatControl("X", values.x, dragWidth, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
        ImGui::SameLine();
        DrawFloatControl("Y", values.y, dragWidth, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::SameLine();
        DrawFloatControl("Z", values.z, dragWidth, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
	}

}; // namespace UI
