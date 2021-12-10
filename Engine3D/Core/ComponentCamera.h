#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Component.h"
#include "Globals.h"
#include <string>
#include "MathGeoLib.h"
#include"Geometry/Frustum.h"

class GameObject;

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent);
	~ComponentCamera();

	bool Update(float dt) override;
	
	void RecalculateMatrix();
	void DrawFrustrum();

	bool ContainsBBox(const AABB& refBox) const;

public:
	Frustum frustum;

	float3 right;
	float3 up;
	float3 front;
	float3 position;
	float3 reference;

	float nearPlaneDistance = 0.0f;
	float farPlaneDistance = 20.0f;

	float verticalFOV = 60.0f;
	float aspectRatio = 16 / 9;
};

#endif //!__COMPONENT_CAMERA_H__