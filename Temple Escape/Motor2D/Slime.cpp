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

	idle_right.PushBack({ 74, 85, 16, 11 });
	idle_right.PushBack({ 90, 85, 16, 11 });
	idle_right.speed = 0.15f;
	idle_right.loop = true;

	idle_left.PushBack({ 76, 50, 16, 11 });
	idle_left.PushBack({ 92, 50, 16, 11 });
	idle_left.speed = 0.15f;
	idle_left.loop = true;

	animation = &idle_left;

	original_pos.x = x;
	original_pos.y = y;

	collider = App->collider->AddCollider({ 0, 0, 16, 11 }, COLLIDER_TYPE::COLLIDER_ENEMY_SLIME, (j1Module*)App->enemies);
}

void Slime::Move()
{
	if (slime_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);
		goal.x += 1;
		movementGoal = App->map->MapToWorld(goal.x, goal.y);
		moving = true;
		slime_IA++;

		animation = &idle_right;

	}
	else if (!slime_going_right && !moving) {

		iPoint goal = App->map->WorldToMap(position.x, position.y);
		goal.x -= 1;
		movementGoal = App->map->MapToWorld(goal.x, goal.y);
		moving = true;
		slime_IA--;

		animation = &idle_left;
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
	}
}

void Slime::OnCollision(Collider* collider)
{
	/*if (collider->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || collider->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT) {
	//App->particles->AddParticle(App->particles->playerShotCollison, (collider->rect.x - (((collider->rect.w)) / 2)), (collider->rect.y - (((collider->rect.h)))));
	//animation = &bee_white;
	}*/
}