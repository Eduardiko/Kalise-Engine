#pragma once
#include "Module.h"

#include <vector>
#include <string>

#include "Math/float3.h"
#include "Math/float2.h"
#include "Assimp/include/mesh.h"

class FileMesh {

public:
	unsigned int verticesSizeBytes = 0;
	unsigned int normalsSizeBytes = 0;
	unsigned int indiceSizeBytes = 0;
	unsigned int textCoordSizeBytes = 0;

	float* vertices_ = nullptr; // 1 vertex = sizeof(float) * 3
	float* normals_ = nullptr; // 1 normal = sizeof(float) * 3
	float* textCoords_ = nullptr; // 1 textCoord = sizeof(float) * 2
	unsigned int* indices_ = nullptr; // 1 index = sizeof(unsigned int) * 1

	std::vector<float3> vecVertices;
	std::vector<float3> vecNormals;
	std::vector<unsigned int> vecIndices;
	std::vector<float2> vecTextCoords;


	std::string name = std::string("");
	std::string texturePath = std::string("");
};

class ModuleResourceManager : public Module {

public:
	ModuleResourceManager(Application* app, bool start_enabled = true);
	~ModuleResourceManager();

	//Model
	bool CreateModelFile(const aiMesh* m, const char* path, std::string name);
	bool saveModelFile(FileMesh* file, const char* path, std::string name);
	FileMesh* LoadFileMesh(std::string name);

	void ArrayToVectorConversion(FileMesh* mymodel);

	std::vector<float3> FloatArray2VecFloat3(float* array, unsigned int n);
	std::vector<float2> FloatArray2VecFloat2(float* array, unsigned int n);
	std::vector<unsigned int> UnsignedIntArray2VecFloat3(unsigned int* array, unsigned int n);
	//Load Texture

	//Data Containers
	std::vector<FileMesh*> models;

};

