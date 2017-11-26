#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Particles.h"
#include "Player.h"
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
	fxPlayerBasicShot.create(config.child("fxPlayerBasicShot").attribute("name").as_string());

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
					LoadParticleAnimation(animations, &player_basic_shot_right.anim, &player_basic_shot_right);
				
				if (animation_type == "player_basic_shot_left")
					LoadParticleAnimation(animations, &player_basic_shot_left.anim, &player_basic_shot_left);
			}
		}
	}
	return true;
}


// Load assets
bool j1Particles::Start()
{
	bool ret = true;

	LOG("Loading particles");
	graphics = App->tex->Load(spritesheetName.GetString());
	if (graphics == nullptr)
		ret = false;

	LOG("Loading fx sound to laser particle");

	player_basic_shot_left.fx = player_basic_shot_right.fx = App->audio->LoadFx(fxPlayerBasicShot.GetString());

	return ret;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");
	App->audio->UnLoadFx(player_basic_shot_right.fx);

	App->tex->UnLoad(graphics); //TODO we need to do this?
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

		if (p->position.x < -10 || p->position.x > App->win->screen_surface->w) { //todo is this right?
			delete p;
			active[i] = nullptr;
		}
		else if (p->Update(dt) == false)
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

bool Particle::Update(float dt)
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

		(speed.x < 0) ? position.x += floor(speed.x * dt): position.x += ceil(speed.x * dt);
		position.y += ceil (speed.y * dt);
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
	if ((c1->type == COLLIDER_TYPE::COLLIDER_PLAYER_BASIC_SHOT 
		&& (c2->type == COLLIDER_TYPE::COLLIDER_ENEMY_BAT 
		|| c2->type == COLLIDER_TYPE::COLLIDER_ENEMY_SLIME
		|| c2->type == COLLIDER_TYPE::COLLIDER_GROUND)))
	{
		//App->audio->PlayFx(enemy_hitted);

		for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		{
			if (active[i] != nullptr && active[i]->collider == c1)
			{
				delete active[i];
				active[i] = nullptr;
				break;
			}
		}
	}
}

void j1Particles::LoadParticleAnimation(pugi::xml_node animation_node, p2Animation * animation, Particle* particle)
{
	bool ret = true;

	for (pugi::xml_node frame = animation_node.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
		animation->PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

	animation->speed = animation_node.attribute("speed").as_float();
	animation->loop = animation_node.attribute("loop").as_bool();
	particle->speed.x = animation_node.attribute("speed_x").as_int();
	particle->speed.y = animation_node.attribute("speed_y").as_int();
	particle->life = animation_node.attribute("life_time").as_uint();
}
