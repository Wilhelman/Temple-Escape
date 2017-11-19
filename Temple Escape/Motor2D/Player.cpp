#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collider.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Particles.h"
#include "j1Entities.h"

#include "Player.h"


// Called before the first frame
Player::Player(int x, int y) : Entity(x, y)
{
	bool ret = true;
	jumpTimer = deadTime = 0;
	isJumping = didDoubleJump = reachedEnd = isDead = god_mode = false;
	current_state = PlayerState::ST_IDLE_RIGHT;
	last_state = PlayerLastState::LAST_ST_RUN_RIGHT;

	LOG("Creating player collider");
	collider = App->collider->AddCollider({ 0, 0, 16, 12 }, COLLIDER_TYPE::COLLIDER_PLAYER, (j1Module*)App->entities);

	iPoint spawnPos = App->map->spawn;
	this->position.x = spawnPos.x;
	this->position.y = spawnPos.y;

	animation = &right_idle;
}

Player::~Player()
{
	LOG("Freeing the player");
}

// Called each loop iteration
void Player::Update(float dt)
{

	if (!key_entities_speed && dt > 0)
		SetEntitiesSpeed(dt);

	if (dt > 0) {
		right_idle.speed = right_idle_vel * dt;
		left_idle.speed = left_idle_vel * dt;
		right_run.speed = right_run_vel * dt;;
		left_run.speed = left_run_vel * dt;;
		right_jump.speed = right_jump_vel * dt;
		left_jump.speed = left_jump_vel * dt;
		right_death_blink.speed = right_death_blink_vel * dt;
		left_death_blink.speed = left_death_blink_vel * dt;
		right_shoot.speed = right_shoot_vel * dt;
		left_shoot.speed = left_shoot_vel * dt;
	}

	current_dt = dt;

	current_state = PlayerState::ST_UNKNOWN;

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !isDead) {
		god_mode = !god_mode;
	}

	if (!isDead)
	{ //MOVEMENT / GRAVITY FUNCTIONALITY
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && !didDoubleJump
			&& App->input->GetKey(SDL_SCANCODE_SPACE) != KEY_REPEAT)
		{
			if (!isJumping)
			{
				App->audio->PlayFx(player_jump);
				isJumping = true;
			}
			else
			{
				App->audio->PlayFx(player_jump);
				didDoubleJump = true;
				left_jump.Reset();
				right_jump.Reset();
			}
			jumpTimer = SDL_GetTicks();
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT
			&& App->input->GetKey(SDL_SCANCODE_SPACE) != KEY_REPEAT)
		{

			this->position.x += canGoRight();

			current_state = PlayerState::ST_RUN_RIGHT;
			last_state = PlayerLastState::LAST_ST_RUN_RIGHT;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT
			&& App->input->GetKey(SDL_SCANCODE_SPACE) != KEY_REPEAT)
		{
			this->position.x -= canGoLeft();

			current_state = PlayerState::ST_RUN_LEFT;
			last_state = PlayerLastState::LAST_ST_RUN_LEFT;
		}

		if ((App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN
			|| App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
			&& (last_state == PlayerLastState::LAST_ST_RUN_RIGHT
				|| last_state == PlayerLastState::LAST_ST_IDLE_RIGHT
				|| last_state == PlayerLastState::LAST_ST_SHOOT_RIGHT)
			&& !isJumping)
		{
			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
				App->particles->AddParticle(App->particles->player_basic_shot_right, position.x + 5, position.y - 8, COLLIDER_PLAYER_BASIC_SHOT);
			current_state = PlayerState::ST_SHOOT_RIGHT;
			last_state = PlayerLastState::LAST_ST_SHOOT_RIGHT;
		}

		if ((App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN
			|| App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
			&& (last_state == PlayerLastState::LAST_ST_RUN_LEFT
				|| last_state == PlayerLastState::LAST_ST_IDLE_LEFT
				|| last_state == PlayerLastState::LAST_ST_SHOOT_LEFT)
			&& !isJumping)
		{
			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
				App->particles->AddParticle(App->particles->player_basic_shot_left, position.x + 5, position.y - 8, COLLIDER_PLAYER_BASIC_SHOT);
			current_state = PlayerState::ST_SHOOT_LEFT;
			last_state = PlayerLastState::LAST_ST_SHOOT_LEFT;
		}

		bool isGettingHigh = false;
		if (currentTime <= jumpTimer + 500 && isJumping)
			isGettingHigh = true;

		if (!isGettingHigh)
		{
			if (float gravity = gravityHaveToWork()) {
				this->position.y += gravity;
				isJumping = true;
			}

		}

		if (isJumping && isGettingHigh) {
			this->position.y -= canGoUp();
		}


	}

	//SEARCH THE STATE AND SET THE ANIMATION
	switch (current_state)
	{
	case Player::ST_UNKNOWN:
		switch (last_state)
		{
		case Player::LAST_ST_UNKNOWN:
			break;
		case Player::LAST_ST_IDLE_RIGHT:
			break;
		case Player::LAST_ST_IDLE_LEFT:
			break;
		case Player::LAST_ST_RUN_RIGHT:
			if (isJumping)
				animation = &right_jump;
			else
				animation = &right_idle;
			break;
		case Player::LAST_ST_RUN_LEFT:
			if (isJumping)
				animation = &left_jump;
			else
				animation = &left_idle;
			break;
		case Player::LAST_ST_SHOOT_RIGHT:
			if (isJumping)
				animation = &right_jump;
			else
				animation = &right_idle;
			break;
		case Player::LAST_ST_SHOOT_LEFT:
			if (isJumping)
				animation = &left_jump;
			else
				animation = &left_idle;
			break;
		default:
			break;
		}
		break;
	case Player::ST_IDLE_RIGHT:
		break;
	case Player::ST_IDLE_LEFT:
		break;
	case Player::ST_RUN_RIGHT:
		if (isJumping)
			animation = &right_jump;
		else
			animation = &right_run;
		break;
	case Player::ST_RUN_LEFT:
		if (isJumping)
			animation = &left_jump;
		else
			animation = &left_run;
		break;
	case Player::ST_SHOOT_RIGHT:
		if (isJumping)
			animation = &right_jump;
		else
			animation = &right_shoot;
		break;
	case Player::ST_SHOOT_LEFT:
		if (isJumping)
			animation = &left_jump;
		else
			animation = &left_shoot;
		break;
	default:
		break;
	}

	//DEAD ANIMATION WITH TIMER
	if (isDead && currentTime < deadTime + 1000)
	{
		if (last_state == LAST_ST_RUN_RIGHT)
			animation = &right_death_blink;
		else
			animation = &left_death_blink;
	}
	else if (isDead)
	{
		isDead = false;
		collider->type = COLLIDER_PLAYER;
		iPoint spawnPos = App->map->spawn;
		this->position.x = spawnPos.x;
		this->position.y = spawnPos.y;
	}

	// Draw everything --------------------------------------
	SDL_Rect r = animation->GetCurrentFrame();

	// Update player collider
	collider->SetPos(position.x, position.y - r.h);
	collider->rect.w = r.w;
	collider->rect.h = r.h;
	/*
	if (!App->render->Blit(graphics, (int)position.x, (int)position.y - r.h, &r)) {
		LOG("Cannot blit the texture in j1Player %s\n", SDL_GetError());
	}
	*/
	currentTime = SDL_GetTicks();
}

void Player::OnCollision(Collider* c1, Collider* c2) {

	if (((c2->type == COLLIDER_ENEMY_BAT || c2->type == COLLIDER_ENEMY_SLIME) && !isDead) && !god_mode) {
		isDead = true;
		App->audio->PlayFx(player_dead);
		deadTime = SDL_GetTicks();
	}

	if (c2->type == COLLIDER_LVL_END)
	{
		if (!reachedEnd)
			reachedEnd = true;
	}
}

float Player::gravityHaveToWork()
{

	fPoint tmpPosLeft;
	tmpPosLeft.x = position.x;
	tmpPosLeft.y = position.y - 1;

	iPoint characterPosInTileWorldLeft = App->map->WorldToMap(tmpPosLeft.x, tmpPosLeft.y);

	fPoint tmpPosRight;
	tmpPosRight.x = position.x + collider->rect.w - 1;
	tmpPosRight.y = position.y - 1;

	iPoint characterPosInTileWorldRight = App->map->WorldToMap(tmpPosRight.x, tmpPosRight.y);

	characterPosInTileWorldLeft.y++;
	characterPosInTileWorldRight.y++;

	if ((App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) == 596 || App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y) == 596) && !isDead) {
		if (god_mode) {
			position.x = App->map->MapToWorld(last_save_position.x, last_save_position.y).x;
			position.y = App->map->MapToWorld(last_save_position.x, last_save_position.y).y;
		}
		else {
			didDoubleJump = isJumping = false;
			left_jump.Reset();
			right_jump.Reset();
			jumpTimer = 0;
			isDead = true;
			App->audio->PlayFx(player_dead);
			deadTime = SDL_GetTicks();


			characterPosInTileWorldLeft = App->map->MapToWorld(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y);
			characterPosInTileWorldRight = App->map->MapToWorld(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y);
			SDL_Rect tileColliderUp = { characterPosInTileWorldLeft.x,characterPosInTileWorldLeft.y, App->map->data.tile_width , App->map->data.tile_height };

			SDL_Rect tileColliderDown = { characterPosInTileWorldRight.x,characterPosInTileWorldRight.y, App->map->data.tile_width , App->map->data.tile_height };

			SDL_Rect player = { position.x, position.y - collider->rect.h, collider->rect.w, collider->rect.h };

			float distance_to_wall = DistanceToWall(tileColliderDown, player, DOWN);
			distance_to_wall *= -1;
			if (distance_to_wall == 0.0f)
				return 0.0f;
			else if (distance_to_wall >= ceil(GRAVITY * current_dt))
				return ceil(GRAVITY*current_dt);
			else if (distance_to_wall < ceil(GRAVITY*current_dt))
				return distance_to_wall;
		}
	}

	if (App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) == 43 || App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y) == 43) {
		didDoubleJump = isJumping = false;
		left_jump.Reset();
		right_jump.Reset();

		characterPosInTileWorldLeft = App->map->MapToWorld(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y);
		characterPosInTileWorldRight = App->map->MapToWorld(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y);
		SDL_Rect tileColliderUp = { characterPosInTileWorldLeft.x,characterPosInTileWorldLeft.y, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect tileColliderDown = { characterPosInTileWorldRight.x,characterPosInTileWorldRight.y, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect player = { position.x, position.y - collider->rect.h, collider->rect.w, collider->rect.h };
		SDL_Rect res = { 0, 0, 0, 0 };

		float distance_to_wall = DistanceToWall(tileColliderDown, player, DOWN);
		distance_to_wall *= -1;

		fPoint tmpPosL;
		tmpPosL.x = position.x;
		tmpPosL.y = position.y - 1;
		iPoint characterLeft = App->map->WorldToMap(tmpPosL.x, tmpPosL.y);
		fPoint tmpPosR;
		tmpPosR.x = position.x + collider->rect.w - 1;
		tmpPosR.y = position.y - 1;

		(App->map->collisionLayer->Get(characterLeft.x, characterLeft.y + 1) == 43) ?
			last_save_position = App->map->WorldToMap(tmpPosL.x, tmpPosL.y)
			:
			last_save_position = App->map->WorldToMap(tmpPosR.x, tmpPosR.y);


		if (distance_to_wall == 0.0f)
			return 0.0f;
		else if (distance_to_wall >= ceil(GRAVITY * current_dt))
			return ceil(GRAVITY*current_dt);
		else if (distance_to_wall < ceil(GRAVITY*current_dt)) {
			return distance_to_wall;
		}
	}

	return ceil(GRAVITY*current_dt);
}

float Player::canGoLeft()
{
	bool ret = true;

	fPoint tmpPosUp;
	tmpPosUp.x = position.x;
	tmpPosUp.y = position.y - collider->rect.h;

	iPoint characterPosInTileWorldUp = App->map->WorldToMap(tmpPosUp.x, tmpPosUp.y);

	fPoint tmpPosDown;
	tmpPosDown.x = position.x;
	tmpPosDown.y = position.y - 1;

	iPoint characterPosInTileWorldDown = App->map->WorldToMap(tmpPosDown.x, tmpPosDown.y);

	characterPosInTileWorldUp.x--;
	characterPosInTileWorldDown.x--;
	if (App->map->collisionLayer->Get(characterPosInTileWorldUp.x, characterPosInTileWorldUp.y) != 0 || App->map->collisionLayer->Get(characterPosInTileWorldDown.x, characterPosInTileWorldDown.y) != 0) {

		//characterPosInTileWorldUp = App->map->MapToWorld(characterPosInTileWorldUp.x, characterPosInTileWorldUp.y);
		characterPosInTileWorldDown = App->map->MapToWorld(characterPosInTileWorldDown.x, characterPosInTileWorldDown.y);
		//SDL_Rect tileColliderUp = { characterPosInTileWorldUp.x,characterPosInTileWorldUp.y, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect tileColliderDown = { characterPosInTileWorldDown.x,characterPosInTileWorldDown.y, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect player = { position.x, position.y - collider->rect.h, collider->rect.w, collider->rect.h };


		float distance_to_wall = DistanceToWall(tileColliderDown, player, LEFT);
		distance_to_wall *= -1;
		if (distance_to_wall == 0.0f)
			return 0.0f;
		else if (distance_to_wall >= ceil(PLAYER_SPEED * current_dt))
			return ceil(PLAYER_SPEED*current_dt);
		else if (distance_to_wall < ceil(PLAYER_SPEED*current_dt))
			return distance_to_wall;

	}

	return ceil(PLAYER_SPEED*current_dt);
}

float Player::canGoUp()
{
	bool ret = true;

	fPoint tmpPosLeft;
	tmpPosLeft.x = position.x;
	tmpPosLeft.y = position.y - collider->rect.h;

	iPoint characterPosInTileWorldLeft = App->map->WorldToMap(tmpPosLeft.x, tmpPosLeft.y);

	fPoint tmpPosRight;
	tmpPosRight.x = position.x + collider->rect.w - 1;
	tmpPosRight.y = position.y - collider->rect.h;

	iPoint characterPosInTileWorldRight = App->map->WorldToMap(tmpPosRight.x, tmpPosRight.y);

	characterPosInTileWorldLeft.y--;
	characterPosInTileWorldRight.y--;

	if (App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) != 0 || App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y) != 0) {

		//isJumping = false;

		characterPosInTileWorldRight = App->map->MapToWorld(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y);

		SDL_Rect tileColliderDown = { characterPosInTileWorldRight.x,characterPosInTileWorldRight.y + App->map->data.tile_height, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect player = { position.x, position.y - collider->rect.h, collider->rect.w, collider->rect.h };

		float distance_to_wall = DistanceToWall(tileColliderDown, player, UP);
		//distance_to_wall *= -1;
		LOG("DISTANCE TO WALL %f", distance_to_wall);
		if (distance_to_wall == 0.0f) {
			isJumping = false;
			jumpTimer = 0;
			return 0.0f;
		}
		else if (distance_to_wall >= ceil(JUMP_SPEED * current_dt))
			return ceil(JUMP_SPEED*current_dt);
		else if (distance_to_wall < ceil(JUMP_SPEED*current_dt)) {
			LOG("VEL TO RETURN %f - distance to wall: %f", ceil(JUMP_SPEED*current_dt), distance_to_wall);
			return distance_to_wall;
		}
	}
	return ceil(JUMP_SPEED*current_dt);
}

float Player::canGoRight()
{
	fPoint tmpPosUp;
	tmpPosUp.x = position.x + collider->rect.w - 1;
	tmpPosUp.y = position.y - collider->rect.h;

	iPoint characterPosInTileWorldUp = App->map->WorldToMap(tmpPosUp.x, tmpPosUp.y);

	fPoint tmpPosDown;
	tmpPosDown.x = position.x + collider->rect.w - 1;
	tmpPosDown.y = position.y - 1;

	iPoint characterPosInTileWorldDown = App->map->WorldToMap(tmpPosDown.x, tmpPosDown.y);

	characterPosInTileWorldUp.x++;
	characterPosInTileWorldDown.x++;
	if (App->map->collisionLayer->Get(characterPosInTileWorldUp.x, characterPosInTileWorldUp.y) != 0 || App->map->collisionLayer->Get(characterPosInTileWorldDown.x, characterPosInTileWorldDown.y) != 0) {

		characterPosInTileWorldDown = App->map->MapToWorld(characterPosInTileWorldDown.x, characterPosInTileWorldDown.y);
		SDL_Rect tileColliderDown = { characterPosInTileWorldDown.x,characterPosInTileWorldDown.y, App->map->data.tile_width , App->map->data.tile_height };
		SDL_Rect player = { position.x, position.y - collider->rect.h, collider->rect.w, collider->rect.h };

		float distance_to_wall = DistanceToWall(tileColliderDown, player, RIGHT);
		distance_to_wall *= -1;
		if (distance_to_wall == 0.0f)
			return 0.0f;
		else if (distance_to_wall >= ceil(PLAYER_SPEED * current_dt))
			return ceil(PLAYER_SPEED*current_dt);
		else if (distance_to_wall < ceil(PLAYER_SPEED*current_dt))
			return distance_to_wall;

	}

	return ceil(PLAYER_SPEED*current_dt);
}

float Player::DistanceToWall(SDL_Rect wall, SDL_Rect player, Direction direction)
{
	switch (direction)
	{
	case Direction::RIGHT:
		return player.x + player.w - wall.x;
		break;
	case Direction::LEFT:
		return wall.x + wall.w - player.x;
		break;
	case Direction::UP:
		return player.y - wall.y;
		break;
	case Direction::DOWN:
		return player.y + player.h - wall.y;
		break;
	case Direction::NO_DIR:
	default:
		LOG("YOU ARE DOING IT WRONG!");
		break;
	}
}

bool Player::Load(pugi::xml_node& load)
{
	bool ret = true;

	if (!load.child("position").empty())
	{
		tmp.x = load.child("position").attribute("x").as_float();
		tmp.y = load.child("position").attribute("y").as_float() - 2.0f;
	}

	if (App->fadeToBlack->FadeIsOver())
		ImplementLoad();

	return ret;
}

void Player::ImplementLoad()
{
	position.x = tmp.x;
	position.y = tmp.y;
}


bool Player::Save(pugi::xml_node& save) const
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

void Player::SetEntitiesSpeed(float dt) {
	right_idle_vel = right_idle.speed;
	left_idle_vel = left_idle.speed;
	right_jump_vel = right_jump.speed;
	left_jump_vel = left_jump.speed;
	right_run_vel = right_run.speed;
	left_run_vel = left_run.speed;
	right_death_blink_vel= right_death_blink.speed;
	left_death_blink_vel = left_death_blink.speed;
	key_entities_speed = true;
}