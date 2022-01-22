#pragma once

#include "Component.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "MathGeoLib/src/MathGeoLib.h"
#include "ComponentBone.h"

#include <vector>

typedef unsigned int GLuint;
typedef unsigned char GLubyte;

class TransformComponent;
class MaterialComponent;
class Mesh;
class Resource;
class Bone;

struct Bone_Vertex
{
	//All bones that influence the vertex
	//stored in "bones_reference" vector
	std::vector<uint> bone_index;
	std::vector<float> weights;

	void AddBone(uint index, float weight) { bone_index.push_back(index);	weights.push_back(weight); }
};


struct Bone_Reference
{
	Bone_Reference(ComponentBone* bone, float4x4 offset) { this->bone = bone; this->offset = offset; }
	ComponentBone* bone;
	float4x4 offset = float4x4::identity;
};


class MeshComponent : public Component
{
public:
	MeshComponent(GameObject* own, TransformComponent* trans);
	MeshComponent(MeshComponent* meshComponent, TransformComponent* trans);
	~MeshComponent();

	void Draw() override;
	void DrawOutline() override;
	void OnEditor() override;

	bool OnLoad(JsonParsing& node) override;
	bool OnSave(JsonParsing& node, JSON_Array* array) override;

	void SetMesh(std::shared_ptr<Resource> m);

	inline void SetTransform(TransformComponent* trans) { transform = trans; }
	inline void SetMaterial(MaterialComponent* mat) { material = mat; }

	inline AABB GetLocalAABB() { return localBoundingBox; }
	const std::shared_ptr<Mesh> GetMesh() const { return mesh; }

	bool HasBones();
	void AddBone(ComponentBone* bone);
	void DeformAnimMesh();

	bool animated = false;
	unsigned int weight_id = 0;
	unsigned int bone_id = 0;
	std::vector<math::float4x4> bones_trans;

	std::vector<Bone_Reference> bones_reference;
	std::vector<Bone_Vertex> bones_vertex;
	GameObject* game_object;

private:
	TransformComponent* transform;
	MaterialComponent* material;

	float normalLength;
	float3 colorNormal;
	
	bool faceNormals;
	bool verticesNormals;

	std::shared_ptr<Mesh> mesh;

	AABB localBoundingBox;

	bool showMeshMenu;
	
};

