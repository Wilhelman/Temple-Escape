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

	current_state = PlayerState::ST_UNKNOWN;
	
	if (!isDead) 
	{ //MOVEMENT / GRAVITY FUNCTIONALITY
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && !didDoubleJump) 
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

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{
			if (canGoRight()) 
			{
				this->position.x += 2.0f;
			}
			current_state = PlayerState::ST_RUN_RIGHT;
			last_state = PlayerLastState::LAST_ST_RUN_RIGHT;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{
			if (canGoLeft())
				this->position.x -= 2.0f;

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
		if (currentTime <= jumpTimer + 300 && isJumping)
			isGettingHigh = true;

		if (!isGettingHigh) 
		{
			if (float gravity = gravityHaveToWork())
				this->position.y += gravity;
		}

		if (isJumping && isGettingHigh && canGoUp()) 
		{
			if (currentTime <= jumpTimer + 150)
				this->position.y -= 2.2f;
			else
				this->position.y -= 1.5f;
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

	if (c2->type == COLLIDER_ENEMY_BAT && !isDead) {
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



float j1Player::gravityHaveToWork() {

	isJumping = true;

	//variables checking the next 1 pixel
	fPoint tmpPositionLeft;
	tmpPositionLeft.x = (int)pCollider->rect.x + (int)(pCollider->rect.w/4);
	tmpPositionLeft.y = (int)pCollider->rect.y + (int)pCollider->rect.h;
	fPoint tmpPositionRight;
	tmpPositionRight.x = (int)pCollider->rect.x + (int)pCollider->rect.w - (int)(pCollider->rect.w / 4);
	tmpPositionRight.y = (int)pCollider->rect.y + (int)pCollider->rect.h;

	iPoint characterPosInTileWorldLeft = App->map->WorldToMap(tmpPositionLeft.x, tmpPositionLeft.y);
	iPoint characterPosInTileWorldRight = App->map->WorldToMap(tmpPositionRight.x, tmpPositionRight.y);

	//TOUCHING DEAD TILE COLLIDER
	if (App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) == 596 && !isDead) {
		didDoubleJump = isJumping = false;
		left_jump.Reset();
		right_jump.Reset();
		jumpTimer = 0;
		isDead = true;
		App->audio->PlayFx(player_dead);
		deadTime = SDL_GetTicks();
		return 0.0f;
	}

	//TOUCHING THE GROUND COLLIDER TILE
	if (App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) == 43) {
		didDoubleJump = isJumping = false;
		left_jump.Reset();
		right_jump.Reset();
		return 0.0f;
	}
	if (App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y) == 43) {
		didDoubleJump = isJumping = false;
		left_jump.Reset();
		right_jump.Reset();
		return 0.0f;
	}
	//END TOUCHING THE GROUND COLLIDER TILE

	//variables checking the next 2 pixels
	tmpPositionLeft.y = (int)pCollider->rect.y + (int)pCollider->rect.h + 1;
	tmpPositionRight.y = (int)pCollider->rect.y + (int)pCollider->rect.h + 1;

	characterPosInTileWorldLeft = App->map->WorldToMap(tmpPositionLeft.x, tmpPositionLeft.y);
	characterPosInTileWorldRight = App->map->WorldToMap(tmpPositionRight.x, tmpPositionRight.y);

	if (App->map->collisionLayer->Get(characterPosInTileWorldLeft.x, characterPosInTileWorldLeft.y) == 43) {
		return 1.0f;
	}
	if (App->map->collisionLayer->Get(characterPosInTileWorldRight.x, characterPosInTileWorldRight.y + 1) == 43) {
		return 1.0f;
	}

	return 2.0f;
}

bool j1Player::canGoLeft() {

	bool ret = true;

	fPoint tmpPosition;
	tmpPosition.x = (int)pCollider->rect.x - 1;
	tmpPosition.y = (int)pCollider->rect.y + (int)pCollider->rect.h - 1;

	iPoint characterPosInTileWorld = App->map->WorldToMap((int)tmpPosition.x, (int)tmpPosition.y);

	if (App->map->collisionLayer->Get(characterPosInTileWorld.x, characterPosInTileWorld.y) != 0)
		ret = false;

	tmpPosition.x = (int)pCollider->rect.x - 1;
	tmpPosition.y = (int)pCollider->rect.y;

	characterPosInTileWorld = App->map->WorldToMap((int)tmpPosition.x, (int)tmpPosition.y);

	if (App->map->collisionLayer->Get(characterPosInTileWorld.x, characterPosInTileWorld.y) != 0)
		ret = false;

	return ret;
}

bool j1Player::canGoUp() {

	bool ret = true;
	fPoint tmpPosition;
	tmpPosition.x = (int)pCollider->rect.x;
	tmpPosition.y = (int)pCollider->rect.y - 1;

	iPoint characterPosInTileWorld = App->map->WorldToMap((int)tmpPosition.x, (int)tmpPosition.y);

	if (App->map->collisionLayer->Get(characterPosInTileWorld.x, characterPosInTileWorld.y) != 0)
		ret = false;

	tmpPosition.x = (int)pCollider->rect.x + (int)pCollider->rect.w;
	tmpPosition.y = (int)pCollider->rect.y - 1;

	characterPosInTileWorld = App->map->WorldToMap((int)tmpPosition.x, (int)tmpPosition.y);

	if (App->map->collisionLayer->Get(characterPosInTileWorld.x, characterPosInTileWorld.y) != 0)
		ret = false;

	return ret;
}

bool j1Player::canGoRight() {

	bool ret = true;

	fPoint tmpPosition;
	tmpPosition.x = (int)pCollider->rect.x ;
	tmpPosition.y = (int)pCollider->rect.y + (int)pCollider->rect.h / 2;

	iPoint characterPosInTileWorld = App->map->WorldToMap(tmpPosition.x, tmpPosition.y);
	characterPosInTileWorld.x++;

	if (App->map->collisionLayer->Get(characterPosInTileWorld.x, characterPosInTileWorld.y) != 0) {
		characterPosInTileWorld = App->map->MapToWorld(characterPosInTileWorld.x, characterPosInTileWorld.y);

		SDL_Rect tileCollider = { characterPosInTileWorld.x,characterPosInTileWorld.y, App->map->data.tile_width , App->map->data.tile_height };

		SDL_Rect player = { position.x, position.y, pCollider->rect.w, pCollider->rect.h };
		SDL_Rect res = { 0, 0, 0, 0 };

		if (check_collision(player,tileCollider)) {
			ret = false;
		}
	}
	
	return ret;

}

bool j1Player::check_collision(SDL_Rect A, SDL_Rect B)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = A.x;
	rightA = A.x + A.w;
	topA = A.y;
	bottomA = A.y + A.h;

	//Calculate the sides of rect B
	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;
	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return true;
	}

	if (topA >= bottomB)
	{
		return true;
	}

	if (rightA <= leftB)
	{
		return true;
	}

	if (leftA >= rightB)
	{
		return true;
	}

	//If none of the sides from A are outside B
	return false;
}

bool j1Player::Load(pugi::xml_node& load) {
	bool ret = true;

	if (!load.child("position").empty()) {
		tmp.x = load.child("position").attribute("x").as_float();
		tmp.y = load.child("position").attribute("y").as_float() - 2.0f;
	}

	if (App->fadeToBlack->FadeIsOver())
		ImplementLoad();
	
	return ret;
}

void j1Player::ImplementLoad() {
	position.x = tmp.x;
	position.y = tmp.y;
}


bool j1Player::Save(pugi::xml_node& save) const {
	bool ret = true;

	if (save.child("position").empty()) {
		save = save.append_child("position");
		save.append_attribute("x").set_value(position.x);
		save.append_attribute("y").set_value(position.y);
	}
	else {
		save.child("position").attribute("x").set_value(position.x);
		save.child("position").attribute("y").set_value(position.y);
	}

	return ret;
}