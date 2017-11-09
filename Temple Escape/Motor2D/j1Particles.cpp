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
	//shot player
	{
		basic_shot.anim.PushBack({ 613, 801, 15, 19 });

		basic_shot.anim.loop = false;
		basic_shot.anim.speed = 0.1f;
		basic_shot.speed.y = -5;
		basic_shot.life = 3000;

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
	App->audio->UnLoadFx(basic_shot.fx);

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