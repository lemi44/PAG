#include "GUI.h"
#include "imgui.h"

std::string const GUI::labels[4] = { "World Translation##value", "Translate##value", "Rotate##value", "Scale##value" };
float const GUI::min[4] = { -50.0f, -50.0f, -glm::pi<float>(), 0.0f };
float const GUI::max[4] = { 50.0f, 50.0f, glm::pi<float>(), 2.0f };

void GUI::ShowPropertyEditor(bool* p_open, std::vector<Model>& all_models)
{
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Property editor", p_open))
	{
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	ImGui::Columns(2);
	ImGui::Separator();



	// Iterate dummy objects with dummy members (all the same data)
	for (auto &mdl : all_models)
		ShowObject("Object", &mdl);

	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::PopStyleVar();
	ImGui::End();
}

void GUI::ShowObject(const char* prefix, Model * mdl)
{
	ImGui::PushID(mdl->getID());                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
	ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
	bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, mdl->getID());
	ImGui::NextColumn();
	ImGui::AlignTextToFramePadding();
	
	ImGui::NextColumn();
	if (node_open)
	{
		int i;
		for (i = 0; i < mdl->getChildren().size(); i++)
		{
			ImGui::PushID(i);
			ShowObject("Child", &mdl->getChildren()[i]);
			ImGui::PopID();
		}
		glm::mat4 matrix = mdl->world.getMatrix();
		glm::vec3 world(matrix[3][0], matrix[3][1], matrix[3][2]);

		std::vector<float*> refs = { reinterpret_cast<float*>(&world), reinterpret_cast<float*>(&mdl->pos), reinterpret_cast<float*>(&mdl->rot), reinterpret_cast<float*>(&mdl->scale) };
		for (auto j = 0; j < 4; j++)
		{
			ImGui::PushID(++i); // Use field index as identifier.
			ImGui::AlignTextToFramePadding();
			// Here we use a Selectable (instead of Text) to highlight on hover
			//ImGui::Text("Field_%d", i);
			ImGui::Bullet();
			ImGui::Selectable(labels[j].c_str());
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat3(labels[j].c_str(), refs[j], min[j], max[j]);

			ImGui::PopItemWidth();
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}
