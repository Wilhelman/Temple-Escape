#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Particles.h"
#include "j1Collider.h"
#include "j1Window.h"
#include "p2Log.h"

#include "SDL/include/SDL_timer.h"

j1Particles::j1Particles()
{
	name.create("particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
}

j1Particles::~j1Particles()
{}

// Called before render is available
bool j1Particles::Awake(pugi::xml_node& config)
{
	//TODO read the xml anims

	LOG("Loading Player from config file");
	bool ret = true;

	spritesheetName.create(config.child("spritesheetSource").attribute("name").as_string());

	//set all the animations


	for (pugi::xml_node particle = config.child("spritesheetSource").child("particle"); particle && ret; particle = particle.next_sibling("particle"))
	{
		for (pugi::xml_node animations = config.child("spritesheetSource").child("particle").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
		{
			p2SString particle_type(particle.attribute("name").as_string());
			p2SString animation_type(animations.attribute("name").as_string());

			if (particle_type == "player_basic_shot")
			{
				if (animation_type == "player_basic_shot_right")
				{
					for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
						player_basic_shot_right.anim.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

					player_basic_shot_right.anim.speed = animations.attribute("speed").as_float();
					player_basic_shot_right.anim.loop = animations.attribute("loop").as_bool();
					player_basic_shot_right.speed.x = animations.attribute("abs_speed_x").as_int();
					player_basic_shot_right.speed.y = animations.attribute("abs_speed_y").as_int();
					player_basic_shot_right.life = animations.attribute("life_time").as_uint();
				}
				if (animation_type == "player_basic_shot_left")
				{
					for (pugi::xml_node frame = animations.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
						player_basic_shot_left.anim.PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

					player_basic_shot_left.anim.speed = animations.attribute("speed").as_float();
					player_basic_shot_left.anim.loop = animations.attribute("loop").as_bool();
					player_basic_shot_left.speed.x = animations.attribute("abs_speed_x").as_int();
					player_basic_shot_left.speed.y = animations.attribute("abs_speed_y").as_int();
					player_basic_shot_left.life = animations.attribute("life_time").as_uint();
				}

			}

		}
	}
	return true;
}


// Load assets
bool j1Particles::Start()
{

	LOG("Loading particles");
	graphics = App->tex->Load("Assets/particles/particles.png");
	MotionTree = App->tex->Load("Assets/maps/forest/Motion_trees.png");

	LOG("Loading fx sound to laser particle");
	//basic_shot.fx = App->audio->LoadFx("Assets/audio/effects/valnus_shot_1_2.wav");

	return true;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");
	App->audio->UnLoadFx(player_basic_shot_right.fx);

	//App->tex->Unload(graphics); TODO we need to do this?
	graphics = nullptr;

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
		}
	}

	return true;
}

// Update: draw background
bool j1Particles::Update(float dt)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];

		if (p == nullptr)
			continue;

		if (p->position.y < -10 || p->position.y > App->win->screen_surface->h) {
			delete p;
			active[i] = nullptr;
		}
		else if (p->Update() == false)
		{
			delete p;
			active[i] = nullptr;
		}
		else if (SDL_GetTicks() >= p->born)
		{
			App->render->Blit(graphics, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			if (p->fx_played == false)
			{
				p->fx_played = true;
				App->audio->PlayFx(p->fx);
			}
		}

	}

	return true;
}

void j1Particles::AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type, Uint32 delay)
{
	Particle* p = new Particle(particle);
	p->born = SDL_GetTicks() + delay;
	p->position.x = x;
	p->position.y = y;

	if (collider_type != COLLIDER_NONE) {
		p->collider = App->collider->AddCollider({ p->position.x, p->position.y, p->anim.GetCurrentFrame().w , p->anim.GetCurrentFrame().h }, collider_type, this);
	}

	active[last_particle++] = p;
	if (last_particle == MAX_ACTIVE_PARTICLES)
		last_particle = 0;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed),
	fx(p.fx), born(p.born), life(p.life)
{}

bool Particle::Update()
{
	bool ret = true;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}
	else
		if (anim.Finished())
			ret = false;

	if (!SDL_GetTicks() - born > 0) {
		position.x += speed.x;
		position.y += speed.y;
	}

	if (collider != nullptr)
		collider->SetPos(position.x, position.y);

	return ret;
}

Particle::~Particle() {
	if (collider != nullptr) {
		App->collider->EraseCollider(collider);
		collider = nullptr;
	}
}

void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	/*
	if ((c1->type == COLLIDER_TYPE::COLLIDER_PLAYER_SHOT || c1->type == COLLIDER_TYPE::COLLIDER_PLAYER2_SHOT || c1->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB || c1->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB) && (c2->type == COLLIDER_TYPE::COLLIDER_ENEMY || c2->type == COLLIDER_TYPE::COLLIDER_ENEMY_FLYING))
		App->audio->PlayFx(enemy_hitted);
	if (c1->type == COLLIDER_TYPE::COLLIDER_PLAYER2_BOMB || c1->type == COLLIDER_TYPE::COLLIDER_PLAYER_BOMB)
		return;
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			delete active[i];
			active[i] = nullptr;
			break;
		}
	}*/


}