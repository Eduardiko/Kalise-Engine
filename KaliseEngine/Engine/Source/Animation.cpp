#include "Animation.h"
#include "Application.h"
#include "FileSystem.h"
#include "AnimationImporter.h"

bool Channel::HasPosKey() const
{
	return ((positionKeys.size() == 1 && positionKeys.begin()->first == -1) == false);
}

std::map<double, float3>::iterator Channel::GetPrevPosKey(double currentKey)
{

	std::map<double, float3>::iterator ret = positionKeys.lower_bound(currentKey);
	if (ret != positionKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::iterator Channel::GetNextPosKey(double currentKey)
{
	return positionKeys.upper_bound(currentKey);
}

bool Channel::HasRotKey() const
{
	return ((rotationKeys.size() == 1 && rotationKeys.begin()->first == -1) == false);
}

std::map<double, Quat>::iterator Channel::GetPrevRotKey(double currentKey)
{
	std::map<double, Quat>::iterator ret = rotationKeys.lower_bound(currentKey);
	if (ret != rotationKeys.begin())
		ret--;
	return ret;
}

std::map<double, Quat>::iterator Channel::GetNextRotKey(double currentKey)
{
	return rotationKeys.upper_bound(currentKey);
}

bool Channel::HasScaleKey() const
{
	return ((scaleKeys.size() == 1 && scaleKeys.begin()->first == -1) == false);
}

std::map<double, float3>::iterator Channel::GetPrevScaleKey(double currentKey)
{
	//Looping through keys
	std::map<double, float3>::iterator ret = scaleKeys.lower_bound(currentKey);
	if (ret != scaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::iterator Channel::GetNextScaleKey(double currentKey)
{
	return scaleKeys.upper_bound(currentKey);
}

ResourceFileAnimation::ResourceFileAnimation(uint uid, std::string& assets, std::string& library) : Resource(uid, ResourceType::ANIM, assets, library)
{
	std::string metaPath = ANIM_FOLDER + std::string("anim_:") + std::to_string(uid) + ".meta";
	//AnimationImporter::CreateMetaAnim(metaPath, assets, uid);
	name = assets;
	app->fs->GetFilenameWithoutExtension(name);
	name = name.substr(name.find_first_of("__") + 2, name.length());
}

ResourceFileAnimation::ResourceFileAnimation(const std::string& file_path, unsigned int uuid) : Resource(ResourceType::ANIM, file_path, uuid)
{
	std::string metaPath = ANIM_FOLDER + std::string("anim_:") + std::to_string(uid) + ".meta";
	//AnimationImporter::CreateMetaAnim(metaPath, file_path, uid);
	name = file_path;
	app->fs->GetFilenameWithoutExtension(name);
	name = name.substr(name.find_first_of("__") + 2, name.length());
}

ResourceFileAnimation::~ResourceFileAnimation()
{

}

void ResourceFileAnimation::LoadInMemory()
{
	AnimationImporter::LoadAnimation(path.c_str(), this);
}

void ResourceFileAnimation::UnloadInMemory()
{

}