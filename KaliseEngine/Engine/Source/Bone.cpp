#include "Bone.h"
#include "AnimationImporter.h"

Bone::Bone(uint uid, std::string& assets, std::string& library) : Resource(uid,ResourceType::BONE, assets, library)
{

}

Bone::~Bone()
{

}

void Bone::LoadInMemory()
{
	//AnimationImporter::LoadBone(file_path.c_str(), this);
}

void Bone::UnloadInMemory()
{

}