#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	right = float3(1.0f, 0.0f, 0.0f);
	up = float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 5.0f, -15.0f);
	reference = float3(0.0f, 0.0f, 0.0f);
	
	CalculateViewMatrix();

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");

	LookAt(float3::zero);

	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{

	float3 newPos(0,0,0);
	float speed = cameraSpeed * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 4.f;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
			newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
			newPos.y -= speed;
	}

	//Focus
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) 
	{
		if(App->editor->gameobjectSelected != nullptr)
		{			
			if (ComponentMesh* mesh = App->editor->gameobjectSelected->GetComponent<ComponentMesh>())
			{
				const float3 meshCenter = mesh->GetCenterPointInWorldCoords();
				LookAt(meshCenter);
				const float meshRadius = mesh->GetSphereRadius();
				const float currentDistance = meshCenter.Distance(position);
				const float desiredDistance = (meshRadius * 2) / atan(cameraFrustum.horizontalFov);
				position = position + front * (currentDistance - desiredDistance);
			}
			else
			{
				LookAt(App->editor->gameobjectSelected->transform->GetPosition());
			}
		}
	}

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			newPos += front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			newPos -= front * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			newPos += right * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			newPos -= right * speed;
	}

	if (App->input->GetMouseZ() > 0) 
		newPos += front * speed * 2;
	if (App->input->GetMouseZ() < 0) 
		newPos -= front * speed * 2;

	position += newPos;

	// Mouse motion ----------------

	bool hasRotated = false;

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
			if (App->editor->gameobjectSelected != nullptr)
			{
				const float newDeltaX = (float)dx * cameraSensitivity;
				const float newDeltaY = (float)dy * cameraSensitivity;

				reference = App->editor->gameobjectSelected->transform->GetPosition();
				Quat orbitMat = Quat::RotateY(newDeltaX * .1f);								
				
				if (abs(up.y) < 0.3f) // Avoid gimball lock on up & down apex
				{
					if (position.y > reference.y && newDeltaY < 0.f)
						orbitMat = orbitMat * math::Quat::RotateAxisAngle(right, newDeltaY * .1f);
					if (position.y < reference.y && newDeltaY > 0.f)
						orbitMat = orbitMat * math::Quat::RotateAxisAngle(right, newDeltaY * .1f);
				}
				else
				{
					orbitMat = orbitMat * math::Quat::RotateAxisAngle(right, newDeltaY * .1f);
				}
				
				position = orbitMat * (position - reference) + reference;

				CalculateViewMatrix();
				LookAt(reference);
			}
		}
		else
		{

			if (dx != 0)
			{
				const float newDeltaX = (float)dx * cameraSensitivity;
				float deltaX = newDeltaX + 0.95f * (lastDeltaX - newDeltaX); //lerp for smooth rotation acceleration to avoid jittering
				lastDeltaX = deltaX;
				Quat rotateY = Quat::RotateY(up.y >= 0.f ? deltaX * .1f : -deltaX * .1f);
				up = rotateY * up;
				front = rotateY * front;
				CalculateViewMatrix();
				hasRotated = true;
			}

			if (dy != 0)
			{
				const float newDeltaY = (float)dy * cameraSensitivity;
				float deltaY = newDeltaY + 0.95f * (lastDeltaY - newDeltaY); //lerp for smooth rotation acceleration to avoid jittering
				lastDeltaY = deltaY;
				Quat rotateX = Quat::RotateAxisAngle(right, -deltaY * .1f);
				up = rotateX * up;
				front = rotateX * front;
				CalculateViewMatrix();
				hasRotated = true;
			}
		}
	}

	!hasRotated ? lastDeltaX = lastDeltaY = 0.f : 0.f;

	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& point)
{		
	reference = point;

	front = (reference - position).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}



// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	if (projectionIsDirty)
		RecalculateProjection();

	cameraFrustum.pos = position;
	cameraFrustum.front = front.Normalized();
	cameraFrustum.up = up.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	right = up.Cross(front);
	viewMatrix = cameraFrustum.ViewMatrix();
}

void ModuleCamera3D::RecalculateProjection()
{
	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
}

void ModuleCamera3D::OnClick(SDL_Event event)
{
	if (event.button.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT) return;
	MousePick(float2(event.button.x, event.button.y));

}

GameObject* ModuleCamera3D::MousePick(float2 screenPoint)
{
	float2 screenPos = screenPoint;
	screenPos.x = 2.0f * screenPos.x / (float)App->window->width - 1.0f;
	screenPos.y = 1.0f - 2.0f * screenPos.y / (float)App->window->height;

	Ray newRay = cameraFrustum.UnProjectFromNearPlane(screenPos.x, screenPos.y);

	std::map<float, GameObject*> hitGameObjects;

	for (size_t i = 0; i < App->scene->gameObjectList.size(); i++)
	{
		ComponentMesh* m = App->scene->gameObjectList[i]->GetComponent<Mesh>();
		if (m != nullptr)
		{
			bool hit = newRay.Intersects(m->GetGlobalAABB());

			if (hit)
			{
				hitGameObjects[i] = App->scene->gameObjectList[i];
			}
		}

	}

	std::map<float, GameObject*>::iterator j = hitGameObjects.begin();
	for (j; j != hitGameObjects.end(); j++)
	{
		GameObject* gameObject = j->second;

		Ray rayLocal = newRay;
		rayLocal.Transform(gameObject->GetComponent<Transform>()->GetTransform().Inverted());

		ComponentMesh* mesh = gameObject->GetComponent<Mesh>();

		if (mesh != nullptr)
		{
			for (size_t i = 0; i < mesh->numIndices; i += 3)
			{
				float3 v1;
				v1.x = mesh->vertices[mesh->indices[i]].x;
				v1.y = mesh->vertices[mesh->indices[i]].y;
				v1.z = mesh->vertices[mesh->indices[i]].z;

				float3 v2;
				v2.x = mesh->vertices[mesh->indices[i + 1]].x;
				v2.y = mesh->vertices[mesh->indices[i + 1]].y;
				v2.z = mesh->vertices[mesh->indices[i + 1]].z;

				float3 v3;
				v3.x = mesh->vertices[mesh->indices[i + 2]].x;
				v3.y = mesh->vertices[mesh->indices[i + 2]].y;
				v3.z = mesh->vertices[mesh->indices[i + 2]].z;

				const Triangle triangle(v1, v2, v3);

				float distance;
				float3 intersectionPoint;
				if (rayLocal.Intersects(triangle, &distance, &intersectionPoint))
				{
					App->editor->gameobjectSelected ? App->editor->gameobjectSelected->isSelected = !App->editor->gameobjectSelected->isSelected : 0;
					App->editor->gameobjectSelected = gameObject;
					App->editor->gameobjectSelected->isSelected = !App->editor->gameobjectSelected->isSelected;
					return gameObject;
				}
			}
		}
	
	}

	return nullptr;
}

void ModuleCamera3D::OnGui()
{
	if (ImGui::CollapsingHeader("Editor Camera"))
	{
		if (ImGui::DragFloat("Vertical fov", &verticalFOV))
		{
			projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Near plane distance", &nearPlaneDistance))
		{
			projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Far plane distance", &farPlaneDistance))
		{
			projectionIsDirty = true;
		}
	}
}

void ModuleCamera3D::OnSave(JSONWriter& writer) const
{
	writer.String("camera");	
	writer.StartObject();
	SAVE_JSON_FLOAT(verticalFOV)
	SAVE_JSON_FLOAT(nearPlaneDistance)
	SAVE_JSON_FLOAT(farPlaneDistance)
	SAVE_JSON_FLOAT(cameraSpeed)
	SAVE_JSON_FLOAT(cameraSensitivity)
	writer.EndObject();
}

void ModuleCamera3D::OnLoad(const JSONReader& reader)
{
	if (reader.HasMember("camera"))
	{
		const auto& config = reader["camera"];
		LOAD_JSON_FLOAT(verticalFOV);
		LOAD_JSON_FLOAT(nearPlaneDistance);
		LOAD_JSON_FLOAT(farPlaneDistance);
		LOAD_JSON_FLOAT(cameraSpeed);
		LOAD_JSON_FLOAT(cameraSensitivity);
	}
	RecalculateProjection();
}