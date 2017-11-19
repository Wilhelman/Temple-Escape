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
	fxPlayerJump.create(config.child("fxPlayerJump").attribute("name").as_string());
	fxPlayerDead.create(config.child("fxPlayerDead").attribute("name").as_string());
	
	for (pugi::xml_node animations = config.child("spritesheetSource").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
	{
		p2SString tmp(animations.attribute("name").as_string());

		if (tmp == "slime_right_idle") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				slime_right_idle.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			slime_right_idle.speed = animations.attribute("speed").as_float();
			slime_right_idle.loop = animations.attribute("loop").as_bool();
		}

		if (tmp == "slime_left_idle") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				slime_left_idle.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			slime_left_idle.speed = animations.attribute("speed").as_float();
			slime_left_idle.loop = animations.attribute("loop").as_bool();
		}

		if (tmp == "slime_right_jump") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				slime_right_jump.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			slime_right_jump.speed = animations.attribute("speed").as_float();
			slime_right_jump.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "slime_left_jump") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				slime_left_jump.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			slime_left_jump.speed = animations.attribute("speed").as_float();
			slime_left_jump.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "bat_fly_right") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				bat_fly_right.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			bat_fly_right.speed = animations.attribute("speed").as_float();
			bat_fly_right.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "bat_fly_left") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				bat_fly_left.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			bat_fly_left.speed = animations.attribute("speed").as_float();
			bat_fly_left.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_idle") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_right_idle.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_right_idle.speed = animations.attribute("speed").as_float();
			player_right_idle.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_idle") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_left_idle.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_left_idle.speed = animations.attribute("speed").as_float();
			player_left_idle.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_run") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_right_run.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_right_run.speed = animations.attribute("speed").as_float();
			player_right_run.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_run") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_left_run.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_left_run.speed = animations.attribute("speed").as_float();
			player_left_run.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_jump") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_right_jump.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_right_jump.speed = animations.attribute("speed").as_float();
			player_right_jump.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_jump") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_left_jump.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_left_jump.speed = animations.attribute("speed").as_float();
			player_left_jump.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_dead") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_right_dead.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_right_dead.speed = animations.attribute("speed").as_float();
			player_right_dead.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_dead") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_left_dead.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_left_dead.speed = animations.attribute("speed").as_float();
			player_left_dead.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_shoot") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_right_shoot.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_right_shoot.speed = animations.attribute("speed").as_float();
			player_right_shoot.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_shoot") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				player_left_shoot.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			player_left_shoot.speed = animations.attribute("speed").as_float();
			player_left_shoot.loop = animations.attribute("loop").as_bool();
		}

	}
	return ret;
}

bool j1Entities::Start()
{
	bool ret = true;

	entity_sprites = App->tex->Load(spritesheetName.GetString());

	if (entity_sprites == NULL) {
		LOG("Error loading enemies spritesheet!!");
		ret = false;
	}

	App->win->GetWindowSize(screen_width, screen_height);
	screen_scale = App->win->GetScale();

	LOG("Loading player audios");
	player_jump = App->audio->LoadFx(fxPlayerJump.GetString());
	player_dead = App->audio->LoadFx(fxPlayerDead.GetString());


	App->entities->AddEntity(PLAYER, App->map->spawn.x, App->map->spawn.y);

	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::NO_TYPE)
		{
			if (queue[i].x * screen_scale < App->render->camera.x + (App->render->camera.w * screen_scale) + SPAWN_MARGIN)
			{
				SpawnEntity(queue[i]);
				queue[i].type = ENTITY_TYPES::NO_TYPE;
				LOG("Spawning entity at %d", queue[i].x * App->win->GetScale());
			}
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
			if (queue[i].x * screen_scale < App->render->camera.x + (App->render->camera.w * screen_scale) + SPAWN_MARGIN)
			{
				SpawnEntity(queue[i]);
				queue[i].type = ENTITY_TYPES::NO_TYPE;
				LOG ("Spawning entity at %d", queue[i].x * App->win->GetScale());
			}
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

	LOG("Unloading player sound fx");
	App->audio->UnLoadFx(player_jump);
	App->audio->UnLoadFx(player_dead);

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
			Bat* bat = new Bat(info.x, info.y);
			entities[i] = bat;
			entities[i]->type = ENTITY_TYPES::BAT;
			bat->standard_left_fly = bat_fly_left;
			bat->standard_right_fly = bat_fly_right;
			break;
		}
		case ENTITY_TYPES::SLIME: {
			Slime* slime = new Slime(info.x, info.y);
			entities[i] = slime;
			entities[i]->type = ENTITY_TYPES::SLIME;
			slime->standard_left_jump = slime_left_jump;
			slime->standard_right_jump = slime_right_jump;
			slime->standard_left_idle = slime_left_idle;
			slime->standard_right_idle = slime_right_idle;
			break;
		}
		case ENTITY_TYPES::PLAYER: {
			player = new Player(info.x, info.y);
			entities[i] = player;
			entities[i]->type = ENTITY_TYPES::PLAYER;
			player->right_idle = player_right_idle;
			player->left_idle = player_left_idle;
			player->right_jump = player_right_jump;
			player->left_jump = player_left_jump;
			player->right_run = player_right_run;
			player->left_run = player_left_run;
			player->right_death_blink = player_right_dead;
			player->left_death_blink = player_left_dead;
			player->right_shoot = player_right_shoot;
			player->left_shoot = player_left_shoot;
			player->player_jump = player_jump;
			player->player_dead = player_dead;
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

			if (entities[i]->type == ENTITY_TYPES::PLAYER)
			{
				if (((c2->type == COLLIDER_ENEMY_BAT || c2->type == COLLIDER_ENEMY_SLIME) && !player->isDead) && !player->god_mode) {
					player->isDead = true;
					App->audio->PlayFx(player_dead);
					player->deadTime = SDL_GetTicks();
				}

				if (c2->type == COLLIDER_LVL_END)
				{
					if (!player->reachedEnd)
						player->reachedEnd = true;
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
		player->Load(player_load);
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
				if (save.child("player").empty())
				{
					pugi::xml_node& player_save = save.append_child("player");
					entities[i]->Save(player_save);
				}
				else
					entities[i]->Save(save);
			}

			if (entities[i]->type == BAT) {
				pugi::xml_node& bat_save = save.append_child("bat");
				entities[i]->Save(bat_save);
			}

			if (entities[i]->type == SLIME) {
				pugi::xml_node& slime_save = save.append_child("slime");
				entities[i]->Save(slime_save);
			}
		}
	}

	return ret;
}