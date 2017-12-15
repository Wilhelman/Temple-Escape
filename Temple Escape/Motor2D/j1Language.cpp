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
	dictionary.continue_btn.create(node->child("continue_btn").attribute("string").as_string());
	dictionary.music_volume.create(node->child("music_volume").attribute("string").as_string());
	dictionary.fx_volume.create(node->child("fx_volume").attribute("string").as_string());
	dictionary.cap_to.create(node->child("cap_to").attribute("string").as_string());
	dictionary.language_option.create(node->child("language_option").attribute("string").as_string());
	dictionary.quit_lan.create(node->child("quit_lan").attribute("string").as_string());
	dictionary.save_lan.create(node->child("save_lan").attribute("string").as_string());
	dictionary.load_lan.create(node->child("load_lan").attribute("string").as_string());
	dictionary.main_menu_lan.create(node->child("main_menu_lan").attribute("string").as_string());
	
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
	dictionary.continue_btn.create(node->child("continue_btn").attribute("string").as_string());
	dictionary.music_volume.create(node->child("music_volume").attribute("string").as_string());
	dictionary.fx_volume.create(node->child("fx_volume").attribute("string").as_string());
	dictionary.cap_to.create(node->child("cap_to").attribute("string").as_string());
	dictionary.language_option.create(node->child("language_option").attribute("string").as_string());
	dictionary.quit_lan.create(node->child("quit_lan").attribute("string").as_string());
	dictionary.save_lan.create(node->child("save_lan").attribute("string").as_string());
	dictionary.load_lan.create(node->child("load_lan").attribute("string").as_string());
	dictionary.main_menu_lan.create(node->child("main_menu_lan").attribute("string").as_string());


}

Dictionary j1Language::GetDictionary() const 
{
	return dictionary;
}