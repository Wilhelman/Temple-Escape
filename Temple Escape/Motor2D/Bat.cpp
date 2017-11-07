#include "j1App.h"
#include "Bat.h"
#include "j1Collider.h"
#include "j1Map.h"
//#include "Path.h"

Bat::Bat(int x, int y) : Enemy(x, y)
{
	bat_IA = 0;
	bat_going_right = true;
	moving = false;

	fly_r.PushBack({ 76, 35, 16, 12 });
	fly_r.PushBack({ 92, 35, 16, 12 });
	fly_r.PushBack({ 109, 35, 16, 12 });
	fly_r.speed = 0.15f;
	fly_r.loop = true;

	fly_l.PushBack({ 47, 182, 16, 12 });
	fly_l.PushBack({ 31, 182, 16, 12 });
	fly_l.PushBack({ 14, 182, 16, 12 });
	fly_l.speed = 0.15f;
	fly_l.loop = true;

	animation = &fly_r;

	original_pos.x = x;
	original_pos.y = y;


	collider = App->collider->AddCollider({ 0, 0, 16, 12 }, COLLIDER_TYPE::COLLIDER_ENEMY_BAT, (j1Module*)App->enemies);
}

void Bat::Move()
{
	if (bat_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);
		goal.x += 1;
		movementGoal = App->map->MapToWorld(goal.x, goal.y);
		moving = true;
		bat_IA++;
		
		animation = &fly_r;

	}
	else if (!bat_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);
		goal.x -= 1;
		movementGoal = App->map->MapToWorld(goal.x, goal.y);
		moving = true;
		bat_IA--;

		animation = &fly_l;

	}
	else {
		if (moving && bat_going_right) {
			position = position + movementSpeed;
		}
		else if (moving && !bat_going_right) {
			position = position - movementSpeed;
		}

		if ((int)position.x == (int)movementGoal.x) {
			moving = false;
			if (bat_IA == 3 || bat_IA == 0)
				bat_going_right = !bat_going_right;
		}
	}

	
	
}

void Bat::OnCollision(Collider* collider)
{
	/*if (collider->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || collider->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT) {
		//App->particles->AddParticle(App->particles->playerShotCollison, (collider->rect.x - (((collider->rect.w)) / 2)), (collider->rect.y - (((collider->rect.h)))));
		//animation = &bee_white;
	}*/
}