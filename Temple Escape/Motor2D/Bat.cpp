#include "j1App.h"
#include "Bat.h"
#include "j1Collider.h"
#include "j1Map.h"
#include "Player.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1Entities.h"


Bat::Bat(int x, int y) : Entity(x, y)
{
	bat_IA = 1;
	bat_going_right = true;
	moving = player_in_radar = have_to_chill = key_entities_speed = false;
	lives = 2;

	animation = &standard_right_fly;

	original_pos = App->map->WorldToMap(x, y);

	collider = App->collider->AddCollider({ 0, 0, 16, 12 }, COLLIDER_TYPE::COLLIDER_ENEMY_BAT, (j1Module*)App->entities);

	SetRadar();
}

void Bat::Update(float dt)
{

	if (!key_entities_speed && dt > 0)
		SetEntitiesSpeed(dt);

	if (dt > 0) {
		(player_in_radar) ? standard_right_fly.speed = standard_right_fly_vel * 2 * dt : standard_right_fly.speed = standard_right_fly_vel * dt;
		(player_in_radar) ? standard_left_fly.speed = standard_left_fly_vel * 2 * dt : standard_left_fly.speed = standard_left_fly_vel * dt;
	}

	iPoint bat_pos_UP_LEFT = App->map->WorldToMap(position.x + 1, position.y + 1);
	iPoint bat_pos_DOWN_RIGHT = App->map->WorldToMap(position.x + collider->rect.w - 1, position.y + collider->rect.h - 1);

	if (have_to_chill && !player_in_radar && !moving) {
		const p2DynArray<iPoint>* path1 = nullptr;
		const p2DynArray<iPoint>* path2 = nullptr;
		if (App->pathfinding->CreatePath(iPoint(bat_pos_UP_LEFT.x, bat_pos_UP_LEFT.y), original_pos) != -1) {
			path1 = App->pathfinding->GetLastPath();
			if (App->pathfinding->CreatePath(iPoint(bat_pos_DOWN_RIGHT.x, bat_pos_DOWN_RIGHT.y), original_pos) != -1)
				path2 = App->pathfinding->GetLastPath();

			if (path2 != nullptr && path1->Count() < path2->Count()) {
				SetMovementWithPath(path2, dt, bat_pos_DOWN_RIGHT);
			}
			else if (path1->Count() > 0) {
				SetMovementWithPath(path1, dt, bat_pos_UP_LEFT);
			}


		}
		else if (App->pathfinding->CreatePath(iPoint(bat_pos_DOWN_RIGHT.x, bat_pos_DOWN_RIGHT.y), original_pos) != -1) {
			path1 = App->pathfinding->GetLastPath();
			if (App->pathfinding->CreatePath(iPoint(bat_pos_UP_LEFT.x, bat_pos_UP_LEFT.y), original_pos) != -1)
				path2 = App->pathfinding->GetLastPath();

			if (path2 != nullptr && path1->Count() < path2->Count()) {
				SetMovementWithPath(path2, dt, bat_pos_UP_LEFT);
			}
			else if (path1->Count() > 0) {
				SetMovementWithPath(path1, dt, bat_pos_DOWN_RIGHT);
			}
		}
		else {
			animation = &standard_right_fly;
			bat_IA = 1;
			bat_going_right = true;
			have_to_chill = false;
		}

	}
	else if (player_in_radar && !moving) {

		const p2DynArray<iPoint>* path1 = nullptr;
		const p2DynArray<iPoint>* path2 = nullptr;
		if (App->pathfinding->CreatePath(iPoint(bat_pos_UP_LEFT.x, bat_pos_UP_LEFT.y), playerGoal) != -1) {
			path1 = App->pathfinding->GetLastPath();
			if (App->pathfinding->CreatePath(iPoint(bat_pos_DOWN_RIGHT.x, bat_pos_DOWN_RIGHT.y), playerGoal) != -1)
				path2 = App->pathfinding->GetLastPath();

			if (path2 != nullptr && path1->Count() < path2->Count()) {
				SetMovementWithPath(path2, dt, bat_pos_DOWN_RIGHT);
			}
			else if (path1->Count() > 0) {
				SetMovementWithPath(path1, dt, bat_pos_UP_LEFT);
			}


		}
		else if (App->pathfinding->CreatePath(iPoint(bat_pos_DOWN_RIGHT.x, bat_pos_DOWN_RIGHT.y), playerGoal) != -1) {
			path1 = App->pathfinding->GetLastPath();
			if (App->pathfinding->CreatePath(iPoint(bat_pos_UP_LEFT.x, bat_pos_UP_LEFT.y), playerGoal) != -1)
				path2 = App->pathfinding->GetLastPath();

			if (path2 != nullptr && path1->Count() < path2->Count()) {
				SetMovementWithPath(path2, dt, bat_pos_UP_LEFT);
			}
			else if (path1->Count() > 0) {
				SetMovementWithPath(path1, dt, bat_pos_DOWN_RIGHT);
			}
		}

	}
	else if (bat_going_right && !moving && dt != 0.0f) {

		iPoint goal = bat_pos_UP_LEFT;

		goal.x += 1;
		movementGoal = goal;
		moving = true;
		bat_IA++;
		movementSpeed = { 10.0f * dt, 0.0f * dt };
		animation = &standard_right_fly;

	}
	else if (!bat_going_right && !moving  && dt != 0.0f) {

		iPoint goal = bat_pos_UP_LEFT;
		goal.x -= 1;
		movementGoal = goal;
		moving = true;
		bat_IA--;
		movementSpeed = { -10.0f * dt,0.0f * dt };
		animation = &standard_left_fly;

	}
	else {
		if (moving) {
			position = position + movementSpeed;
		}

		if (bat_pos_UP_LEFT == movementGoal && bat_pos_DOWN_RIGHT == movementGoal) {
			moving = false;
			if (bat_IA == 3 || bat_IA == 0)
				bat_going_right = !bat_going_right;

			if (!App->entities->player->isDead && !App->entities->player->god_mode)
				player_in_radar = CheckForPlayer();

			if (!have_to_chill && player_in_radar)
				have_to_chill = true;
		}
	}
	//LOG("BAT POS x : %i y : %i", bat_pos_UP_LEFT.x, bat_pos_UP_LEFT.y);
	//LOG("MOV GOAL x : %i goal y : %i", movementGoal.x, movementGoal.y);
	//LOG("ORIGINAL POS X: %i | ORIGINAL POS Y: %i", original_pos.x, original_pos.y);
}

void Bat::SetRadar() {
	uint counter = 0;

	for (int i = -3; i < 4; i++)
	{
		for (int k = -3; k < 4; k++)
		{
			iPoint tmp_radar = App->map->WorldToMap(position.x, position.y);
			if (!(i == 0 && k == 0)) {
				tmp_radar.x += i;
				tmp_radar.y += k;
				tile_radar[counter++] = tmp_radar;
			}
		}
	}

}

bool Bat::CheckForPlayer() {
	//LOG("CHECKING 4 PLAYER");
	iPoint tmp_player = App->map->WorldToMap(App->entities->player->position.x, App->entities->player->position.y - 1);

	for (uint i = 0; i < TILE_RADAR; i++)
	{
		if (tile_radar[i] == tmp_player) {
			//LOG("NANANANANANANA BAAAAT RADAAAAR");
			//LOG("Player goal x = %i | player goal y = %i", playerGoal.x, playerGoal.y);
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
	}
}

void Bat::SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position)
{
	movementGoal = iPoint(path->At(0)->x, path->At(0)->y);

	fPoint xSpeed(0.0f, 0.0f), ySpeed(0.0f, 0.0f);
	if (movementGoal.x < position.x) {
		xSpeed = { -20.0f * dt, 0.0f * dt };
		animation = &standard_left_fly;
	}
	else if (movementGoal.x > position.x) {
		xSpeed = { 20.0f * dt,0.0f * dt };
		animation = &standard_right_fly;
	}

	if (movementGoal.y < position.y) {
		ySpeed = { 0.0f * dt ,-20.0f * dt };
	}
	else if (movementGoal.y > position.y) {
		ySpeed = { 0.0f * dt, 20.0f * dt };
	}

	movementSpeed.x = xSpeed.x;
	movementSpeed.y = ySpeed.y;
	moving = true;
}

void Bat::SetEntitiesSpeed(float dt) {
	standard_right_fly_vel = standard_right_fly.speed;
	standard_left_fly_vel = standard_left_fly.speed;
	key_entities_speed = true;
}

bool Bat::Save(pugi::xml_node& save) const
{
	bool ret = true;

	if (save.child("position").empty())
	{
		save = save.append_child("position");
		save.append_attribute("x").set_value(position.x);
		save.append_attribute("y").set_value(position.y);
	}
	else
	{
		save.child("position").attribute("x").set_value(position.x);
		save.child("position").attribute("y").set_value(position.y);
	}

	return ret;
}