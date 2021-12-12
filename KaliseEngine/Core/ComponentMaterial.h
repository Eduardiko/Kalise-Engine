#pragma once

#include "Component.h"

struct TextureObject;

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent);

	void SetTexture(const TextureObject& texture);
	void OnGui() override;
	void OnSave(JSONWriter& writer) override;
	void OnLoad(const JSONReader& reader) override;

	inline uint GetTextureId() const { return textureId; }


private:

	std::string textureName;
	uint textureId = 0, width = 0, height = 0;

	UUID_ uid;
};