#pragma once

// -- Tools
#include "Module.h"
#include "Globals.h"
#include "ModuleImport.h"

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);

	bool Start() override;
	update_status Update(float dt) override;
	
	bool CleanUp() override;
	bool DeleteAllGameObjects();
	bool DeleteSelectedGameObject(GameObject* selectedGameObject);

	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateGameObjectByName(const std::string name, GameObject* parent = nullptr);	
	GameObject* CreateEmptyGameObject(GameObject* parent = nullptr);
	GameObject* CreateChildrenGameObject(GameObject* parent = nullptr);
	
	void DuplicateGameObject(GameObject* parent = nullptr);
	void CreateRoot();

	void OnSave(JSONWriter& writer) const override;
	void OnLoad(const JSONReader& reader) override;

private:
	int emptyCounter = 0;
public:
	GameObject* root;
	std::vector<GameObject*> gameObjectList;
	std::vector<GameObject*> rootList;

	ImVec2 winPos;
	ImVec2 winSize;
	ImVec2 mouseWinPos;
};
