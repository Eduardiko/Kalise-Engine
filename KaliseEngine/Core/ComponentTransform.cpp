#include "ComponentTransform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Math/TransformOps.h"
#include "glew.h"
#include "imgui.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent) {
	
	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;

	transformMatrix.SetIdentity();
	transformMatrixLocal.SetIdentity();
}


bool ComponentTransform::Update(float dt) {
	if (isDirty)
	{
		transformMatrixLocal = float4x4::FromTRS(position, rotation, scale);

		right = transformMatrixLocal.Col3(0).Normalized();
		up = transformMatrixLocal.Col3(1).Normalized();
		front = transformMatrixLocal.Col3(2).Normalized();
		RecomputeGlobalMatrix();
		owner->PropagateTransform();
		isDirty = false;
	}
	return true;
}

void ComponentTransform::OnGui()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		float3 newPosition = position;
		if (ImGui::DragFloat3("Location", &newPosition[0]))
		{
			SetPosition(newPosition);
		}
		float3 newRotationEuler;
		newRotationEuler.x = RADTODEG * rotationEuler.x;
		newRotationEuler.y = RADTODEG * rotationEuler.y;
		newRotationEuler.z = RADTODEG * rotationEuler.z;

		if (ImGui::DragFloat3("Rotation", &(newRotationEuler[0])))
		{
			newRotationEuler.x = DEGTORAD * newRotationEuler.x;
			newRotationEuler.y = DEGTORAD * newRotationEuler.y;
			newRotationEuler.z = DEGTORAD * newRotationEuler.z;
			SetRotation(newRotationEuler);
		}
		float3 newScale = scale;
		if (ImGui::DragFloat3("Scale", &(newScale[0])))
		{
			SetScale(newScale);
		}
	}
}

void ComponentTransform::OnSave(JSONWriter& writer)
{
	uint64_t uid64 = uid;
	std::string uidstr = std::to_string(uid64);
	const char* uidchr = uidstr.c_str();

	writer.String(uidchr);
	writer.StartObject();

	SAVE_JSON_FLOAT(position.x);
	SAVE_JSON_FLOAT(position.y);
	SAVE_JSON_FLOAT(position.z);

	SAVE_JSON_FLOAT(rotation.x);
	SAVE_JSON_FLOAT(rotation.y);
	SAVE_JSON_FLOAT(rotation.z);
	SAVE_JSON_FLOAT(rotation.w);

	SAVE_JSON_FLOAT(rotationEuler.x);
	SAVE_JSON_FLOAT(rotationEuler.y);
	SAVE_JSON_FLOAT(rotationEuler.z);

	SAVE_JSON_FLOAT(scale.x);
	SAVE_JSON_FLOAT(scale.y);
	SAVE_JSON_FLOAT(scale.z);

	SAVE_JSON_BOOL(isDirty);

	writer.EndObject();
}

void ComponentTransform::OnLoad(const JSONReader& reader)
{
	uint64_t uid64 = uid;
	std::string uidstr = std::to_string(uid64);
	const char* uidchr = uidstr.c_str();

	if (reader.HasMember(uidchr))
	{
		const auto& config = reader[uidchr];
		LOAD_JSON_FLOAT(position.x);
		LOAD_JSON_FLOAT(position.y);
		LOAD_JSON_FLOAT(position.z);

		LOAD_JSON_FLOAT(rotation.x);
		LOAD_JSON_FLOAT(rotation.y);
		LOAD_JSON_FLOAT(rotation.z);
		LOAD_JSON_FLOAT(rotation.w);

		LOAD_JSON_FLOAT(rotationEuler.x);
		LOAD_JSON_FLOAT(rotationEuler.y);
		LOAD_JSON_FLOAT(rotationEuler.z);

		LOAD_JSON_FLOAT(scale.x);
		LOAD_JSON_FLOAT(scale.y);
		LOAD_JSON_FLOAT(scale.z);

		LOAD_JSON_BOOL(isDirty);
	}

	transformMatrixLocal = float4x4::FromTRS(position, rotation, scale);

	right = transformMatrixLocal.Col3(0).Normalized();
	up = transformMatrixLocal.Col3(1).Normalized();
	front = transformMatrixLocal.Col3(2).Normalized();
	RecomputeGlobalMatrix();
	owner->PropagateTransform();
	isDirty = false;

}

void ComponentTransform::SetPosition(const float3& newPosition)
{
	position = newPosition;
	isDirty = true;
}

void ComponentTransform::SetRotation(const float3& newRotation)
{
	Quat rotationDelta = Quat::FromEulerXYZ(newRotation.x - rotationEuler.x, newRotation.y - rotationEuler.y, newRotation.z - rotationEuler.z);	
	rotation = rotation * rotationDelta;
	rotationEuler = newRotation;
	isDirty = true;
}

void ComponentTransform::SetScale(const float3& newScale)
{
	scale = newScale;
	isDirty = true;
}

float4x4 ComponentTransform::GetTransform()
{
	return transformMatrix;
}

void ComponentTransform::NewAttachment()
{
	if (owner->parent != App->scene->root)
		transformMatrixLocal = owner->parent->transform->transformMatrix.Inverted().Mul(transformMatrix);

	float3x3 rot;
	transformMatrixLocal.Decompose(position, rot, scale);
	rotationEuler = rot.ToEulerXYZ();
}

void ComponentTransform::OnParentMoved()
{
	RecomputeGlobalMatrix();
}

void ComponentTransform::RecomputeGlobalMatrix()
{
	if (owner->parent != nullptr)
	{
		transformMatrix = owner->parent->transform->transformMatrix.Mul(transformMatrixLocal);
	}
	else
	{
		transformMatrix = transformMatrixLocal;
	}
}
