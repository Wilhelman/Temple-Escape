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
	moving = key_entities_speed = false;
	lives = 3;

	animation = &standard_right_jump;

	original_pos.x = x;
	original_pos.y = y;

	collider = App->collider->AddCollider({ 0, 0, 16, 16 }, COLLIDER_TYPE::COLLIDER_ENEMY_SLIME, (j1Module*)App->enemies);

}

void Slime::Move(float dt)
{

	SetRadar();

	if (!key_entities_speed && dt > 0)
		SetEntitiesSpeed(dt);

	iPoint slime_pos_UP_LEFT = App->map->WorldToMap(position.x + 1, position.y + 1);
	iPoint slime_pos_DOWN_RIGHT = App->map->WorldToMap(position.x + collider->rect.w - 1, position.y + collider->rect.h - 1);


	if (App->map->collisionLayer->Get(slime_pos_UP_LEFT.x, slime_pos_UP_LEFT.y + 1) != 43 && !moving) {
		iPoint goal = slime_pos_UP_LEFT;
		goal.y += 1;
		movementGoal = goal;
		moving = true;

		float diference = this->position.y - 1 - App->map->MapToWorld(movementGoal.x, movementGoal.y).y;
		diference *= -1;
		if (this->position.x + collider->rect.w + ceil(GRAVITY_SLIME* dt) > diference) {
			iPoint tmpPos = App->map->WorldToMap(this->position.x, this->position.y - 1 + ceil(GRAVITY_SLIME* dt));
			if (App->map->collisionLayer->Get(tmpPos.x, tmpPos.y) == 43)
				movementSpeed = { 0.0f, diference };
			else
				movementSpeed = { 0.0f, ceil(GRAVITY_SLIME * dt) };
		}
		else
			movementSpeed = { 0.0f, ceil(GRAVITY_SLIME * dt) };

	}
	else if (player_in_radar && !moving) {

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

				float diference = position.x + collider->rect.w - App->map->MapToWorld(goal.x, goal.y).x;
				diference *= -1;
				if (position.x + collider->rect.w + ceil(SLIME_SPEED* dt) > diference) {
					iPoint tmpPos = App->map->WorldToMap(position.x + collider->rect.w + ceil(SLIME_SPEED* dt), position.y + 1);
					if (App->map->collisionLayer->Get(tmpPos.x, tmpPos.y) == 43)
						movementSpeed = { diference, 0.0f };
					else
						movementSpeed = { ceil(SLIME_SPEED * dt),0.0f };
				}
				else
					movementSpeed = { ceil(SLIME_SPEED * dt), 0.0f };

				animation = &standard_right_jump;
				animation->Reset();
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

				float diference = App->map->MapToWorld(goal.x, goal.y).x + 16 - position.x;
				diference *= -1;
				if (position.x - ceil(SLIME_SPEED* dt) > diference) {
					iPoint tmpPos = App->map->WorldToMap(position.x + collider->rect.w + ceil(SLIME_SPEED* dt), position.y + 1);
					if (App->map->collisionLayer->Get(tmpPos.x, tmpPos.y) == 43)
						movementSpeed = { -diference, 0.0f };
					else
						movementSpeed = { -ceil(SLIME_SPEED * dt),0.0f };
				}
				else
					movementSpeed = { -ceil(SLIME_SPEED * dt), 0.0f };


				animation = &standard_left_jump;
				animation->Reset();
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
			if (!App->player->isDead && !App->player->god_mode)
				player_in_radar = CheckForPlayer();
		}
	}
	LOG("SLIME LEFT UP POS x : %i y : %i", slime_pos_UP_LEFT.x, slime_pos_UP_LEFT.y);
	LOG("SLIME DOWN RIGHT POS x : %i y : %i", slime_pos_DOWN_RIGHT.x, slime_pos_DOWN_RIGHT.y);
	LOG("MOV GOAL x : %i goal y : %i", movementGoal.x, movementGoal.y);
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
		player_in_radar = false;
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

void Slime::SetRadar() {
	uint counter = 0;

	for (int i = -4; i < 5; i++)
	{
		for (int k = 0; k < 5; k++)
		{
			iPoint tmp_radar = App->map->WorldToMap(position.x, position.y);
			if (i != 0) {
				tmp_radar.x += i;
				tmp_radar.y += k;
				tile_radar[counter++] = tmp_radar;
			}
		}
	}

}

bool Slime::CheckForPlayer() {

	iPoint tmp_player = App->map->WorldToMap(App->player->position.x, App->player->position.y - 1);

	for (uint i = 0; i < TILE_RADAR; i++)
	{
		if (tile_radar[i] == tmp_player) {
			//LOG("NANANANANANANA SLIME RADAAAAR");
			//LOG("Player goal x = %i | player goal y = %i", playerGoal.x, playerGoal.y);
			playerGoal = tile_radar[i];
			if (playerGoal == iPoint(App->map->WorldToMap(position.x, position.y)))
				return false;
			return true;
		}
	}

	return false;
}

void Slime::SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position)
{
	movementGoal = iPoint(path->At(0)->x, path->At(0)->y);

	fPoint xSpeed(0.0f, 0.0f), ySpeed(0.0f, 0.0f);
	if (movementGoal.x < position.x) {

		float diference = App->map->MapToWorld(movementGoal.x, movementGoal.y).x + 16 - this->position.x;
		diference *= -1;
		if (this->position.x - ceil(ANGRY_SLIME_SPEED* dt) > diference) {
			iPoint tmpPos = App->map->WorldToMap(this->position.x + collider->rect.w + ceil(ANGRY_SLIME_SPEED* dt), this->position.y + 1);
			if (App->map->collisionLayer->Get(tmpPos.x, tmpPos.y) == 43)
				xSpeed = { -diference, 0.0f };
			else
				xSpeed = { -ceil(ANGRY_SLIME_SPEED * dt),0.0f };
		}
		else
			xSpeed = { -ceil(ANGRY_SLIME_SPEED * dt), 0.0f };

		animation = &standard_left_jump;
	}
	else if (movementGoal.x > position.x) {


		float diference = this->position.x + collider->rect.w - App->map->MapToWorld(movementGoal.x, movementGoal.y).x;
		diference *= -1;
		if (this->position.x + collider->rect.w + ceil(ANGRY_SLIME_SPEED* dt) > diference) {
			iPoint tmpPos = App->map->WorldToMap(this->position.x + collider->rect.w + ceil(ANGRY_SLIME_SPEED* dt), this->position.y + 1);
			if (App->map->collisionLayer->Get(tmpPos.x, tmpPos.y) == 43)
				xSpeed = { diference, 0.0f };
			else
				xSpeed = { ceil(ANGRY_SLIME_SPEED * dt),0.0f };
		}
		else
			xSpeed = { ceil(ANGRY_SLIME_SPEED * dt), 0.0f };

		animation = &standard_right_jump;
	}

	movementSpeed.x = xSpeed.x;
	movementSpeed.y = ySpeed.y;
	moving = true;
}

void Slime::SetEntitiesSpeed(float dt) {
	standard_left_idle.speed = standard_left_idle.speed*dt;
	standard_left_jump.speed = standard_left_jump.speed*dt;
	standard_right_idle.speed = standard_right_idle.speed*dt;
	standard_right_jump.speed = standard_right_jump.speed*dt;
	key_entities_speed = true;
}