#include "Gameobject.h"
#include "ComponentCamera.h"
#include "glew.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent) {
	
	frustrum.SetPos(float3(0.0f, 0.0f, -5.0f));
	frustrum.SetUp(float3(0.0f, 1.0f, 0.0f));
	frustrum.SetFront(float3(0.0f, 0.0f, 1.0f));

	//This function calculates the verticalFOV using the given horizontal FOV and aspect ratio. Also sets type to PerspectiveFrustum.
	frustrum.SetHorizontalFovAndAspectRatio(horizontalFOV * DEGTORAD, aspectRatio);

	frustrum.SetViewPlaneDistances(0.3f, 1000.0f);
}

ComponentCamera::~ComponentCamera() {}

bool ComponentCamera::Update(float dt) {

	frustrum.SetPos(owner->GetComponent<ComponentTransform>()->GetPos());
	frustrum.SetUp(owner->GetComponent<ComponentTransform>()->GetGlobalTransform().WorldY());
	frustrum.SetFront(owner->GetComponent<ComponentTransform>()->GetGlobalTransform().WorldZ());

	DrawFrustrum();

	return true;
}

void ComponentCamera::DrawFrustrum()
{
	float3 cornerPoints[8];
	frustrum.GetCornerPoints(cornerPoints);

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