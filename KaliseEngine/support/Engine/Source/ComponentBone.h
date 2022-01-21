#pragma once

#include "Component.h"
#include "Bone.h"
#include "MathGeoLib/src/MathGeoLib.h"
#include <string>


class ResourceFileBone;

class ComponentBone : public Component
{
public:
	ComponentBone(GameObject* game_object);
	~ComponentBone();

	void OnInspector(bool debug);

	void Save(JsonParsing& file, JSON_Array* array) const;
	void Load(JsonParsing& conf);

	void SetResource(Bone* rBone);
	Bone* GetResource() const;

	//const char* GetResourcePath() const;

	void Update();

	//float4x4 GetSystemTransform();
	//ComponentBone* GetRoot();
private:

private:
	Bone* rBone = nullptr;
};