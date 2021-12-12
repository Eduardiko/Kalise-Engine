#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "imgui.h"

#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"

GameObject::GameObject()
{

	name = name + ("GameObject");
	parent = nullptr;

	transform = CreateComponent<ComponentTransform>();


	active = true;
}

GameObject::GameObject(const std::string name) : name(name)
{
	transform = CreateComponent<ComponentTransform>();

	active = true;
}


GameObject::~GameObject() {

	for (size_t i = 0; i < components.size(); i++)
	{
		RELEASE(components[i]);
	}

	components.clear();

	for (GameObject* go : children)
	{
		RELEASE(go);
	}

	parent = nullptr;
	active = false;
	isSelected = false;
	transform = nullptr;
}

void GameObject::Update(float dt)
{
	for (Component* component : components)
	{
		component->Update(dt);
	}
	parentUid = parent->uid;
}

void GameObject::OnGui()
{
	if (App->scene->root != this)
	{
		ImGui::Text("%s", name.c_str());
		ImGui::Separator();

		for (Component* component : components)
		{
			component->OnGui();
		}

	}
	if (ImGui::CollapsingHeader("Identifiers"))
	{
		ImGui::Text("UID: %" PRIu64, uid);
		ImGui::Text("Parent UID: %" PRIu64, parentUid);
	}
}

void GameObject::OnSave(JSONWriter& writer)
{

	uint64_t uid64 = uid;
	std::string uidstr = std::to_string(uid64);
	const char* uidchr = uidstr.c_str();

	writer.String(uidchr);
	writer.StartObject();
	SAVE_JSON_BOOL(active);
	SAVE_JSON_BOOL(isSelected);
	
	writer.EndObject();
	
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->OnSave(writer);
	}
}

void GameObject::OnLoad(const JSONReader& reader)
{
	uint64_t uid64 = uid;
	std::string uidstr = std::to_string(uid64);
	const char* uidchr = uidstr.c_str();

	if (reader.HasMember(uidchr))
	{
		const auto& config = reader[uidchr];

		LOAD_JSON_BOOL(active);
		LOAD_JSON_BOOL(isSelected);

	}



	for (int i = 0; i < components.size(); i++)
	{
		components[i]->OnLoad(reader);
	}
}



void GameObject::DeleteComponent(Component* component) {

	auto componentIt = std::find(components.begin(), components.end(), component);
	if (componentIt != components.end())
	{
		components.erase(componentIt);
		components.shrink_to_fit();
	}
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
}

void GameObject::AttachChild(GameObject* child)
{
	child->parent = this;
	children.push_back(child);
	child->transform->NewAttachment();
	child->PropagateTransform();
	App->scene->gameObjectList.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
	{
		children.erase(it);
	}
}

void GameObject::PropagateTransform()
{
	for (GameObject* go : children)
	{
		go->transform->OnParentMoved();
	}

}
