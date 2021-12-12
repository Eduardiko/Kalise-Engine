#include "Application.h"
#include "ModuleTextures.h"
#include "imgui.h"
#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent) {}

void ComponentMaterial::SetTexture(const TextureObject& texture)
{
	textureName = texture.name;
	textureId = texture.id;
	width = texture.width;
	height = texture.height;
}

void ComponentMaterial::OnGui()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		if (textureId != 0)
		{
			ImGui::Text("Name: %s", textureName.c_str());
			ImGui::Image((ImTextureID)textureId, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::Text("Size: %d x %d", width, height);
		}
		else
		{
			ImGui::Text("Name: none");
			ImGui::Text("Size: 0 x 0");
		}
	}
}

void ComponentMaterial::OnSave(JSONWriter& writer)
{
	uint64_t uid64 = uid;
	std::string uidstr = std::to_string(uid64);
	const char* uidchr = uidstr.c_str();

	writer.String(uidchr);
	writer.StartObject();
	SAVE_JSON_FLOAT(textureId);
	SAVE_JSON_FLOAT(width);
	SAVE_JSON_FLOAT(height);
	writer.EndObject();

}

void ComponentMaterial::OnLoad(const JSONReader& reader)
{
	uint64_t uid64 = uid;
	std::string uidstr = std::to_string(uid64);
	const char* uidchr = uidstr.c_str();

	if (reader.HasMember(uidchr))
	{
		const auto& config = reader[uidchr];
		LOAD_JSON_FLOAT(textureId);
		LOAD_JSON_FLOAT(width);
		LOAD_JSON_FLOAT(height);
		LOAD_JSON_STRING(textureName);
	}
}
