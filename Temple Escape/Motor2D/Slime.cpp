#include "j1App.h"
#include "Slime.h"
#include "j1Collider.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "p2Log.h"
#include "j1Entities.h"
#include "Player.h"

#define SLIME_MAX_LIVES 3
#define GRAVITY_SLIME 20
#define SLIME_SPEED 5
#define ANGRY_SLIME_SPEED 10

Slime::Slime(int x, int y) : Entity(x, y)
{

	bool ret = true;

	//TODO : this looks ugly but it seems the only way :'c
	pugi::xml_document	config_file;
	pugi::xml_node* node = &App->LoadConfig(config_file); //todo: make a method to get the root without passing the xml_document
	node = &node->child("entities").child("slime");

	lives = SLIME_MAX_LIVES;

	//read animation from node
	for (pugi::xml_node animations = node->child("animations").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
	{
		p2SString tmp(animations.attribute("name").as_string());

		if (tmp == "slime_right_idle")
			LoadAnimation(animations, &standard_right_idle);
		else if (tmp == "slime_left_idle")
			LoadAnimation(animations, &standard_left_idle);
		else if (tmp == "slime_right_jump")
			LoadAnimation(animations, &standard_right_jump);
		else if (tmp == "slime_left_jump")
			LoadAnimation(animations, &standard_left_jump);

	}

	collider = App->collider->AddCollider({ 0, 0, 16, 16 }, ColliderType::COLLIDER_ENEMY_SLIME, (j1Module*)App->entities);

	animation = &standard_right_jump;

}

void Slime::LoadAnimation(pugi::xml_node animation_node, p2Animation* animation) {
	bool ret = true;

	for (pugi::xml_node frame = animation_node.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
		animation->PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

	animation->speed = animation_node.attribute("speed").as_float();
	animation->loop = animation_node.attribute("loop").as_bool();
}

void Slime::Update(float dt)
{

	SetRadar();

	if (!key_entities_speed && dt > 0)
		SetEntitiesSpeed(dt);

	if (dt > 0) {
		standard_left_idle.speed = standard_left_idle_vel*dt;
		standard_left_jump.speed = standard_left_jump_vel*dt;
		standard_right_idle.speed = standard_right_idle_vel*dt;
		standard_right_jump.speed = standard_right_jump_vel*dt;
	}
	

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

		animation->loop = false;
		

	}
	else if (player_in_radar && !moving) {
		animation->loop = true;

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
			animation->loop = true;
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
		animation->loop = true;
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
		animation->loop = true;
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
			if (!App->entities->GetPlayer()->isDead && !App->entities->GetPlayer()->god_mode)
				player_in_radar = CheckForPlayer();
		}
	}
	/*LOG("SLIME LEFT UP POS x : %i y : %i", slime_pos_UP_LEFT.x, slime_pos_UP_LEFT.y);
	LOG("SLIME DOWN RIGHT POS x : %i y : %i", slime_pos_DOWN_RIGHT.x, slime_pos_DOWN_RIGHT.y);
	LOG("MOV GOAL x : %i goal y : %i", movementGoal.x, movementGoal.y);*/
	currentTime = SDL_GetTicks();
}

uint Slime::getLives()
{
	return lives;
}

void Slime::OnCollision(Collider* collider)
{
	if (collider->type == ColliderType::COLLIDER_PLAYER)
		player_in_radar = false;
	else if (collider->type == ColliderType::COLLIDER_PLAYER_BASIC_SHOT)
		lives--;
	

	if (lives <= 0)
	{
		this->to_destroy = true;
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

	iPoint tmp_player = App->map->WorldToMap(App->entities->GetPlayer()->position.x, App->entities->GetPlayer()->position.y - 1);

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
	standard_left_idle_vel = standard_left_idle.speed;
	standard_right_idle_vel = standard_right_idle.speed;
	standard_right_jump_vel = standard_right_jump.speed;
	standard_left_jump_vel = standard_left_jump.speed;
	key_entities_speed = true;
}

bool Slime::Save(pugi::xml_node& save) const
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