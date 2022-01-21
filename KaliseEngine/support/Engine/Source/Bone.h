#pragma once

#include "Resource.h"
#include "MathGeoLib/src/MathGeoLib.h"
#include <map>
#include "Globals.h"

class Bone : public Resource
{
public:
	Bone(uint uid, std::string& assets, std::string& library);
	~Bone();

private:
	void LoadInMemory();
	void UnloadInMemory();

public:
	uint numWeights = 0;
	uint* weightsIndex = nullptr;
	float* weights = nullptr;
	float4x4 offset = float4x4::identity;

	std::string mesh_path;
};