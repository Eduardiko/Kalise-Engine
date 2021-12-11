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

	float* vertices = nullptr;
	float* normals = nullptr;
	float* textCoords = nullptr; 
	unsigned int* indices = nullptr; 

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
	bool FBX2Kalise(const aiMesh* m, const char* path, std::string name);
	bool Kalise2Binary(FileMesh* file, const char* path, std::string name);
	FileMesh* Binary2Mesh(std::string name);

	void Array2Vector(FileMesh* mymodel);

	std::vector<float3> FloatArray2VecFloat3(float* array, unsigned int n);
	std::vector<float2> FloatArray2VecFloat2(float* array, unsigned int n);
	std::vector<unsigned int> UnsignedIntArray2VecFloat3(unsigned int* array, unsigned int n);

	std::vector<FileMesh*> fileList;

};

