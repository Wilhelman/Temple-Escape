#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Entities.h"
#include "j1Textures.h"
#include "Entity.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "Bat.h"
#include "Slime.h"
#include "j1Collider.h"
#include "Player.h"

#include "p2Log.h"
#include "p2Defs.h"

#include "j1Map.h"

#define SPAWN_MARGIN 500

j1Entities::j1Entities()
{
	for (uint i = 0; i < MAX_ENTITIES; ++i)
		entities[i] = nullptr;

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

	if (!App->entities->AddEntity(PLAYER, App->map->spawn.x, App->map->spawn.y)) {
		LOG("Error adding entity player ...");
		ret = false;
	}

	if (!ret)
		return false;

	//SpawnEntity(queue[0]); 
	//we know already that is the player. TODO: this looks bad

	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::NO_TYPE)
		{
			SpawnEntity(queue[i]);
			queue[i].type = ENTITY_TYPES::NO_TYPE;
			LOG("Spawning entity at %d", queue[i].x * App->win->GetScale());
		}
	}

	return ret;
}

bool j1Entities::PreUpdate()
{
	// TODO: now we will spawn enemies in a different way ...
	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::NO_TYPE)
		{
			SpawnEntity(queue[i]);
			queue[i].type = ENTITY_TYPES::NO_TYPE;
			LOG("Spawning entity at %d", queue[i].x * App->win->GetScale());
		}
	}
	return true;
}

// Called before render is available
bool j1Entities::Update(float dt)
{

	for (uint i = 0; i < MAX_ENTITIES; ++i)
		if (entities[i] != nullptr) entities[i]->Update(dt);

	for (uint i = 0; i < MAX_ENTITIES; ++i)
		if (entities[i] != nullptr) entities[i]->Draw(entity_sprites);

	return true;
}

// Called before quitting
bool j1Entities::CleanUp()
{
	LOG("Freeing all enemies");

	App->tex->UnLoad(entity_sprites);

	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (entities[i] != nullptr)
		{
			delete entities[i];
			entities[i] = nullptr;
		}
	}

	return true;
}

bool j1Entities::AddEntity(ENTITY_TYPES type, int x, int y)
{
	bool ret = false;

	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type == ENTITY_TYPES::NO_TYPE)
		{
			queue[i].type = type;
			queue[i].x = x;
			queue[i].y = y;
			ret = true;
			break;
		}
	}
	return ret;
}

void j1Entities::SpawnEntity(const EntityInfo& info)
{
	// find room for the new enemy
	uint i = 0;
	for (; entities[i] != nullptr && i < MAX_ENTITIES; ++i);

	if (i != MAX_ENTITIES)
	{
		switch (info.type)
		{
		case ENTITY_TYPES::BAT: {
			entities[i] = new Bat(info.x, info.y);
			entities[i]->type = ENTITY_TYPES::BAT;
			break;
		}
		case ENTITY_TYPES::SLIME: {
			entities[i] = new Slime(info.x, info.y);
			entities[i]->type = ENTITY_TYPES::SLIME;
			break;
		}
		case ENTITY_TYPES::PLAYER: {
			entities[i] = new Player(info.x,info.y);
			entities[i]->type = ENTITY_TYPES::PLAYER;
			break;
		}
		default:
			break;
		}
	}
}

void j1Entities::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (entities[i] != nullptr && entities[i]->GetCollider() == c1)
		{
			entities[i]->OnCollision(c2);
			if (entities[i]->type == ENTITY_TYPES::BAT)
			{
				if (entities[i]->getLives() <= 0)
				{
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BASIC_SHOT)
					{
						//App->audio->PlayFx(medium_explosion);
						delete entities[i];
						entities[i] = nullptr;
						break;
					}
				}
			}
			if (entities[i]->type == ENTITY_TYPES::SLIME)
			{
				if (entities[i]->getLives() <= 0)
				{
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BASIC_SHOT)
					{
						//App->audio->PlayFx(medium_explosion);
						delete entities[i];
						entities[i] = nullptr;
						break;
					}
				}
			}

		}
	} 
}


bool j1Entities::Load(pugi::xml_node& load)
{
	bool ret = true;

	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (entities[i] != nullptr)
		{
			if (entities[i]->type != PLAYER) {
				delete entities[i];
				entities[i] = nullptr;
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
	{
		AddEntity(BAT, bat.child("position").attribute("x").as_float(), bat.child("position").attribute("y").as_float());
	}
	
	for (pugi::xml_node slime = load.child("slime"); slime && ret; slime = slime.next_sibling("slime"))
	{
		AddEntity(SLIME, slime.child("position").attribute("x").as_float(), slime.child("position").attribute("y").as_float());
	}

	return ret;
}

bool j1Entities::Save(pugi::xml_node& save) const
{
	bool ret = true;

	for (uint i = 0; i < MAX_ENTITIES; ++i)
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

	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (entities[i] != nullptr)
		{
			if (entities[i]->type == PLAYER)
				return (Player*)entities[i];
		}
	}

	return nullptr;

}