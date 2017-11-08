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
	for (uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) enemies[i]->Move();

	for (uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) enemies[i]->Shoot();

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
			break;
		case ENEMY_TYPES::SLIME:
			enemies[i] = new Slime(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::SLIME;
			break;
		/*case ENEMY_TYPES::METALLICBALLOON:
			enemies[i] = new Enemy_MetallicBalloon(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::METALLICBALLOON;
			break;*/
		default:
			break;
		}
	}
}

void j1Enemies::OnCollision(Collider* c1, Collider* c2)
{
	/*for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr && enemies[i]->GetCollider() == c1)
		{
			enemies[i]->OnCollision(c2);
			if (enemies[i]->type == ENEMY_TYPES::METALLICBALLOON) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						App->ui->score += 500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						App->ui->scoreP2 += 500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::BIG_RED_TURRET) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						App->ui->score += 600;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						this->AddEnemy(ENEMY_TYPES::BOMB, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						App->ui->scoreP2 += 600;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::TERRESTIALTURRET) {
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
					if (enemies[i]->getLives() <= 0) {
						App->ui->score += 500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->terrestialTurretExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						//delete enemies[i];
						//enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
					if (enemies[i]->getLives() <= 0) {
						App->ui->scoreP2 += 500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->terrestialTurretExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						//delete enemies[i];
						//enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::RED_TURRET) {
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
					if (enemies[i]->getLives() <= 0) {
						App->ui->score += 100;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->terrestialTurretExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
					if (enemies[i]->getLives() <= 0) {
						App->ui->scoreP2 += 100;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->terrestialTurretExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::TORPEDO) {
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
					App->ui->score += 200;
					App->audio->PlayFx(medium_explosion);
					App->particles->AddParticle(App->particles->torpedoExplosion, (c1->rect.x - ((49 - (c1->rect.w)) / 2)), (c1->rect.y - ((35 - (c1->rect.h)) / 2)));
					delete enemies[i];
					enemies[i] = nullptr;
					break;
				}
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
					App->ui->scoreP2 += 200;
					App->audio->PlayFx(medium_explosion);
					App->particles->AddParticle(App->particles->torpedoExplosion, (c1->rect.x - ((49 - (c1->rect.w)) / 2)), (c1->rect.y - ((35 - (c1->rect.h)) / 2)));
					delete enemies[i];
					enemies[i] = nullptr;
					break;
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::BEE) {
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
					//this->AddEnemy(ENEMY_TYPES::BOMB, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
					App->ui->score += 200;
					App->audio->PlayFx(medium_explosion);
					App->particles->AddParticle(App->particles->torpedoExplosion, (c1->rect.x - ((49 - (c1->rect.w)) / 2)), (c1->rect.y - ((35 - (c1->rect.h)) / 2)));
					delete enemies[i];
					enemies[i] = nullptr;
					break;
				}
				if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
					//this->AddEnemy(ENEMY_TYPES::BOMB, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
					App->ui->scoreP2 += 200;
					App->audio->PlayFx(medium_explosion);
					App->particles->AddParticle(App->particles->torpedoExplosion, (c1->rect.x - ((49 - (c1->rect.w)) / 2)), (c1->rect.y - ((35 - (c1->rect.h)) / 2)));
					delete enemies[i];
					enemies[i] = nullptr;
					break;
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::TRUMP_RED_MECHA) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->score += 6000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->scoreP2 += 6000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::FLYING_MACHINE) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						this->AddEnemy(ENEMY_TYPES::BOMB, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->score += 5000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						this->AddEnemy(ENEMY_TYPES::BOMB, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->scoreP2 += 5000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::CASTLE_HOUSEFLAG) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->score += 3000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->scoreP2 += 3000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::FOREST_HOUSE) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 65, c1->rect.y + 50, ENEMY_MOVEMENT::STAY);
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 85, c1->rect.y + 50, ENEMY_MOVEMENT::STAY);
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 45, c1->rect.y + 50, ENEMY_MOVEMENT::STAY);
						App->ui->score += 100;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((129 - (c1->rect.w)) / 2)), (c1->rect.y - ((100 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 65, c1->rect.y + 50, ENEMY_MOVEMENT::STAY);
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 85, c1->rect.y + 50, ENEMY_MOVEMENT::STAY);
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 45, c1->rect.y + 50, ENEMY_MOVEMENT::STAY);
						App->ui->scoreP2 += 100;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((129 - (c1->rect.w)) / 2)), (c1->rect.y - ((100 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
				}
			}
			else if (enemies[i]->type == ENEMY_TYPES::CASTLE_HOUSEFLAG_2) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->score += 3000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						this->AddEnemy(ENEMY_TYPES::POWER_UP, c1->rect.x, c1->rect.y, ENEMY_MOVEMENT::NO_MOVEMENT);
						App->ui->scoreP2 += 3000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->balloonDeathExplosion, (c1->rect.x - ((101 - (c1->rect.w)) / 2)), (c1->rect.y - ((107 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						break;
					}
				}
			}

			else if (enemies[i]->type == ENEMY_TYPES::CASTLE_VASE) {
				if (enemies[i]->getLives() <= 0) {
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						App->ui->score += 500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->vaseExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 13, c1->rect.y + 28, ENEMY_MOVEMENT::STAY);
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						App->ui->scoreP2 += 500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->vaseExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						this->AddEnemy(ENEMY_TYPES::COIN, c1->rect.x + 13, c1->rect.y + 28, ENEMY_MOVEMENT::STAY);
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
			}

			else if (enemies[i]->type == ENEMY_TYPES::FOREST_BOSS_HAND) {
				if (enemies[i]->getLives() <= 0) {
					handsDestroyed++;
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						App->ui->score += 4500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->vaseExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						App->ui->scoreP2 += 4500;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->vaseExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
			}

			else if (enemies[i]->type == ENEMY_TYPES::BOSS_FOREST) {
				if (enemies[i]->getLives() <= 0) {
					bossDestroyed = true;
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) {
						App->ui->score += 8000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->deathPlayerExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						App->particles->AddParticle(App->particles->deathPlayerExplosion, (c1->rect.x - ((120 - (c1->rect.w)) / 2)), (c1->rect.y - ((20 - (c1->rect.h)) / 2)));
						App->particles->AddParticle(App->particles->deathPlayerExplosion, (c1->rect.x - ((20 - (c1->rect.w)) / 2)), (c1->rect.y - ((90 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
					if (c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB) {
						App->ui->scoreP2 += 8000;
						App->audio->PlayFx(medium_explosion);
						App->particles->AddParticle(App->particles->deathPlayerExplosion, (c1->rect.x - ((58 - (c1->rect.w)) / 2)), (c1->rect.y - ((66 - (c1->rect.h)) / 2)));
						App->particles->AddParticle(App->particles->deathPlayerExplosion, (c1->rect.x - ((120 - (c1->rect.w)) / 2)), (c1->rect.y - ((20 - (c1->rect.h)) / 2)));
						App->particles->AddParticle(App->particles->deathPlayerExplosion, (c1->rect.x - ((20 - (c1->rect.w)) / 2)), (c1->rect.y - ((90 - (c1->rect.h)) / 2)));
						delete enemies[i];
						enemies[i] = nullptr;
						LOG("Result is: %f", c1->rect.x - ((42 - (c1->rect.w)) / 2));
						break;
					}
				}
			}


			else if (enemies[i]->type == ENEMY_TYPES::COIN && c2->type == COLLIDER_TYPE::COLLIDER_PLAYER) {
				App->ui->score += 200;
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
			else if (enemies[i]->type == ENEMY_TYPES::POWER_UP && c2->type == COLLIDER_TYPE::COLLIDER_PLAYER) {
				App->ui->score += 2000;
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
			else if (enemies[i]->type == ENEMY_TYPES::BOMB && c2->type == COLLIDER_TYPE::COLLIDER_PLAYER) {
				App->ui->score += 2000;
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
			else if (enemies[i]->type == ENEMY_TYPES::BOMB && c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2) {
				App->ui->scoreP2 += 2000;
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
			else if (enemies[i]->type == ENEMY_TYPES::COIN && c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2) {
				App->ui->scoreP2 += 200;
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
			else if (enemies[i]->type == ENEMY_TYPES::POWER_UP && c2->type == COLLIDER_TYPE::COLLIDER_PLAYER2) {
				App->ui->scoreP2 += 2000;
				delete enemies[i];
				enemies[i] = nullptr;
				break;
			}
		}
	}*/
}