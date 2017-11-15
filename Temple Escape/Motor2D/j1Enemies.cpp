#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Enemies.h"
#include "j1Textures.h"
#include "Enemy.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "Bat.h"
#include "Slime.h"
#include "j1Collider.h"
#include "j1Player.h"

#include "p2Log.h"
#include "p2Defs.h"

#include "j1Map.h"

#define SPAWN_MARGIN 500

j1Enemies::j1Enemies()
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
		enemies[i] = nullptr;

	name.create("enemies");
}

// Destructor
j1Enemies::~j1Enemies()
{

}

bool j1Enemies::Awake(pugi::xml_node& config)
{
	LOG("Loading Enemies from config file");
	bool ret = true;

	spritesheetName.create(config.child("spritesheetSource").attribute("name").as_string());
	
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

	}
	return ret;
}

bool j1Enemies::Start()
{
	bool ret = true;

	enemy_sprites = App->tex->Load(spritesheetName.GetString());

	if (enemy_sprites == NULL) {
		LOG("Error loading enemies spritesheet!!");
		ret = false;
	}

	App->win->GetWindowSize(screen_width, screen_height);
	screen_scale = App->win->GetScale();

	return ret;
}

bool j1Enemies::PreUpdate()
{
	// TODO: now we will spawn enemies in a different way ...
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type != ENEMY_TYPES::NO_TYPE)
		{
			if (queue[i].x * screen_scale < App->render->camera.x + (App->render->camera.w * screen_scale) + SPAWN_MARGIN)
			{
				SpawnEnemy(queue[i]);
				queue[i].type = ENEMY_TYPES::NO_TYPE;
				LOG ("Spawning enemy at %d", queue[i].x * App->win->GetScale());
			}
		}
	}
	return true;
}

// Called before render is available
bool j1Enemies::Update(float dt)
{
	bat_fly_right.speed = 10 * dt;
	bat_fly_left.speed = 10 * dt;

	for (uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) enemies[i]->Move(dt);

	for (uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) enemies[i]->Shoot(dt);

	for (uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) enemies[i]->Draw(enemy_sprites);

	return true;
}

bool j1Enemies::PostUpdate()
{
	// check camera position to decide what to despawn
	/*for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			int width, height;

			if (enemies[i]->position.x > screen_scale + SPAWN_MARGIN || enemies[i]->position.x < 0 - SPAWN_MARGIN)
			{
				LOG("DeSpawning enemy at %d", enemies[i]->position.x);
				delete enemies[i];
				enemies[i] = nullptr;
			}
			else if (enemies[i]->position.y > screen_scale + SPAWN_MARGIN || enemies[i]->position.y < 0 - SPAWN_MARGIN) {
				LOG("DeSpawning enemy at %d", enemies[i]->position.y);
				delete enemies[i];
				enemies[i] = nullptr;
			}
		}
	}*/

	return true;
}

// Called before quitting
bool j1Enemies::CleanUp()
{
	LOG("Freeing all enemies");

	App->tex->UnLoad(enemy_sprites);

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
	}

	return true;
}

bool j1Enemies::AddEnemy(ENEMY_TYPES type, int x, int y)
{
	bool ret = false;

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type == ENEMY_TYPES::NO_TYPE)
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

void j1Enemies::SpawnEnemy(const EnemyInfo& info)
{
	// find room for the new enemy
	uint i = 0;
	for (; enemies[i] != nullptr && i < MAX_ENEMIES; ++i);

	if (i != MAX_ENEMIES)
	{
		switch (info.type)
		{
		case ENEMY_TYPES::BAT:
			enemies[i] = new Bat(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::BAT;
			enemies[i]->standard_left_fly = bat_fly_left;
			enemies[i]->standard_right_fly = bat_fly_right;
			break;
		case ENEMY_TYPES::SLIME:
			enemies[i] = new Slime(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::SLIME;
			enemies[i]->standard_left_jump = slime_left_jump;
			enemies[i]->standard_right_jump = slime_right_jump;
			enemies[i]->standard_left_idle = slime_left_idle;
			enemies[i]->standard_right_idle = slime_right_idle;
			break;
		default:
			break;
		}
	}
}

void j1Enemies::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr && enemies[i]->GetCollider() == c1)
		{
			enemies[i]->OnCollision(c2);
			if (enemies[i]->type == ENEMY_TYPES::BAT)
			{
				if (enemies[i]->getLives() <= 0) 
				{
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BASIC_SHOT)
					{
						//App->audio->PlayFx(medium_explosion);
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
				}
			}
			if (enemies[i]->type == ENEMY_TYPES::SLIME)
			{
				if (enemies[i]->getLives() <= 0)
				{
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BASIC_SHOT)
					{
						//App->audio->PlayFx(medium_explosion);
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
				}
			}

		}
	} 
}