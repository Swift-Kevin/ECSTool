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

	void DrawReadOnlyIntValue(const std::string& _label, int _val, float _dragWidth, ImVec4 labelColor)
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
		ImGui::BeginDisabled();
		ImGui::InputInt(("##" + _label).c_str(), &_val, 0, 0);
		ImGui::EndDisabled();
		ImGui::PopItemWidth();

		ImGui::EndGroup();
	}

	void ConstructTransformInpsector(GAME::Transform& _transform)
	{
		UI::DrawVec3Control("Position", _transform.localTranslation);
		UI::DrawVec3Control("Rotation", _transform.localRotation);
		UI::DrawVec3Control("Scale", _transform.localScale);
	}

	void DrawEntityNode(entt::registry& registry, entt::entity entity, std::unordered_map<entt::entity, std::vector<entt::entity>>& hierarchy, UI::UIData& uiData)
	{
		auto& inspec = registry.get<GAME::Inspectable>(entity);
		bool hasChildren = hierarchy.find(entity) != hierarchy.end();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanAvailWidth;

		if (!hasChildren)
		{
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		if (uiData.inspectingEntity == entity)
		{
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
		}

		if (registry.get<GAME::Transform>(entity).parentID == entt::null)
		{
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen;
		}

		std::string label = inspec.name + " { E:" + std::to_string((ENTT_ID_TYPE)entity) + " }";
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", label.c_str());

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(entt::entity));
			ImGui::Text("%s", label.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(("EntityPopup" + std::to_string((ENTT_ID_TYPE)entity)).c_str());
		}

		if (ImGui::BeginPopup(("EntityPopup" + std::to_string((ENTT_ID_TYPE)entity)).c_str()))
		{
			if (ImGui::MenuItem("Unparent"))
			{
				auto& transform = registry.get<GAME::Transform>(entity);
				transform.parentID = entt::null;
			}

			ImGui::EndPopup();
		}


		if (ImGui::IsItemClicked())
		{
			uiData.inspectingEntity = entity;
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY");
			if (payload)
			{
				entt::entity draggedEntity = *(entt::entity*)payload->Data;
				entt::entity currentEntity = entity;
				bool subChild = false;

				while (!subChild && currentEntity != entt::null)
				{
					if (registry.get<GAME::Transform>(currentEntity).parentID == draggedEntity)
					{
						subChild = true;
						currentEntity = entt::null;
					}
					else
					{
						currentEntity = registry.get<GAME::Transform>(currentEntity).parentID;
					}
				}

				if (draggedEntity != entity && !subChild)
				{
					auto& transform = registry.get<GAME::Transform>(draggedEntity);
					transform.parentID = entity == entt::null ? entt::null : entity;
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (opened && hasChildren)
		{
			for (auto child : hierarchy[entity])
			{
				DrawEntityNode(registry, child, hierarchy, uiData);
			}

			ImGui::TreePop();
		}
	}

}; // namespace UI
