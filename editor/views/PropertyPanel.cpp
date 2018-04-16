/*******************************
Copyright (c) 2016-2018 Gr�goire Angerand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************/

#include "PropertyPanel.h"

#include <editor/context/EditorContext.h>

#include <imgui/imgui.h>

namespace editor {

PropertyPanel::PropertyPanel(ContextPtr cptr) : Dock("Properties"), ContextLinked(cptr) {
}


void PropertyPanel::paint_ui(CmdBufferRecorder<>&, const FrameToken&) {
	if(!context()->selected()) {
		return;
	}


	Transformable* sel = context()->selected();
	auto [pos, rot, scale] = sel->transform().decompose();

	if(ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen)) {
		float step = 1.0f;
		float big_step = 15.0f;

		ImGui::BeginGroup();
		ImGui::InputFloat("X", &pos.x(), step, big_step, 3);
		ImGui::InputFloat("Y", &pos.y(), step, big_step, 3);
		ImGui::InputFloat("Z", &pos.z(), step, big_step, 3);
		ImGui::EndGroup();

	}

	if(ImGui::CollapsingHeader("Rotation", ImGuiTreeNodeFlags_DefaultOpen)) {
		math::Vec3 euler = rot.to_euler() * math::to_deg(1.0f);
		for(auto& a : euler) {
			// remove -0.0
			if(a == 0.0f) {
				a = 0.0f;
			}
			if(a == -180.0f) {
				a = 180.0f;
			}
		}

		bool angle_changed = false;
		float step = 1.0f;
		float big_step = 15.0f;

		ImGui::BeginGroup();
		angle_changed |= ImGui::InputFloat("Pitch", &euler[math::Quaternion<>::PitchIndex], step, big_step, 3);
		angle_changed |= ImGui::InputFloat("Yaw", &euler[math::Quaternion<>::YawIndex], step, big_step, 3);
		angle_changed |= ImGui::InputFloat("Roll", &euler[math::Quaternion<>::RollIndex], step, big_step, 3);
		ImGui::EndGroup();

		// avoid recomputing angle (not always stable in euler space)
		if(angle_changed) {
			rot = math::Quaternion<>::from_euler(euler * math::to_rad(1.0f));
		}
	}

	if(Light* light = context()->selected_light()) {
		int flags = /*ImGuiColorEditFlags_NoSidePreview |
					ImGuiColorEditFlags_NoSmallPreview |*/
					ImGuiColorEditFlags_NoAlpha |
					ImGuiColorEditFlags_Float |
					ImGuiColorEditFlags_RGB;

		if(ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
			math::Vec4 color(light->color(), 1.0f);
			if(ImGui::ColorButton("Color", color, flags/*, ImVec2(40, 40)*/)) {
				ImGui::OpenPopup("Color");
			}
			ImGui::SameLine();
			ImGui::Text("Light color");
		}

		if(ImGui::BeginPopup("Color")) {
			ImGui::ColorPicker3("##lightpicker", light->color().begin(), flags);
			ImGui::EndPopup();
		}

		ImGui::InputFloat("Intensity", &light->intensity(), 1.0f, 10.0f, 3);
		ImGui::InputFloat("Radius", &light->radius(), 1.0f, 10.0f, 3);


	}

	sel->transform() = math::Transform<>(pos, rot, scale);

	//ImGui::Text("Position: %f, %f, %f", sel->position().x(), sel->position().y(), sel->position().z());

}

}
