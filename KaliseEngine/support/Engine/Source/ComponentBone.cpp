#include "ComponentBone.h"
#include <string>
#include "imgui\imgui.h"
#include "Bone.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ResourceManager.h"
#include "JsonParsing.h"
#include "GameObject.h"
#include "TransformComponent.h"

#include <string>


ComponentBone::ComponentBone(GameObject* game_object) 
{


}

ComponentBone::~ComponentBone()
{

}

void ComponentBone::OnInspector(bool debug)
{
	std::string str = (std::string("Bone") + std::string("##"));
	if (ImGui::CollapsingHeader(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Num Weights: %i", rBone->numWeights);
		ImGui::Text("Offset Matrix:");
		ImGui::Text("%0.2f %0.2f %0.2f %0.2f", rBone->offset.v[0][0], rBone->offset.v[0][1], rBone->offset.v[0][2], rBone->offset.v[0][3]);
		ImGui::Text("%0.2f %0.2f %0.2f %0.2f", rBone->offset.v[1][0], rBone->offset.v[1][1], rBone->offset.v[1][2], rBone->offset.v[1][3]);
		ImGui::Text("%0.2f %0.2f %0.2f %0.2f", rBone->offset.v[2][0], rBone->offset.v[2][1], rBone->offset.v[2][2], rBone->offset.v[2][3]);
		ImGui::Text("%0.2f %0.2f %0.2f %0.2f", rBone->offset.v[3][0], rBone->offset.v[3][1], rBone->offset.v[3][2], rBone->offset.v[3][3]);
	}
}

void ComponentBone::Save(JsonParsing& node, JSON_Array* array)const
{
	JsonParsing file = JsonParsing();

	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()),"type", (int)type);
	//data.AppendUInt("UUID", uuid);
	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "active", active);
	//file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()),"path", rBone->GetLibraryPath());


	//file.AppendArrayValue(file);
	node.SetValueToArray(array, file.GetRootValue());

}

void ComponentBone::Load(JsonParsing& conf)
{
	//uuid = conf.GetUInt("UUID");
	active = conf.GetJsonNumber("active");
	//String* path = conf.GetJsonString("path");


	//rBone = (Bone*)ResourceManager::GetInstance()->LoadResource(path);
}

void ComponentBone::SetResource(Bone* rBone)
{
	this->rBone = rBone;
}

Bone* ComponentBone::GetResource() const
{
	return rBone;
}

//const char* ComponentBone::GetResourcePath() const
//{
//	return rBone == nullptr ? nullptr : rBone->GetFile();
//}

void ComponentBone::Update()
{
	/*if (app->StartInGame() == false)
	{
		for (std::vector<GameObject*>::const_iterator it = game_object->GetChilds()->begin(); it != game_object->GetChilds()->end(); it++)
		{
			float3 pos1 = game_object->transform->GetGlobalMatrix().TranslatePart();
			float3 pos2 = (*it)->transform->GetGlobalMatrix().TranslatePart();
			App->renderer3D->DrawLine(pos1, pos2, float4(1, 0, 1, 1));
		}
	}*/
}

//float4x4 ComponentBone::GetSystemTransform()
//{
//	/*float4x4 transform = game_object->transform->GetGlobalMatrix();
//	return GetRoot()->game_object->GetParent()->GetParent()->transform->GetGlobalMatrix().Inverted() * transform;*/
//
//}
//
//ComponentBone* ComponentBone::GetRoot()
//{
//	/*ComponentBone* parentBone = (ComponentBone*)game_object->GetParent()->GetComponent(C_BONE);
//	return parentBone == nullptr ? this : parentBone->GetRoot();*/
//
//}