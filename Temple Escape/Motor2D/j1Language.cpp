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
	current_language.create(config.attribute("current").as_string());

	for (pugi::xml_node languages = node->first_child() ; languages && ret; languages = languages.next_sibling())
	{
		p2SString tmp_language;
		tmp_language.create(languages.name());
		posible_languages.PushBack(tmp_language);
	}

	node = &node->child(current_language.GetString());

	dictionary.new_game_btn.create(node->child("new_game_btn").attribute("string").as_string());
	//dictionary.new_game_btn.create(node->child("continue_btn").attribute("string").as_string());	1

	
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


void j1Language::ChangeCurrentLanguage(p2SString new_language) {
	current_language = new_language;

	bool ret = true;

	pugi::xml_document	language_file;
	pugi::xml_node* node = &App->LoadLanguages(language_file);

	for (pugi::xml_node languages = node->first_child(); languages && ret; languages = languages.next_sibling())
	{
		p2SString tmp_language;
		tmp_language.create(languages.name());
		posible_languages.PushBack(tmp_language);
	}

	node = &node->child(current_language.GetString());

	dictionary.new_game_btn.create(node->child("new_game_btn").attribute("string").as_string());
	//dictionary.new_game_btn.create(node->child("continue_btn").attribute("string").as_string());	3

}

Dictionary j1Language::GetDictionary()const {
	return dictionary;
}
