#include "j1App.h"
#include "Slime.h"
#include "j1Collider.h"
#include "j1Map.h"
//#include "Path.h"

Slime::Slime(int x, int y) : Enemy(x, y)
{
	slime_IA = 0;
	slime_going_right = true;
	moving = false;
	lives = 3;

	original_pos.x = x;
	original_pos.y = y;

	collider = App->collider->AddCollider({ 0, 0, 16, 16 }, COLLIDER_TYPE::COLLIDER_ENEMY_SLIME, (j1Module*)App->enemies);
}

void Slime::Move(float dt)
{
	/*if (slime_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);
		goal.x += 1;
		movementGoal = App->map->MapToWorld(goal.x, goal.y);
		moving = true;
		slime_IA++;

		//animation = &idle_right;

	}
	else if (!slime_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);
		goal.x -= 1;
		movementGoal = App->map->MapToWorld(goal.x, goal.y);
		moving = true;
		slime_IA--;

		//animation = &idle_left;
	}
	else {
		if (moving && slime_going_right) {
			position = position + movementSpeed;
		}
		else if (moving && !slime_going_right) {
			position = position - movementSpeed;
		}

		if ((int)position.x == (int)movementGoal.x) {
			moving = false;
			if (slime_IA == 3 || slime_IA == 0)
				slime_going_right = !slime_going_right;
		}
	}*/


	animation = &standard_left_idle;
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
		animation = &standard_left_idle;
	}
	else if (movementGoal.x > position.x) {
		xSpeed = { 20.0f * dt,0.0f * dt };
		animation = &standard_right_idle;
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