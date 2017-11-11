#include "j1App.h"
#include "Bat.h"
#include "j1Collider.h"
#include "j1Map.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Pathfinding.h"


//#include "Path.h"

Bat::Bat(int x, int y) : Enemy(x, y)
{
	bat_IA = 1;
	bat_going_right = true;
	moving = player_in_radar = have_to_chill = dead = false; //check have to chill
	lives = 2;

	animation = standard_right_fly;

	original_pos = App->map->WorldToMap(x, y);

	collider = App->collider->AddCollider({ 0, 0, 16, 12 }, COLLIDER_TYPE::COLLIDER_ENEMY_BAT, (j1Module*)App->enemies);

	SetRadar();
}

void Bat::Move()
{
	iPoint bat_pos = App->map->WorldToMap(position.x, position.y);

	if (have_to_chill && !player_in_radar && !moving && App->pathfinding->CreatePath(iPoint(bat_pos.x, bat_pos.y), original_pos) != -1) {

		const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
		if (path->Count() > 0) {
			movementGoal = iPoint(path->At(0)->x, path->At(0)->y);
			//TODO FOLLOW THIS..
			//ill do this working around NO DIAGONALS so this will need an update
			fPoint xSpeed(0.0f,0.0f), ySpeed(0.0f,0.0f);
			if (movementGoal.x < bat_pos.x) {
				xSpeed = { -0.5f,0.0f };
				animation = standard_left_fly;
			}
			else if (movementGoal.x > bat_pos.x) {
				xSpeed = { 0.5f,0.0f };
				animation = standard_right_fly;
			}
			
			if (movementGoal.y < bat_pos.y) {
				ySpeed = { 0.0f,-0.5f };
			}
			else if (movementGoal.y > bat_pos.y) {
				ySpeed = { 0.0f, 0.5f };
			}
			movementSpeed.x = xSpeed.x;
			movementSpeed.y = ySpeed.y;
			moving = true;
		}
		else {
			bat_IA = 1;
			have_to_chill = false;
		}
			
	}else if (player_in_radar && !moving && App->pathfinding->CreatePath(iPoint(bat_pos.x, bat_pos.y), playerGoal) != -1) {
		
		//LOG("x : %i y : %i", bat_pos.x, bat_pos.y);
		const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
		movementGoal = iPoint(path->At(0)->x, path->At(0)->y);
		//LOG("goal x : %i goal y : %i", movementGoal.x, movementGoal.y); 
		//TODO FOLLOW THIS..
		//ill do this working around NO DIAGONALS so this will need an update
		fPoint xSpeed(0.0f,0.0f), ySpeed(0.0f,0.0f);
		if (movementGoal.x < bat_pos.x) {
			xSpeed = { -0.5f,0.0f };
			animation = standard_left_fly;
		}
		else if (movementGoal.x > bat_pos.x) {
			xSpeed = { 0.5f,0.0f };
			animation = standard_right_fly;
		}

		if (movementGoal.y < bat_pos.y) {
			ySpeed = { 0.0f,-0.5f };
		}
		else if (movementGoal.y > bat_pos.y) {
			ySpeed = { 0.0f, 0.5f };
		}
		movementSpeed.x = xSpeed.x;
		movementSpeed.y = ySpeed.y;
		moving = true;
	}
	else if (bat_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);

		goal.x += 1;
		movementGoal = goal;
		moving = true;
		bat_IA++;
		movementSpeed = { 0.5f,0.0f };
		animation = standard_right_fly;

	}
	else if (!bat_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);
		goal.x -= 1;
		movementGoal = goal;
		moving = true;
		bat_IA--;
		movementSpeed = { -0.5f,0.0f };
		animation = standard_left_fly;

	}
	else {
		if (moving) {
			position = position + movementSpeed;
		}

		if (bat_pos == movementGoal) {
			moving = false;
			if (bat_IA == 3 || bat_IA == 0)
				bat_going_right = !bat_going_right;
			
			if(!App->player->isDead)
				player_in_radar = CheckForPlayer();

			if (!have_to_chill && player_in_radar)
				have_to_chill = true;
		}
	}
	/*LOG("BAT POS x : %i y : %i", bat_pos.x, bat_pos.y);
	LOG("MOV GOAL x : %i goal y : %i", movementGoal.x, movementGoal.y);
	LOG("ORIGINAL POS X: %i | ORIGINAL POS Y: %i", original_pos.x, original_pos.y);
	*/
}

void Bat::SetRadar() {
	uint counter = 0;
	
	for (int i = -3; i < 4; i++)
	{
		for (int k = -3; k < 4; k++)
		{
			iPoint tmp_radar = App->map->WorldToMap(position.x, position.y);
			//LOG("x : %i y : %i",tmp_radar.x,tmp_radar.y);
			tmp_radar.x += i;
			tmp_radar.y += k;
			tile_radar[counter++] = tmp_radar;
		}
	}
}

bool Bat::CheckForPlayer() {
	iPoint tmp_player = App->map->WorldToMap(App->player->position.x, App->player->position.y);

	for (uint i = 0; i < TILE_RADAR; i++)
	{
		if (tile_radar[i] == tmp_player) {
			//LOG("NANANANANANANA BAAAAT RADAAAAR ~ PLAYER IS IN THE BAT RADAR!!!");
			playerGoal = tile_radar[i];
			if (playerGoal == iPoint(App->map->WorldToMap(position.x, position.y)))
				return false;
			return true;
		}
	}
	return false;
}
uint Bat::getLives() 
{
	return lives;
}

void Bat::OnCollision(Collider* collider)
{
	if (collider->type == COLLIDER_TYPE::COLLIDER_PLAYER) 
	{
		player_in_radar = false;
		have_to_chill = true;
	}	
	if (collider->type == COLLIDER_TYPE::COLLIDER_PLAYER_BASIC_SHOT)
		lives--;

	if (lives <= 0)
	{
		if (this->collider != nullptr)
		{
			App->collider->EraseCollider(this->collider);
			this->collider = nullptr;
		}
		dead = true;
	}
}