#pragma once

#include <string>

typedef unsigned int uint;

enum class ResourceType
{
	NONE = 0,
	TEXTURE,
	MESH,
	MODEL,
	SCENE,
	BONE,
	ANIM
};

class Resource
{
public:
	Resource(uint id, ResourceType t, std::string& assets, std::string& library) : uid(id), type(t), assetsPath(assets), libraryPath(library){}
	Resource(ResourceType type, const std::string& file_path, unsigned int uuid) : type(type), uid(uuid){};
	~Resource() {}

	virtual void Load() {}
	virtual void UnLoad() {}

	virtual void DrawOnEditor() {}

	//const char* GetFile(return libraryPath.data())const;
	inline const ResourceType& GetType() const { return type; }
	inline const uint& GetUID() const { return uid; }
	inline const std::string& GetAssetsPath() const { return assetsPath; }
	inline const std::string& GetLibraryPath() const { return libraryPath; }
	inline const std::string& GetName() const { return name; }
	inline const char* GetFile()const { return assetsPath.data(); };

protected:
	uint uid;
	ResourceType type;

	std::string assetsPath;
	std::string libraryPath;
	std::string name;
};