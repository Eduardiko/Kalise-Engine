#include "Application.h"
#include "Globals.h"
#include "MeshComponent.h"

#include "ModuleScene.h"
#include "CameraComponent.h"
#include "FileSystem.h"
#include "ResourceManager.h"
#include "ComponentBone.h"
#include "ModuleEditor.h"
#include "MeshImporter.h"

#include "Mesh.h"

#include "Imgui/imgui.h"

#include "glew/include/GL/glew.h"

#include "Profiling.h"

MeshComponent::MeshComponent(GameObject* own, TransformComponent* trans) : material(nullptr), transform(trans), faceNormals(false), verticesNormals(false), normalLength(1.0f), colorNormal(150.0f, 0.0f, 255.0f)
{
	type = ComponentType::MESH_RENDERER;
	owner = own;
	mesh = nullptr;
	material = owner->GetComponent<MaterialComponent>();

	game_object = new GameObject();
	this->game_object->CreateComponent(ComponentType::TRANSFORM);

	showMeshMenu = false;
}

MeshComponent::MeshComponent(MeshComponent* meshComponent, TransformComponent* trans) : material(nullptr), showMeshMenu(false)
{
	transform = trans;
	mesh = meshComponent->GetMesh();

	faceNormals = meshComponent->faceNormals;
	verticesNormals = meshComponent->verticesNormals;
	normalLength = meshComponent->normalLength;
	colorNormal = meshComponent->colorNormal;

	localBoundingBox = meshComponent->localBoundingBox;
}

MeshComponent::~MeshComponent()
{
	if (mesh.use_count() - 1 == 1) mesh->UnLoad();
}

void MeshComponent::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	glMultMatrixf(transform->GetGlobalTransform().Transposed().ptr());
	
	if (material != nullptr && material->GetActive()) material->BindTexture();
	
	if (mesh != nullptr) mesh->Draw(verticesNormals, faceNormals, colorNormal, normalLength);

	if (material != nullptr && material->GetActive()) material->UnbindTexture();
	
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MeshComponent::DrawOutline()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glPushMatrix();
	float4x4 testGlobal = transform->GetGlobalTransform();

	testGlobal.scaleX += 0.05f;
	testGlobal.scaleY += 0.05f;
	testGlobal.scaleZ += 0.05f;
	glMultMatrixf(testGlobal.Transposed().ptr());

	if (mesh != nullptr) mesh->Draw(verticesNormals, faceNormals, colorNormal, normalLength);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MeshComponent::OnEditor()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader("Mesh Renderer"))
	{
		Checkbox(this, "Active", active);
		ImGui::Text("Select mesh");
		ImGui::SameLine();
		if (ImGui::Button(mesh ? mesh->GetName().c_str() : "No Mesh"))
		{
			showMeshMenu = true;
		}
		ImGui::Text("Number of vertices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", mesh ? mesh->GetVerticesSize() : 0);
		ImGui::Text("Number of indices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", mesh ? mesh->GetIndicesSize() : 0);
		ImGui::Checkbox("Vertices normals", &verticesNormals);
		ImGui::Checkbox("Face normals", &faceNormals);
		ImGui::DragFloat("Normal Length", &normalLength, 0.200f);
		ImGui::DragFloat3("Normal Color", colorNormal.ptr(), 1.0f, 0.0f, 255.0f);
		ImGui::Text("Reference Count: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", mesh ? mesh.use_count() : 0);
		ImGui::Separator();
	}

	if (showMeshMenu)
	{
		ImGui::Begin("Meshes", &showMeshMenu, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 mouse = ImGui::GetIO().MousePos;
		if (!(mouse.x < winPos.x + size.x && mouse.x > winPos.x && 
			mouse.y < winPos.y + size.y && mouse.y > winPos.y))
		{
			if (ImGui::GetIO().MouseClicked[0]) showMeshMenu = false;
		}

		std::vector<std::string> files;
		app->fs->DiscoverFiles("Library/Meshes/", files);
		for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
		{
			if ((*it).find(".rgmesh") != std::string::npos)
			{
				app->fs->GetFilenameWithoutExtension(*it);
				*it = (*it).substr((*it).find_last_of("_") + 1, (*it).length());
				uint uid = std::stoll(*it);
				std::shared_ptr<Resource> res = ResourceManager::GetInstance()->LoadResource(uid);
				if (ImGui::Selectable(res->GetName().c_str()))
				{
					if (mesh.use_count() - 1 == 1) mesh->UnLoad();
					SetMesh(res);
				}
			}
		}

		ImGui::End();
	}

	ImGui::PopID();
}

bool MeshComponent::OnLoad(JsonParsing& node)
{
	mesh = std::static_pointer_cast<Mesh>(ResourceManager::GetInstance()->LoadResource(std::string(node.GetJsonString("Path"))));

	active = node.GetJsonBool("Active");

	if (mesh)
	{
		localBoundingBox.SetNegativeInfinity();
		localBoundingBox.Enclose(mesh->GetVerticesData(), mesh->GetVerticesSize());

		owner->SetAABB(localBoundingBox);
	}

	return true;
}

bool MeshComponent::OnSave(JsonParsing& node, JSON_Array* array)
{
	JsonParsing file = JsonParsing();

	file.SetNewJsonNumber(file.ValueToObject(file.GetRootValue()), "Type", (int)type);
	file.SetNewJsonString(file.ValueToObject(file.GetRootValue()), "Path", mesh->GetAssetsPath().c_str());
	file.SetNewJsonBool(file.ValueToObject(file.GetRootValue()), "Active", active);

	node.SetValueToArray(array, file.GetRootValue());

	return true;
}

void MeshComponent::SetMesh(std::shared_ptr<Resource> m)
{
	mesh = std::static_pointer_cast<Mesh>(m);

	if (mesh)
	{
		localBoundingBox.SetNegativeInfinity();
		localBoundingBox.Enclose(mesh->GetVerticesData(), mesh->GetVerticesSize());

		owner->SetAABB(localBoundingBox);
	}
}


bool MeshComponent::HasBones()
{
	return bones_reference.size() > 0;
}

void MeshComponent::AddBone(ComponentBone* bone)
{
	for (uint i = 0; i < bones_reference.size(); i++)
		if (bones_reference[i].bone == bone)
			return;

	TransformComponent trans = (TransformComponent*)bone->game_object->GetComponent(ComponentType::TRANSFORM);
	Bone* bonestatic = bone->GetResource();
	bones_reference.push_back(Bone_Reference(bone,bonestatic->offset));

	if (bones_vertex.empty())
	{
		bones_vertex = std::vector<Bone_Vertex>(verticesNormals);
	}

	Bone* rBone = bone->GetResource();
	
	for (uint i = 0; i < rBone->numWeights; i++)
	{
		uint data_b_index = bones_reference.size() - 1;
		float data_b_float = rBone->weights[i];
		bones_vertex[rBone->weightsIndex[i]].AddBone(data_b_index, data_b_float);
	}
}

void MeshComponent::DeformAnimMesh()
{
	//BROFILER_CATEGORY("ComponentMesh::DeformAnimMesh", Profiler::Color::Maroon)

		bones_trans.clear();

	for (uint i = 0; i < bones_reference.size(); i++)
	{
		float4x4 matrix = bones_reference[i].bone->GetSystemTransform();
		TransformComponent trans = (TransformComponent*)game_object->GetComponent(ComponentType::TRANSFORM);
		matrix = trans.GetLocalTransform().Inverted() * matrix;
		
		float4x4 bone_trn_mat = matrix * bones_reference[i].offset;
		bones_trans.push_back(bone_trn_mat.Transposed());
		
	}
}

void MeshComponent::InitAnimBuffers()
{
	if (mesh != nullptr)
	{
		int size = mesh.get()->GetVerticesSize() * 4;
		float* weights = new float[size];
		int* bones_ids = new int[size];

		for (size_t i = 0; i < bones_vertex.size(); ++i)
		{
			int ver_id = i * 4;

			if (bones_vertex[i].weights.size() != bones_vertex[i].bone_index.size())
			{
				LOG("[WARNING] %s has different number of weights and index in the animation", game_object->name); //Just in case
				//App->editor->DisplayWarning(WarningType::W_WARNING, "%s has different number of weights and index in the animation", game_object->name);
				return;
			}

			//Reset all to zero
			for (int w = 0; w < 4; ++w)
			{
				weights[ver_id + w] = 0;
				bones_ids[ver_id + w] = 0;
			}

			for (size_t w = 0; w < bones_vertex[i].weights.size(); ++w)
			{
				weights[ver_id + w] = bones_vertex[i].weights[w];
				bones_ids[ver_id + w] = bones_vertex[i].bone_index[w];
			}
		}
		
		//MeshImporter::LoadAnimBuffers(weights, size, weight_id, bones_ids, size, bone_id);

		delete[] weights;
		delete[] bones_ids;

		animated = true;
	}
	else
	{
		//		LOG("[WARNING] Trying to init animation buffers from '%s' without a mesh", game_object->name);
		//		App->editor->DisplayWarning(WarningType::W_WARNING, "Trying to init animation buffers from '%s' without a mesh", game_object->name);
	}

}
