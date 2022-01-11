#ifndef __COMPONENT_ANIMATION_H__
#define __COMPONENT_ANIMATION_H__

#include "Component.h"
#include <string>
#include "Globals.h"
#include "MathGeoLib\include\MathGeoLib.h"
#include <map>

class GameObject;
struct Channel;
class ComponentMesh;
class ComponentBone;

class ResourceFileAnimation;

struct Animation
{
	std::string name;

	uint start_frame;
	uint end_frame;

	float ticks_per_second;

	bool loopable = false;
	float time = 0.0f;

	uint index;

	bool Advance(float dt);
	void SetFrameRatio(float ratio);
	float GetDuration();
};

class ComponentAnimation : public Component
{
	//Careful, this could be dangerous, duplicating pointers
	//TODO: try some other way
	struct Link
	{
		Link(GameObject* gameObject, Channel* channel) : gameObject(gameObject), channel(channel) {};
		GameObject* gameObject;
		Channel* channel;
	};

public:
	ComponentAnimation(GameObject* game_object);
	~ComponentAnimation();

	//Base component behaviour ------------------
	void OnGui() override;

	
	bool StartAnimation();
	void Update();

public:
	std::vector<Animation> animations;
	Animation* current_animation = nullptr;
	//Animation out of blend
	Animation* blend_animation = nullptr;

	bool playing = false;
	bool linked = false;

	bool game_started = false;

private:
	ResourceFileAnimation* rAnimation;

	bool started = false;

	float blend_time = 0.0f;
	float blend_time_duration = 0.0f;

	std::vector<Link> links;

	int renaming_animation = -1;
	int popup_animation = -1;
};
#endif // !__COMPONENT_LIGHT_H__