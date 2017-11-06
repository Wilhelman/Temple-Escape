#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Enemies.h"
#include "j1Textures.h"
#include "Enemy.h"
#include "j1Audio.h"
#include "j1Window.h"

#include "p2Log.h"
#include "p2Defs.h"

#define SPAWN_MARGIN 500

j1Enemies::j1Enemies()
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
		enemies[i] = nullptr;

}

// Destructor
j1Enemies::~j1Enemies()
{
	name.create("enemies");
}


bool j1Enemies::Awake(pugi::xml_node& config)
{
	LOG("Loading Player from config file");
	bool ret = true;

	//set all the animations


	return ret;
}

bool j1Enemies::Start()
{
	// Create a prototype for each enemy available so we can copy them around
	/*sprites = App->textures->Load("Assets/maps/castle/enemies_castle.png");*/

	enemy_sprites = App->tex->Load("Assets/enemies/enemies_definitive.png");

	App->win->GetWindowSize(screen_width, screen_height);
	screen_scale = App->win->GetScale();

	return true;
}

bool j1Enemies::PreUpdate()
{
	// check camera position to decide what to spawn
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
bool j1Enemies::Update()
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
	// check camera position to decide what to spawn
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			int width, height;

			if (enemies[i]->position.x > screen_scale + SPAWN_MARGIN || enemies[i]->position.x < 0 - SPAWN_MARGIN)
			{
				LOG("DeSpawning enemy at %d", enemies[i]->position.x);
				//enemies[i]->GetCollider()->to_delete = true;
				delete enemies[i];
				enemies[i] = nullptr;
			}
			else if (enemies[i]->position.y > screen_scale + SPAWN_MARGIN || enemies[i]->position.y < 0 - SPAWN_MARGIN) {
				LOG("DeSpawning enemy at %d", enemies[i]->position.y);
				//enemies[i]->GetCollider()->to_delete = true;
				delete enemies[i];
				enemies[i] = nullptr;
			}
		}
	}

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
			//enemies[i]->GetCollider()->to_delete = true;
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
		/*switch (info.type)
		{
		case ENEMY_TYPES::TORPEDO:
			enemies[i] = new Enemy_Torpedo(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::TORPEDO;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::TORPEDO_DIAGONALL_R:
				enemies[i]->movement = diagonalPathL_R;
				break;
			case ENEMY_MOVEMENT::TORPEDO_HORIZONTALR_L:
				enemies[i]->movement = horizontalPathR_L;
				break;
			case ENEMY_MOVEMENT::TORPEDO_STRAIGHT_ON:
				enemies[i]->movement = strightOnPath;
				break;
			case ENEMY_MOVEMENT::TORPEDO_DIAGONAL_R_FINAL:
				enemies[i]->movement = diagonalPath_R_Final;
				break;
			case ENEMY_MOVEMENT::TORPEDO_DIAGONAL_L_FINAL:
				enemies[i]->movement = diagonalPath_L_Final;
				break;
			case ENEMY_MOVEMENT::TORPEDO_DIAGONAL_R_FINAL2:
				enemies[i]->movement = diagonalPath_R_Final2;
				break;
			case ENEMY_MOVEMENT::TORPEDO_DIAGONAL_L_FINAL2:
				enemies[i]->movement = diagonalPath_L_Final2;
				break;
			default:
				break;
			}
			break;
		case ENEMY_TYPES::METALLICBALLOON:
			enemies[i] = new Enemy_MetallicBalloon(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::METALLICBALLOON;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::BALLOON_PATH_CASTLE:
				enemies[i]->movement = balloonPathCastle;
				break;
			case ENEMY_MOVEMENT::BALLOON_PATH_FOREST:
				enemies[i]->movement = balloonPathForest;
				break;
			default:
				break;
			}
			break;
		case ENEMY_TYPES::TERRESTIALTURRET:
			enemies[i] = new Enemy_TerrestialTurret(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::TERRESTIALTURRET;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::TURRET_1_PATH:
				enemies[i]->movement = turret1_path;
				break;
			case ENEMY_MOVEMENT::TURRET1_L_PATH:
				enemies[i]->movement = turret1_L_path;
				break;
			case ENEMY_MOVEMENT::TURRET_2_PATH:
				enemies[i]->movement = turret2_path;
				break;
			case ENEMY_MOVEMENT::TURRET_3_PATH:
				enemies[i]->movement = turret3_path;
				break;
			case ENEMY_MOVEMENT::STAY:
				enemies[i]->movement = stayPath;
				break;

			default:
				break;
			}
			break;
		case ENEMY_TYPES::BEE:
			enemies[i] = new Enemy_Bee(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::BEE;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::BEE_CORNER_LEFT_PATH:
				enemies[i]->movement = bee_corner_left_path;
				break;
			case ENEMY_MOVEMENT::BEE_CORNER_RIGHT_PATH:
				enemies[i]->movement = bee_corner_right_path;
				break;
			case ENEMY_MOVEMENT::BEE_CORNER_LEFT_PATH2:
				enemies[i]->movement = bee_corner_left_path2;
				break;
			case ENEMY_MOVEMENT::BEE_CORNER_RIGHT_PATH2:
				enemies[i]->movement = bee_corner_right_path2;
				break;
			case ENEMY_MOVEMENT::BEE_CORNER_STRAIGHT:
				enemies[i]->movement = bee_corner_straight;
				break;
			default:
				break;
			}
			break;
		case ENEMY_TYPES::RED_TURRET:
			enemies[i] = new Enemy_Red_Turret(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::RED_TURRET;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::STAY:
				enemies[i]->movement = stayPath;
				break;

			case ENEMY_MOVEMENT::RED_TURRET_LEFT_RIGTH:
				enemies[i]->movement = red_turret_left_rigth;
				break;

			case ENEMY_MOVEMENT::RED_TURRET_RIGHT_LEFT:
				enemies[i]->movement = red_turret_rigth_left;
				break;

			default:
				break;
			}
			break;
		case ENEMY_TYPES::BIG_RED_TURRET:
			enemies[i] = new Enemy_Big_Red_Turret(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::BIG_RED_TURRET;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::STAY:
				enemies[i]->movement = stayPath;
				break;

			default:
				break;
			}
			break;
		case ENEMY_TYPES::TRUMP_RED_MECHA:
			enemies[i] = new Enemy_Trump_Red_Mecha(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::TRUMP_RED_MECHA;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::TRUMP_RED_MECHA_PATH:
				enemies[i]->movement = trump_red_mecha_path;
				break;
			default:
				break;
			}
			break;
		case ENEMY_TYPES::FLYING_MACHINE:
			enemies[i] = new Enemy_Flying_Machine(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::FLYING_MACHINE;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::FLYING_MACHINE_PATH_1:
				enemies[i]->movement = flying_machine_path1;
				break;
			case ENEMY_MOVEMENT::FLYING_MACHINE_PATH_2:
				enemies[i]->movement = flying_machine_path2;
				break;
			default:
				break;
			}
			break;
		case ENEMY_TYPES::FOREST_HOUSE:
			enemies[i] = new SceneForest_house(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::FOREST_HOUSE;
			enemies[i]->movement = stayPath;
			break;

		case ENEMY_TYPES::CASTLE_HOUSEFLAG:
			enemies[i] = new SceneCastle_houseFlag(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::CASTLE_HOUSEFLAG;
			enemies[i]->movement = stayPath;
			break;
		case ENEMY_TYPES::CASTLE_HOUSEFLAG_2:
			enemies[i] = new SceneCastle_houseFlag2(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::CASTLE_HOUSEFLAG_2;
			enemies[i]->movement = stayPath;
			break;
		case ENEMY_TYPES::CASTLE_VASE:
			enemies[i] = new SceneCastle_Vase(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::CASTLE_VASE;
			enemies[i]->movement = stayPath;
			break;
		case ENEMY_TYPES::COIN:
			enemies[i] = new Coin(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::COIN;
			enemies[i]->movement = stayPath;
			break;
		case ENEMY_TYPES::POWER_UP:
			enemies[i] = new Power_Up(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::POWER_UP;
			break;
		case ENEMY_TYPES::BOMB:
			enemies[i] = new Bomb(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::BOMB;
			break;
		case ENEMY_TYPES::FOREST_BOSS_HAND:
			enemies[i] = new Enemy_BossMecha_Hand(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::FOREST_BOSS_HAND;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::BOSS_FOREST_HAND:
				enemies[i]->movement = bossForestHand;
				break;
			default:
				break;
			}
			break;
		case ENEMY_TYPES::BOSS_FOREST:
			enemies[i] = new Enemy_Mecha_Boss(info.x, info.y);
			enemies[i]->type = ENEMY_TYPES::BOSS_FOREST;
			switch (info.typeMovement)
			{
			case ENEMY_MOVEMENT::BOSS_FOREST_HAND:
				enemies[i]->movement = bossForestHand;
				break;
			default:
				break;
			}
			break;
		}
	}*/
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