#pragma once

#include "Component.h"
#include "Bone.h"
#include "MathGeoLib/src/MathGeoLib.h"
#include <string>

#include "TransformComponent.h"


class ResourceFileBone;

class ComponentBone : public Component
{
public:
	ComponentBone(GameObject* game_object,TransformComponent* Transform);
	~ComponentBone();

	void OnEditor() override;

	void Save(JsonParsing& file, JSON_Array* array) const;
	void Load(JsonParsing& conf);

	void SetResource(Bone* rBone);
	Bone* GetResource() const;

	//const char* GetResourcePath() const;

	void Update();

	float4x4 GetSystemTransform();
	ComponentBone* GetRoot();

	GameObject* game_object;
	TransformComponent* transform;

private:

private:
	Bone* rBone = nullptr;
};