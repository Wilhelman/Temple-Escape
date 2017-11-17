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

#include "j1Player.h"

j1Player::j1Player() : j1Module()
{
	name.create("player");
}

// Destructor
j1Player::~j1Player()
{}

// Called before render is available
bool j1Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player from config file");
	bool ret = true;

	spritesheetName.create(config.child("spritesheetSource").attribute("name").as_string());
	fxPlayerJump.create(config.child("fxPlayerJump").attribute("name").as_string());
	fxPlayerDead.create(config.child("fxPlayerDead").attribute("name").as_string());
	
	//set all the animations
	for (pugi::xml_node animations = config.child("spritesheetSource").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
	{	
		p2SString tmp(animations.attribute("name").as_string());
		if (tmp == "right_idle") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				right_idle.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
			
			right_idle.speed = animations.attribute("speed").as_float();
			right_idle.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_idle") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				left_idle.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			left_idle.speed = animations.attribute("speed").as_float();
			left_idle.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_run") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				right_run.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			right_run.speed = animations.attribute("speed").as_float();
			right_run.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_run") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				left_run.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			left_run.speed = animations.attribute("speed").as_float();
			left_run.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_jump") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				left_jump.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			left_jump.speed = animations.attribute("speed").as_float();
			left_jump.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_jump") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				right_jump.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			right_jump.speed = animations.attribute("speed").as_float();
			right_jump.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_dead") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				left_death_blink.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			left_death_blink.speed = animations.attribute("speed").as_float();
			left_death_blink.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_dead") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				right_death_blink.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			right_death_blink.speed = animations.attribute("speed").as_float();
			right_death_blink.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "right_shoot") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				right_shoot.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });
			
			right_shoot.speed = animations.attribute("speed").as_float();
			right_shoot.loop = animations.attribute("loop").as_bool();
		}
		if (tmp == "left_shoot") {

			for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
				left_shoot.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

			left_shoot.speed = animations.attribute("speed").as_float();
			left_shoot.loop = animations.attribute("loop").as_bool();
		}
	}

	current_animation = &right_idle;

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{
	bool ret = true;
	jumpTimer = deadTime = 0;
	isJumping = didDoubleJump = reachedEnd = isDead = false;
	current_state = PlayerState::ST_IDLE_RIGHT;
	last_state = PlayerLastState::LAST_ST_RUN_RIGHT;

	LOG("Loading player textures");

	graphics = App->tex->Load(spritesheetName.GetString());
	if (graphics == nullptr)
		ret = false;

	iPoint spawnPos = App->map->spawn;
	this->position.x = spawnPos.x;
	this->position.y = spawnPos.y;

	LOG("Creating player collider");
	pCollider = App->collider->AddCollider({ (int)position.x,(int)position.y,39,21 }, COLLIDER_PLAYER, this);

	LOG("Loading player audios");
	player_jump = App->audio->LoadFx(fxPlayerJump.GetString());
	player_dead = App->audio->LoadFx(fxPlayerDead.GetString());

	return ret;
}

// Called each loop iteration
bool j1Player::PreUpdate()
{
	bool ret = true;
	
	return ret;
}

// Called each loop iteration
bool j1Player::Update(float dt)
{
	bool ret = true;

	current_dt = dt;

	right_idle.speed = 5 * dt;
	left_idle.speed = 5 * dt;
	right_run.speed = 5 * dt;;
	left_run.speed = 5 * dt;;
	right_jump.speed = 5 * dt;
	left_jump.speed = 5 * dt;
	right_death_blink.speed = 5 * dt;
	left_death_blink.speed = 5 * dt;
	right_shoot.speed = 5 * dt;
	left_shoot.speed = 5 * dt;

	current_state = PlayerState::ST_UNKNOWN;
	
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
	case j1Player::ST_UNKNOWN:
		switch (last_state)
		{
		case j1Player::LAST_ST_UNKNOWN:
			break;
		case j1Player::LAST_ST_IDLE_RIGHT:
			break;
		case j1Player::LAST_ST_IDLE_LEFT:
			break;
		case j1Player::LAST_ST_RUN_RIGHT:
			if(isJumping)
				current_animation = &right_jump;
			else
				current_animation = &right_idle;
			break;
		case j1Player::LAST_ST_RUN_LEFT:
			if (isJumping)
				current_animation = &left_jump;
			else
				current_animation = &left_idle;
			break;
		case j1Player::LAST_ST_SHOOT_RIGHT:
			if (isJumping)
				current_animation = &right_jump;
			else
				current_animation = &right_idle;
			break;
		case j1Player::LAST_ST_SHOOT_LEFT:
			if (isJumping)
				current_animation = &left_jump;
			else
				current_animation = &left_idle;
			break;
		default:
			break;
		}
		break;
	case j1Player::ST_IDLE_RIGHT:
		break;
	case j1Player::ST_IDLE_LEFT:
		break;
	case j1Player::ST_RUN_RIGHT:
		if (isJumping)
			current_animation = &right_jump;
		else 
			current_animation = &right_run;
		break;
	case j1Player::ST_RUN_LEFT:
		if (isJumping)
			current_animation = &left_jump;
		else 
			current_animation = &left_run;
		break;
	case j1Player::ST_SHOOT_RIGHT:
		if (isJumping)
			current_animation = &right_jump;
		else 
			current_animation = &right_shoot;
		break;
	case j1Player::ST_SHOOT_LEFT:
		if (isJumping)
			current_animation = &left_jump;
		else 
			current_animation = &left_shoot;
		break;
	default:
		break;
	}

	//DEAD ANIMATION WITH TIMER
	if (isDead && currentTime < deadTime + 1000) 
	{
		if (last_state == LAST_ST_RUN_RIGHT)
			current_animation = &right_death_blink;
		else
			current_animation = &left_death_blink;
	}
	else if (isDead)
	{
		isDead = false;
		pCollider->type = COLLIDER_PLAYER;
		iPoint spawnPos = App->map->spawn;
		this->position.x = spawnPos.x;
		this->position.y = spawnPos.y;
	}

	// Draw everything --------------------------------------
	SDL_Rect r = current_animation->GetCurrentFrame();

	// Update player collider
	pCollider->SetPos(position.x, position.y - r.h);
	pCollider->rect.w = r.w;
	pCollider->rect.h = r.h;

	if (!App->render->Blit(graphics, (int)position.x, (int)position.y - r.h, &r)) {
		LOG("Cannot blit the texture in j1Player %s\n", SDL_GetError());
		ret = false;
	}

	currentTime = SDL_GetTicks();
	
	return ret;
}

// Called each loop iteration
bool j1Player::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1Player::CleanUp()
{
	LOG("Freeing the player");

	bool ret = true;

	if (pCollider != nullptr) {
		App->collider->EraseCollider(pCollider);
		pCollider = nullptr;
	}
	App->tex->UnLoad(graphics);

	LOG("Unloading player sound fx");
	App->audio->UnLoadFx(player_jump);
	App->audio->UnLoadFx(player_dead);

	return ret;
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {

	if ((c2->type == COLLIDER_ENEMY_BAT || c2->type == COLLIDER_ENEMY_SLIME) && !isDead) {
		isDead = true;
		App->audio->PlayFx(player_dead);
		deadTime = SDL_GetTicks();
	}


	if (c2->type == COLLIDER_LVL_END)
	{
		if(!reachedEnd)
			reachedEnd = true;
	}
}



float j1Player::gravityHaveToWork() 
{

	fPoint tmpPosLeft;
	tmpPosLeft.x = position.x;
	tmpPosLeft.y = position.y - 1;

	iPoint characterPosInTileWorldLeft = App->map->WorldToMap(tmpPosLeft.x, tmpPosLeft.y);

	fPoint tmpPosRight;
	tmpPosRight.x = position.x + pCollider->rect.w - 1;
	tmpPosRight.y = position.y - 1;

	iPoint characterPosInTileWorldRight = App->map->WorldToMap(tmpPosRight.x, tmpPosRight.y);

	characterPosInTileWorldLeft.y++;
	characterPosInTileWorldRight.y++;

	if ((App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) == 596 || App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y) == 596) && !isDead) {
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

		SDL_Rect player = { position.x, position.y - pCollider->rect.h, pCollider->rect.w, pCollider->rect.h };

		float distance_to_wall = DistanceToWall(tileColliderDown, player, DOWN);
		distance_to_wall *= -1;
		if (distance_to_wall == 0.0f)
			return 0.0f;
		else if (distance_to_wall >= ceil(GRAVITY * current_dt))
			return ceil(GRAVITY*current_dt);
		else if (distance_to_wall < ceil(GRAVITY*current_dt)) 
			return distance_to_wall;
	}

	if (App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) == 43 || App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y) == 43) {
		didDoubleJump = isJumping = false;
		left_jump.Reset();
		right_jump.Reset();

		characterPosInTileWorldLeft = App->map->MapToWorld(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y);
		characterPosInTileWorldRight = App->map->MapToWorld(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y);
		SDL_Rect tileColliderUp = { characterPosInTileWorldLeft.x,characterPosInTileWorldLeft.y, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect tileColliderDown = { characterPosInTileWorldRight.x,characterPosInTileWorldRight.y, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect player = { position.x, position.y - pCollider->rect.h, pCollider->rect.w, pCollider->rect.h };
		SDL_Rect res = { 0, 0, 0, 0 };

		float distance_to_wall = DistanceToWall(tileColliderDown, player, DOWN);
		distance_to_wall *= -1;
		
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

float j1Player::canGoLeft() 
{
	bool ret = true;

	fPoint tmpPosUp;
	tmpPosUp.x = position.x;
	tmpPosUp.y = position.y - pCollider->rect.h;

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

		SDL_Rect player = { position.x, position.y - pCollider->rect.h, pCollider->rect.w, pCollider->rect.h };
		

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

float j1Player::canGoUp() 
{
	bool ret = true;

	fPoint tmpPosLeft;
	tmpPosLeft.x = position.x;
	tmpPosLeft.y = position.y - pCollider->rect.h;

	iPoint characterPosInTileWorldLeft = App->map->WorldToMap(tmpPosLeft.x, tmpPosLeft.y);

	fPoint tmpPosRight;
	tmpPosRight.x = position.x + pCollider->rect.w - 1;
	tmpPosRight.y = position.y - pCollider->rect.h;

	iPoint characterPosInTileWorldRight = App->map->WorldToMap(tmpPosRight.x, tmpPosRight.y);

	characterPosInTileWorldLeft.y--;
	characterPosInTileWorldRight.y--;

	if (App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) != 0 || App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y) != 0) {

		//isJumping = false;

		characterPosInTileWorldRight = App->map->MapToWorld(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y);

		SDL_Rect tileColliderDown = { characterPosInTileWorldRight.x,characterPosInTileWorldRight.y + App->map->data.tile_height, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect player = { position.x, position.y - pCollider->rect.h, pCollider->rect.w, pCollider->rect.h };

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

float j1Player::canGoRight() 
{
	fPoint tmpPosUp;
	tmpPosUp.x = position.x + pCollider->rect.w - 1;
	tmpPosUp.y = position.y - pCollider->rect.h;

	iPoint characterPosInTileWorldUp = App->map->WorldToMap(tmpPosUp.x, tmpPosUp.y);

	fPoint tmpPosDown;
	tmpPosDown.x = position.x + pCollider->rect.w - 1;
	tmpPosDown.y = position.y - 1;

	iPoint characterPosInTileWorldDown = App->map->WorldToMap(tmpPosDown.x, tmpPosDown.y);

	characterPosInTileWorldUp.x++;
	characterPosInTileWorldDown.x++;
	if (App->map->collisionLayer->Get(characterPosInTileWorldUp.x, characterPosInTileWorldUp.y) != 0 || App->map->collisionLayer->Get(characterPosInTileWorldDown.x, characterPosInTileWorldDown.y) != 0) {

		characterPosInTileWorldDown = App->map->MapToWorld(characterPosInTileWorldDown.x, characterPosInTileWorldDown.y);
		SDL_Rect tileColliderDown = { characterPosInTileWorldDown.x,characterPosInTileWorldDown.y, App->map->data.tile_width , App->map->data.tile_height };
		SDL_Rect player = { position.x, position.y - pCollider->rect.h, pCollider->rect.w, pCollider->rect.h };

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

float j1Player::DistanceToWall(SDL_Rect wall, SDL_Rect player, Direction direction)
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

bool j1Player::Load(pugi::xml_node& load) 
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

void j1Player::ImplementLoad() 
{
	position.x = tmp.x;
	position.y = tmp.y;
}


bool j1Player::Save(pugi::xml_node& save) const 
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
