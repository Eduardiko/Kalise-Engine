#include "ModuleResourceManager.h"
#include <fstream>

ModuleResourceManager::ModuleResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleResourceManager::~ModuleResourceManager()
{
}

bool ModuleResourceManager::FBX2Kalise(const aiMesh* m, const char* path, std::string name)
{
	FileMesh* file = new FileMesh();

	file->verticesSizeBytes = m->mNumVertices * sizeof(float) * 3;
	file->vertices = (float*)malloc(file->verticesSizeBytes);
	memcpy(file->vertices, m->mVertices, file->verticesSizeBytes);

	file->normalsSizeBytes = m->mNumVertices * sizeof(float) * 3;
	file->normals = (float*)malloc(file->normalsSizeBytes);
	memcpy(file->normals, m->mNormals, file->normalsSizeBytes);

	file->textCoordSizeBytes = m->mNumVertices * sizeof(float) * 2;
	file->textCoords = (float*)malloc(file->textCoordSizeBytes);
	for (int i = 0; i < m->mNumVertices; i++)
	{
		*(file->textCoords + i * 2) = m->mTextureCoords[0][i].x;
		*(file->textCoords + i * 2 + 1) = m->mTextureCoords[0][i].y;
	}

	file->indiceSizeBytes = m->mNumFaces * sizeof(unsigned int) * 3;
	file->indices = (unsigned*)malloc(file->indiceSizeBytes);
	for (int i = 0; i < m->mNumFaces; i++)
	{
		aiFace* f = m->mFaces + i;
		*(file->indices + 0 + i * 3) = f->mIndices[0];
		*(file->indices + 1 + i * 3) = f->mIndices[1];
		*(file->indices + 2 + i * 3) = f->mIndices[2];
	}

	file->name = name;
	fileList.push_back(file);

	return Kalise2Binary(file, path, name);
}

//LOAD IN BINARY THE FILE
bool ModuleResourceManager::Kalise2Binary(FileMesh* file, const char* path, std::string name)
{
	std::string fileName = "Assets/Library/" + name + ".kali";
	std::ofstream myfile;
	myfile.open(fileName, std::ios::binary);
	if (myfile.is_open())
	{
		myfile.write((char*)file, 4 * sizeof(unsigned int));

		myfile.write((char*)file->vertices, file->verticesSizeBytes);
		myfile.write((char*)file->normals, file->normalsSizeBytes);
		myfile.write((char*)file->textCoords, file->textCoordSizeBytes);
		myfile.write((char*)file->indices, file->indiceSizeBytes);

		myfile.close();
		return true;
	}
	else
	{
		LOG("Error creating FileMesh at '%s'", fileName);
		return false;
	}
}

FileMesh* ModuleResourceManager::Binary2Mesh(std::string name)
{
	std::string fullName = "Assets/Library/" + name + ".kali";

	std::ifstream myfile;
	myfile.open(fullName, std::ios::binary);
	if (myfile.is_open())
	{
		FileMesh* mymodel = new FileMesh();
		myfile.read((char*)mymodel, 4 * sizeof(unsigned int));

		mymodel->vertices = (float*)malloc(mymodel->verticesSizeBytes);
		myfile.read((char*)mymodel->vertices, mymodel->verticesSizeBytes);

		mymodel->normals = (float*)malloc(mymodel->normalsSizeBytes);
		myfile.read((char*)mymodel->normals, mymodel->normalsSizeBytes);

		mymodel->textCoords = (float*)malloc(mymodel->textCoordSizeBytes);
		myfile.read((char*)mymodel->textCoords, mymodel->textCoordSizeBytes);

		mymodel->indices = (unsigned int*)malloc(mymodel->indiceSizeBytes);
		myfile.read((char*)mymodel->indices, mymodel->indiceSizeBytes);

		myfile.close();

		Array2Vector(mymodel);

		return mymodel;
	}
	else
	{
		LOG("Error loading MontuFileMesh from '%s'", fullName);
		return nullptr;
	}
}

void ModuleResourceManager::Array2Vector(FileMesh* mymodel)
{
	mymodel->vecVertices = FloatArray2VecFloat3(mymodel->vertices, mymodel->verticesSizeBytes / (sizeof(float)));
	mymodel->vecNormals = FloatArray2VecFloat3(mymodel->normals, mymodel->normalsSizeBytes / (sizeof(float)));
	mymodel->vecTextCoords = FloatArray2VecFloat2(mymodel->textCoords, mymodel->textCoordSizeBytes / (sizeof(float)));
	mymodel->vecIndices = UnsignedIntArray2VecFloat3(mymodel->indices, mymodel->indiceSizeBytes / (sizeof(unsigned int)));
}

std::vector<float3> ModuleResourceManager::FloatArray2VecFloat3(float* array, unsigned int n)
{
	std::vector<float3> dest;
	for (unsigned int i = 0; i < n; i += 3)
	{
		dest.push_back({ array[i], array[i + 1], array[i + 2] });
	}
	return dest;
}

std::vector<float2> ModuleResourceManager::FloatArray2VecFloat2(float* array, unsigned int n)
{
	std::vector<float2> dest;
	for (unsigned int i = 0; i < n; i += 2)
	{
		dest.push_back({ array[i], array[i + 1] });
	}
	return dest;
}

std::vector<unsigned int> ModuleResourceManager::UnsignedIntArray2VecFloat3(unsigned int* array, unsigned int n)
{
	std::vector<unsigned int> dest;
	for (unsigned int i = 0; i < n; ++i)
	{
		dest.push_back(array[i]);
	}
	return dest;
}


