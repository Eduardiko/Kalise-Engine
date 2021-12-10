#include "Gameobject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "glew.h"
#include "imgui.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent) {

}

ComponentCamera::~ComponentCamera() {}

bool ComponentCamera::Update(float dt) {

	position = owner->GetComponent<ComponentTransform>()->GetPosition();
	up = owner->GetComponent<ComponentTransform>()->GetTransform().WorldY();
	front = owner->GetComponent<ComponentTransform>()->GetTransform().WorldZ();

	RecalculateProjection();

	DrawFrustrum();

	return true;
}

void ComponentCamera::OnGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		float3 newPosition = position;

		float newNplDist = nearPlaneDistance;
		if (ImGui::DragFloat("Near Plane Distance", &newNplDist, 0, 0.0f, farPlaneDistance - 1))
		{

			nearPlaneDistance = newNplDist;
		}

		float newFOV = FOV;
		if (ImGui::DragFloat("FOV", &newFOV, 0.0f, 0.0f, 200.0f))
		{
			FOV = newFOV;
		}

		float newFplDist = farPlaneDistance;
		float min = nearPlaneDistance;
		if (ImGui::DragFloat("Far Plane Distance", &newFplDist, 0.0f, min + 1, nearPlaneDistance + 100.0f))
		{

			farPlaneDistance = newFplDist;
		}

		ImGui::Checkbox("Apply Frustum Culling", &applyFrustum);
	}
}

void ComponentCamera::OnSave(JSONWriter& writer)
{
	uint64_t uid64 = uid;
	std::string uidstr = std::to_string(uid64);
	const char* uidchr = uidstr.c_str();

	writer.String(uidchr);
	writer.StartObject();

	SAVE_JSON_FLOAT(position.x);
	SAVE_JSON_FLOAT(position.y);
	SAVE_JSON_FLOAT(position.z);

	SAVE_JSON_FLOAT(up.x);
	SAVE_JSON_FLOAT(up.y);
	SAVE_JSON_FLOAT(up.z);

	SAVE_JSON_FLOAT(front.x);
	SAVE_JSON_FLOAT(front.y);
	SAVE_JSON_FLOAT(front.z);

	SAVE_JSON_FLOAT(nearPlaneDistance);
	SAVE_JSON_FLOAT(farPlaneDistance);

	SAVE_JSON_FLOAT(FOV);

	SAVE_JSON_BOOL(applyFrustum);

	writer.EndObject();
}

void ComponentCamera::OnLoad(const JSONReader& reader)
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

		LOAD_JSON_FLOAT(up.x);
		LOAD_JSON_FLOAT(up.y);
		LOAD_JSON_FLOAT(up.z);

		LOAD_JSON_FLOAT(front.x);
		LOAD_JSON_FLOAT(front.y);
		LOAD_JSON_FLOAT(front.z);

		LOAD_JSON_FLOAT(nearPlaneDistance);
		LOAD_JSON_FLOAT(farPlaneDistance);

		LOAD_JSON_FLOAT(FOV);

		LOAD_JSON_BOOL(applyFrustum);

	}

	RecalculateProjection();
}

void ComponentCamera::RecalculateProjection()
{
	frustum.pos = position;
	frustum.front = front.Normalized();
	frustum.up = up.Normalized();
	float3::Orthonormalize(frustum.front, frustum.up);


	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.verticalFov = (FOV * 3.141592 / 2) / 180.f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);

	frustum.nearPlaneDistance = nearPlaneDistance;
	frustum.farPlaneDistance = farPlaneDistance;
}

void ComponentCamera::DrawFrustrum()
{
	float3 cornerPoints[8];
	frustum.GetCornerPoints(cornerPoints);

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(3.5f);
	glBegin(GL_LINES);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}


// tests if a BBox is within the frustum
bool ComponentCamera::ContainsBBox(const AABB& refBox) const
{
	if (!applyFrustum) return true;

	float3 vCorner[8];
	int totalIn = 0;

	//Get BBox
	refBox.GetCornerPoints(vCorner);

	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for (int p = 0; p < 6; ++p) {
		int cornersOutside = 8;
		int iPtIn = 1;

		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (frustum.GetPlane(p).IsOnPositiveSide(vCorner[i]))
			{
				iPtIn = 0;
				--cornersOutside;
			}
		}
		// were all the points outside of plane p?
		if (cornersOutside == 0) return false;
		totalIn += iPtIn;
	}
	if (totalIn == 6) return true;

	return true;
}