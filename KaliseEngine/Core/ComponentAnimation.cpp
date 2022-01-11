#include "ComponentAnimation.h"
#include "imgui\imgui.h"
#include "Data.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ModuleFileSystem.h"

#include "ModuleEditor.h"

#include "Time.h"

bool Animation::Advance(float dt)
{
	time += dt;

	if (time >= GetDuration())
	{
		if (loopable == false)
		{
			//So we keep last frame
			time = GetDuration();
			return false;
		}
		else
		{
			time = time - GetDuration();
		}
	}
	return true;
}

float Animation::GetDuration()
{
	return ((float)end_frame - (float)start_frame) / ticks_per_second;
}

void Animation::SetFrameRatio(float ratio)
{
	if (ratio >= 0 && ratio <= 1)
		time = GetDuration() * ratio;
}

ComponentAnimation::ComponentAnimation(GameObject* parent) : Component(parent)
{

}

ComponentAnimation::~ComponentAnimation()
{

}

//Base component behaviour ------------------
void ComponentAnimation::OnGui()
{
	if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Playing", &playing))
		{
		}

		ImGui::Text("Lock animation frame");
		static float ratio;
		if (ImGui::SliderFloat("##frameSlider", &ratio, 0.0f, 1.0f))

		ImGui::Text("Animations size: %i", animations.size());
		ImGui::Separator();
		ImGui::Separator();
		for (uint i = 0; i < animations.size(); i++)
		{
			if (renaming_animation == i)
			{
				static char new_name[128];
				if (ImGui::InputText("##", new_name, 128, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					animations[renaming_animation].name = new_name;
					renaming_animation = -1;
					new_name[0] = '\0';
				}
			}
			else
				ImGui::Text(animations[i].name.c_str());
			if (ImGui::IsItemClicked(1))
			{
				popup_animation = i;
				ImGui::OpenPopup("AnimPopup");
			}

			ImGui::Separator();

			std::string loop_label = std::string("Loop##") + std::string(std::to_string(i));
			ImGui::Checkbox(loop_label.c_str(), &animations[i].loopable);

			bool isCurrent = (&animations[i] == current_animation);
			std::string current_label = std::string("CurrentAnimation##") + std::string(std::to_string(i));

			if (ImGui::Checkbox(current_label.c_str(), &isCurrent))
			{
				if (isCurrent == true)
				{
				}
			}

			int start_frame = animations[i].start_frame;
			std::string startF_label = std::string("Start Frame##") + std::string(std::to_string(i));
			if (ImGui::InputInt(startF_label.c_str(), &start_frame))
			{
				if (start_frame >= 0)
					animations[i].start_frame = start_frame;
			}

			int end_frame = animations[i].end_frame;
			std::string endF_label = std::string("End Frame##") + std::string(std::to_string(i));
			if (ImGui::InputInt(endF_label.c_str(), &end_frame))
			{
				if (end_frame >= 0 && end_frame != animations[i].end_frame)
				{
					//TODO: restart animation?
					animations[i].end_frame = end_frame;
				}
			}

			float ticksPerSecond = animations[i].ticks_per_second;
			std::string speed_label = std::string("Speed##") + std::string(std::to_string(i));
			if (ImGui::InputFloat(speed_label.c_str(), &ticksPerSecond))
			{
				if (ticksPerSecond >= 0)
					animations[i].ticks_per_second = ticksPerSecond;
			}
			ImGui::Separator();
			ImGui::Separator();
		}

		if (ImGui::BeginPopup("AnimPopup"))
		{
			if (ImGui::MenuItem("Rename"))
			{
			}

			if (ImGui::MenuItem("Delete"))
			{
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Add Animation"))
		{
		}
	}
}

bool ComponentAnimation::StartAnimation()
{
	if (linked == false)
	{
		LOG("[ERROR] The animation of %s is not linked and is trying to be played.");
	}

	if (current_animation != nullptr)
		started = true;
	return started;
}

void ComponentAnimation::Update()
{
	

}