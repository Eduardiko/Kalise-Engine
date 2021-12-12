
#include "Application.h"
#include "ModuleScene.h"
#include "glew.h"
#include "imgui.h"

#include "ModuleImport.h"
#include "ModuleTextures.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleViewportFrameBuffer.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include <stack>
#include <queue>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	
	CreateRoot();

	App->import->LoadGeometry("Assets/Models/BakerHouse.fbx");
	
	camera = CreateGameObject("Camera");
	camera->CreateComponent<ComponentCamera>();
	float3 camInitPos(0.0f, 0.0f, -12.0f);
	camera->GetComponent<ComponentTransform>()->SetPosition(camInitPos);

	return ret;
}

bool ModuleScene::CleanUp()
{
	std::stack<GameObject*> S;
	for (GameObject* child : root->children)	
	{
		S.push(child);
	}
	root->children.clear();

	while (!S.empty())
	{
		GameObject* go = S.top();
		S.pop();
		for (GameObject* child : go->children)
		{
			S.push(child);
		}
		go->children.clear();
		delete go;
	}

	delete root;

	return true;
}

bool ModuleScene::DeleteAllGameObjects()
{
	LOG("Deleting all game objects");

	root->~GameObject();
	gameObjectList.clear();


	return true;
}

bool ModuleScene::DeleteSelectedGameObject(GameObject* selectedGameObject)
{
	bool ret = true;

	if (selectedGameObject == nullptr)
		LOG("No Game Object has been selected!")
	else
	{
		LOG("Deleting game object: %s", selectedGameObject->name.c_str())
			if (selectedGameObject != root)
			{
				for (int i = 0; i < gameObjectList.size(); i++)
				{
					if (gameObjectList[i] == selectedGameObject && selectedGameObject->parent->name == "Root")
					{
						root->RemoveChild(selectedGameObject);
						gameObjectList.erase(gameObjectList.begin() + i);
					}
				}
			}
			else
			{
				root->~GameObject();
				gameObjectList.clear();
			}
	}

	return ret;
}

update_status ModuleScene::Update(float dt)
{
	if (warningMessage)
	{
		ImGui::Begin("Warning Message", &warningMessage);
		{
			ImGui::Text("Street Environment is not loaded at the \nstart because it messes up the inspector and \nhierarchy windows! After loading Street Environment\ndelete all game objects to use them properly.\n\nPd: Mouse pick is super unprecise but it works.");
			ImGui::End();
		}
	}

	if (!rootList.empty())
	{
		for (int i = 0; i < rootList.size(); ++i)
		{
			// Load S queue with all GameObjects
			std::queue<GameObject*> S;
			for (GameObject* child : rootList[i]->children)
			{
				S.push(child);
			}

			// Call to every GameObject Update
			while (!S.empty())
			{
				GameObject* go = S.front();
				go->Update(dt);
				S.pop();
				for (GameObject* child : go->children)
				{
					S.push(child);
				}
			}
		}
	}

	glDisable(GL_DEPTH_TEST);

	if (App->editor->gameobjectSelected)
	{
		ComponentTransform* transform = App->editor->gameobjectSelected->GetComponent<ComponentTransform>();
		float3 pos = transform->GetPosition();
		glLineWidth(10.f);
		glBegin(GL_LINES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(pos.x + transform->Right().x, pos.y + transform->Right().y, pos.z + transform->Right().z);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(pos.x + transform->Front().x, pos.y + transform->Front().y, pos.z + transform->Front().z);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(pos.x + transform->Up().x, pos.y + transform->Up().y, pos.z + transform->Up().z);
		glEnd();
		glLineWidth(1.f);
	}

	glEnable(GL_DEPTH_TEST);

	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const std::string name, GameObject* parent)
{
	GameObject* temp = new GameObject(name);

	if (name.empty())
		LOG("A name must be sent to CreateGameObject")
	else
	{
		if (parent)
		{
			LOG("Creating game object (%s)to %s", temp->name.c_str(), parent->name.c_str());
			parent->AttachChild(temp);
		}

		else
		{
			LOG("Creating game object (%s) at %s", temp->name.c_str(), root->name.c_str());
			root->AttachChild(temp);
		}
	}

	return temp;
}

GameObject* ModuleScene::CreateEmptyGameObject(GameObject* parent)
{
	GameObject* temp = new GameObject();
	if (emptyCounter > 0)
		temp->name += std::to_string(emptyCounter);
	emptyCounter++;

	if (parent)
	{
		LOG("Creating empty game object to %s", parent->name.c_str());
		parent->AttachChild(temp);
	}
	else
	{
		LOG("Creating empty game object at %s", root->name.c_str());
		root->AttachChild(temp);
	}


	return temp;
}

GameObject* ModuleScene::CreateChildrenGameObject(GameObject* parent)
{
	GameObject* temp = new GameObject();
	if (emptyCounter > 0)
		temp->name += std::to_string(emptyCounter);
	emptyCounter++;

	if (parent == nullptr)
		LOG("No Game Object has been selected!")
	else
	{
		LOG("Creating empty game object at %s", parent->name.c_str());
		parent->AttachChild(temp);
	}

	return temp;
}

void ModuleScene::DuplicateGameObject(GameObject* parent)
{
	if (parent == nullptr)
		LOG("You have to select a game object to duplicate!")
	else
	{
		if (parent->parent->name == "Root")
			root->AttachChild(parent);
		else parent->parent->AttachChild(parent);
	}
}

void ModuleScene::CreateRoot()
{
	rootList.clear();

	root = new GameObject("Root");
	gameObjectList.push_back(root);
	rootList.push_back(root);

	for (GameObject* child : root->children)
	{
		gameObjectList.push_back(child);
	}
}



void ModuleScene::OnSave(JSONWriter& writer) const
{
	for (int i = 0; i < gameObjectList.size(); i++)
	{
		gameObjectList[i]->OnSave(writer);
	}
		int i = 0;
}

void ModuleScene::OnLoad(const JSONReader& reader)
{
	for (int i = 0; i < gameObjectList.size(); i++)
	{
		gameObjectList[i]->OnLoad(reader);
	}
}
