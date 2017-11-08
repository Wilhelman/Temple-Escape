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
	bat_IA = 0;
	bat_going_right = true;
	moving = player_in_radar = false;

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
	if (player_in_radar && !moving) {
		iPoint bat_pos = App->map->WorldToMap(position.x, position.y);
		App->pathfinding->CreatePath(iPoint(bat_pos.x, bat_pos.y), movementGoal);
		LOG("x : %i y : %i", bat_pos.x, bat_pos.y);
		const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
		movementGoal = App->map->WorldToMap(path->At(0)->x, path->At(0)->y);
		LOG("goal x : %i goal y : %i", movementGoal.x, movementGoal.y);
		//TODO FOLLOW THIS..
		player_in_radar = CheckForPlayer();
	}
	else if (bat_going_right && !moving) {

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
		else if (moving &&player_in_radar) {

		}

		if ((int)position.x == (int)movementGoal.x) {
			moving = false;
			if (bat_IA == 3 || bat_IA == 0)
				bat_going_right = !bat_going_right;
			SetRadar();
			player_in_radar = CheckForPlayer();
		}
	}
	
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
			LOG("NANANANANANANA BAAAAT RADAAAAR ~ PLAYER IS IN THE BAT RADAR!!!");
			movementGoal = tile_radar[i];
			return true;
		}
	}
	return false;
}

void Bat::OnCollision(Collider* collider)
{
	/*if (collider->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || collider->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT) {
		//App->particles->AddParticle(App->particles->playerShotCollison, (collider->rect.x - (((collider->rect.w)) / 2)), (collider->rect.y - (((collider->rect.h)))));
		//animation = &bee_white;
	}*/
}