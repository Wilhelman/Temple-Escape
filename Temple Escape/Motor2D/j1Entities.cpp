#include "j1App.h"
#include "j1Render.h"
#include "j1Entities.h"
#include "j1Textures.h"
#include "Entity.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "j1Collider.h"
#include "p2Log.h"
#include "j1Map.h"

#include "Bat.h"
#include "Slime.h"
#include "Player.h"


j1Entities::j1Entities()
{
	name.create("entities");
}

// Destructor
j1Entities::~j1Entities()
{

}

bool j1Entities::Awake(pugi::xml_node& config)
{
	LOG("Loading Entities from config file");
	bool ret = true;

	spritesheetName.create(config.child("spritesheetSource").attribute("name").as_string());

	return ret;
}

bool j1Entities::Start()
{
	bool ret = true;

	entity_sprites = App->tex->Load(spritesheetName.GetString());

	if (entity_sprites == NULL) {
		LOG("Error loading entities spritesheet!!");
		ret = false;
	}

	if (App->map->spawn.x >= 0 && App->map->spawn.y >= 0) {
		if (!SpawnEntity(App->map->spawn.x, App->map->spawn.y, PLAYER)) {
			LOG("Error adding entity player ...");
			ret = false;
		}
	}

	if (!ret)
		return false;

	return ret;
}

bool j1Entities::PreUpdate()
{
	for (int i = 0; i < entities.Count(); i++) {
		if (entities[i]->to_destroy) {
			delete(entities[i]);
			entities[i] = nullptr;
			if (!entities.RemoveAt(i)) {
				LOG("Error removing entity");
				return false;
			}
		}
	}
	return true;
}

// Called before render is available
bool j1Entities::Update(float dt)
{

	for (int i = 0; i < entities.Count(); i++)
		if (entities[i] != nullptr) entities[i]->Update(dt);

	for (int i = 0; i < entities.Count(); i++)
		if (entities[i] != nullptr) entities[i]->Draw(entity_sprites);
	
	return true;
}

// Called before quitting
bool j1Entities::CleanUp()
{
	LOG("Freeing all enemies");

	App->tex->UnLoad(entity_sprites);

	for (int i = entities.Count() - 1; i >= 0; --i)
	{
		if (entities[i] != nullptr) {
			delete(entities[i]);
			entities[i] = nullptr;
			if (!entities.RemoveAt(i)) {
				LOG("Error removing entity");
				return false;
			}
		}
	}

	entities.Clear();

	return true;
}

bool j1Entities::SpawnEntity(int x, int y, EntityType type)
{
	// find room for the new entity
	bool ret = false;

	switch (type)
	{
	case EntityType::BAT: {
		Bat* batty = new Bat(x, y);
		batty->type = EntityType::BAT; //TODO PARAMETER TO CONSTRUCTOR!
		entities.PushBack(batty);
		ret = true;
		break;
	}
	case EntityType::SLIME: {
		Slime* slimmy = new Slime(x, y);
		slimmy->type = EntityType::SLIME; //TODO PARAMETER TO CONSTRUCTOR!
		entities.PushBack(slimmy);
		ret = true;
		break;
	}
	case EntityType::PLAYER: {
		Player* player = new Player(x, y);
		player->type = EntityType::PLAYER; //TODO PARAMETER TO CONSTRUCTOR!
		entities.PushBack(player);
		ret = true;
		break;
	}
	default:
		break;
	}
	

	return ret;
}

void j1Entities::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < entities.Count(); ++i)
		if (entities[i] != nullptr && entities[i]->GetCollider() == c1)
			entities[i]->OnCollision(c2);
}


bool j1Entities::Load(pugi::xml_node& load)
{
	bool ret = true;

	//delete all enemies but the player
	for (uint i = 0; i < entities.Count(); ++i)
	{
		if (entities[i] != nullptr){
			if (entities[i]->type != PLAYER) {
				entities[i]->to_destroy = true;
			}
		}
	}

	if (!load.child("player").empty())
	{
		pugi::xml_node& player_load = load.child("player");
		if(Player* tmpPlayer = this->GetPlayer())
			tmpPlayer->Load(player_load);
		else {
			LOG("ERROR: Trying to load a nonexist player ... j1Entities");
			return false;
		}
	}
			
	for (pugi::xml_node bat = load.child("bat"); bat && ret; bat = bat.next_sibling("bat"))
		SpawnEntity(bat.child("position").attribute("x").as_float(), bat.child("position").attribute("y").as_float(), BAT);
	
	for (pugi::xml_node slime = load.child("slime"); slime && ret; slime = slime.next_sibling("slime"))
		SpawnEntity(slime.child("position").attribute("x").as_float(), slime.child("position").attribute("y").as_float(), SLIME);

	return ret;
}

bool j1Entities::Save(pugi::xml_node& save) const
{
	bool ret = true;

	for (uint i = 0; i < entities.Count(); ++i)
	{
		if (entities[i] != nullptr)
		{
			if (entities[i]->type == PLAYER) {
				pugi::xml_node& player_save = save.append_child("player");
				entities[i]->Save(player_save);
			}else
			if (entities[i]->type == BAT) {
				pugi::xml_node& bat_save = save.append_child("bat");
				entities[i]->Save(bat_save);
			}
			else
			if (entities[i]->type == SLIME) {
				pugi::xml_node& slime_save = save.append_child("slime");
				entities[i]->Save(slime_save);
			}
		}
	}

	return ret;
}

Player* j1Entities::GetPlayer() const {

	for (uint i = 0; i < entities.Count(); ++i)
	{
		if (entities[i] != nullptr)
		{
			if (entities[i]->type == PLAYER)
				return (Player*)entities[i];
		}
	}

	return nullptr;

}