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
	
	void DrawFrustrum();

public:
	Frustum frustrum;
	float horizontalFOV = 90.0f;
	float aspectRatio = 16 / 9;
};

#endif //!__COMPONENT_CAMERA_H__