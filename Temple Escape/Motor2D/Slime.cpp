#include "j1App.h"
#include "Slime.h"
#include "j1Collider.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "p2Log.h"


Slime::Slime(int x, int y) : Enemy(x, y)
{
	slime_IA = slime_time_chilling = 0;
	slime_going_right = true;
	moving = false;
	lives = 3;

	animation = &standard_right_jump;

	original_pos.x = x;
	original_pos.y = y;

	collider = App->collider->AddCollider({ 0, 0, 16, 16 }, COLLIDER_TYPE::COLLIDER_ENEMY_SLIME, (j1Module*)App->enemies);
}

void Slime::Move(float dt)
{
		animation->speed = 10 * dt;

	iPoint slime_pos_UP_LEFT = App->map->WorldToMap(position.x + 1, position.y + 1);
	iPoint slime_pos_DOWN_RIGHT = App->map->WorldToMap(position.x + collider->rect.w - 1, position.y + collider->rect.h - 1);

	if (player_in_radar && !moving) {

		const p2DynArray<iPoint>* path1 = nullptr;
		const p2DynArray<iPoint>* path2 = nullptr;

		if (App->pathfinding->CreatePath(iPoint(slime_pos_UP_LEFT.x, slime_pos_UP_LEFT.y), playerGoal) != -1) {

			path1 = App->pathfinding->GetLastPath();
			if (App->pathfinding->CreatePath(iPoint(slime_pos_DOWN_RIGHT.x, slime_pos_DOWN_RIGHT.y), playerGoal) != -1)
				path2 = App->pathfinding->GetLastPath();

			if (path2 != nullptr && path1->Count() < path2->Count()) {
				SetMovementWithPath(path2, dt, slime_pos_DOWN_RIGHT);
			}
			else if (path1->Count() > 0) {
				SetMovementWithPath(path1, dt, slime_pos_UP_LEFT);
			}

		}
		else if (App->pathfinding->CreatePath(iPoint(slime_pos_DOWN_RIGHT.x, slime_pos_DOWN_RIGHT.y), playerGoal) != -1) {
			path1 = App->pathfinding->GetLastPath();
			if (App->pathfinding->CreatePath(iPoint(slime_pos_UP_LEFT.x, slime_pos_UP_LEFT.y), playerGoal) != -1)
				path2 = App->pathfinding->GetLastPath();

			if (path2 != nullptr && path1->Count() < path2->Count()) {
				SetMovementWithPath(path2, dt, slime_pos_UP_LEFT);
			}
			else if (path1->Count() > 0) {
				SetMovementWithPath(path1, dt, slime_pos_DOWN_RIGHT);
			}
		}

	}
	else if (slime_going_right && !moving && dt != 0.0f) {

		if (currentTime < slime_time_chilling + 500) {
			if (animation->Finished())
				animation = &standard_right_idle;
		}
		else {
			iPoint goal = slime_pos_UP_LEFT;

			if (App->map->collisionLayer->Get(goal.x + 1, goal.y + 1) == 43 && App->map->collisionLayer->Get(goal.x + 1, goal.y) != 43) {
				goal.x += 1;
				movementGoal = goal;
				moving = true;
				movementSpeed = { 10.0f * dt, 0.0f * dt };
				animation = &standard_right_jump;
			}
			else
				slime_going_right = !slime_going_right;
		}

	}
	else if (!slime_going_right && !moving  && dt != 0.0f) {

		if (currentTime < slime_time_chilling + 500) {
			if (animation->Finished())
				animation = &standard_left_idle;
		}
		else {
			iPoint goal = slime_pos_DOWN_RIGHT;
			if (App->map->collisionLayer->Get(goal.x - 1, goal.y + 1) == 43 && App->map->collisionLayer->Get(goal.x - 1, goal.y) != 43) {
				goal.x -= 1;
				movementGoal = goal;
				moving = true;
				movementSpeed = { -10.0f * dt,0.0f * dt };
				animation = &standard_left_jump;
			}
			else
				slime_going_right = !slime_going_right;
		}
	}
	else {
		if (moving) {
			position = position + movementSpeed;
		}

		if (slime_pos_UP_LEFT == movementGoal && slime_pos_DOWN_RIGHT == movementGoal) {
			moving = false;
			slime_time_chilling = SDL_GetTicks();
			//if (!App->player->isDead)
			//player_in_radar = CheckForPlayer();
		}
	}
	//LOG("BAT POS x : %i y : %i", bat_pos_UP_LEFT.x, bat_pos_UP_LEFT.y);
	//LOG("MOV GOAL x : %i goal y : %i", movementGoal.x, movementGoal.y);
	//LOG("ORIGINAL POS X: %i | ORIGINAL POS Y: %i", original_pos.x, original_pos.y);
	currentTime = SDL_GetTicks();
}

uint Slime::getLives()
{
	return lives;
}

void Slime::OnCollision(Collider* collider)
{
	if (collider->type == COLLIDER_TYPE::COLLIDER_PLAYER)
	{
		//player_in_radar = false;
		//have_to_chill = true;
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

void Slime::SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position)
{
	movementGoal = iPoint(path->At(0)->x, path->At(0)->y);

	fPoint xSpeed(0.0f, 0.0f), ySpeed(0.0f, 0.0f);
	if (movementGoal.x < position.x) {
		xSpeed = { -20.0f * dt, 0.0f * dt };
		animation = &standard_left_jump;
	}
	else if (movementGoal.x > position.x) {
		xSpeed = { 20.0f * dt,0.0f * dt };
		animation = &standard_right_jump;
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