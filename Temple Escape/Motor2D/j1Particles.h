#ifndef __MODULEPARTICLES_H__
#define __MODULEPARTICLES_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Point.h"
#include "j1Collider.h"

#define MAX_ACTIVE_PARTICLES 2000
#define SHOOT_SPEED 60

struct SDL_Texture;
struct Collider;
enum ColliderType;

struct Particle
{
	Collider* collider = nullptr;
	p2Animation anim;
	uint fx = 0;
	iPoint position;
	iPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	bool fx_played = false;

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update(float dt);
};

class j1Particles : public j1Module
{
public:
	j1Particles();
	~j1Particles();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	void AddParticle(const Particle& particle, int x, int y, ColliderType collider_type = COLLIDER_NONE, Uint32 delay = 0);

	void OnCollision(Collider* c1, Collider* c2);

private:
	SDL_Texture* MotionTree = nullptr;
	SDL_Texture* graphics = nullptr;
	Particle* active[MAX_ACTIVE_PARTICLES];
	uint last_particle = 0;

	p2SString spritesheetName;
	p2SString fxPlayerBasicShot;

private:
	void LoadParticleAnimation(pugi::xml_node animation_node, p2Animation* animation, Particle* particle);

public:

	//Player shots
	Particle player_basic_shot_right;
	Particle player_basic_shot_left;

};

#endif // __MODULEPARTICLES_H__