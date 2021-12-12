#pragma once

// -- Tools
#include <vector>
#include <string>
#include "GameObject.h"
#include "UUID_.h"

class Component {

public:


	Component(GameObject* parent) : owner(parent)
	{
		if (parent)
			parent->AddComponent(this);
	};
	virtual ~Component() {};

	virtual bool Update(float dt) {
		return true;
	}

	virtual bool Enable() { return active = true; };
	virtual bool Disable() { return active = false; };
	virtual void OnGui() { }
	virtual void OnSave(JSONWriter& writer) { };
	virtual void OnLoad(const JSONReader& reader) { };

public:

	GameObject* owner = nullptr;
	bool active = true;
};