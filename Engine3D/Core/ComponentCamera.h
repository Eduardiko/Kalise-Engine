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
	void OnGui() override;
	void OnSave(JSONWriter& writer) override;
	void OnLoad(const JSONReader& reader) override;

	void RecalculateProjection();
	void DrawFrustrum();

	bool ContainsBBox(const AABB& refBox) const;

public:
	Frustum frustum;

	float3 up;
	float3 front;
	float3 position;

	float nearPlaneDistance = 0.0f;
	float farPlaneDistance = 20.0f;

	float FOV = 60.0f;
	float aspectRatio = 16 / 9;

	bool applyFrustum = true;

	UUID_ uid;
};

#endif //!__COMPONENT_CAMERA_H__