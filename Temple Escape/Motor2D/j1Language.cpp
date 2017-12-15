#include "p2Defs.h"
#include "p2Log.h"
#include "j1Language.h"
#include "j1App.h"


j1Language::j1Language() : j1Module()
{
	name.create("language");
}

// Destructor
j1Language::~j1Language()
{}

bool j1Language::Load(pugi::xml_node& save) {
	bool ret = true;

	return ret;
}

bool j1Language::Save(pugi::xml_node& save)const {
	bool ret = true;

	return ret;
}

// Called before render is available
bool j1Language::Awake(pugi::xml_node& config)
{
	LOG("Loading Languages");
	bool ret = true;

	pugi::xml_document	language_file;
	pugi::xml_node* node = &App->LoadLanguages(language_file);
	node = &node->child(config.attribute("current").as_string());

	dictionary.new_game_btn.create(node->child("new_game_btn").attribute("string").as_string());
	
	return ret;
}

bool j1Language::Update(float dt)
{
	

	return true;
}

// Called before quitting
bool j1Language::CleanUp()
{

	return true;
}


Dictionary j1Language::GetDictionary()const {
	return dictionary;
}
