#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"
#include "p2Animation.h"

struct SDL_Texture;
struct Collider;

class Entity
{
protected:
	p2Animation* animation = nullptr;

	Collider* collider = nullptr;
	uint lives;
	uint currentTime;
	uint lastTime;
	bool key_entities_speed;

public:
	fPoint position;
	int type;

public:
	Entity(int x, int y);
	virtual ~Entity();

	const Collider* GetCollider() const;

	virtual void Update(float dt) {};
	virtual void Shoot(float dt) {};
	virtual void Draw(SDL_Texture* sprites);
	virtual void OnCollision(Collider* collider);
	virtual uint getLives() { return 0; };
	virtual void SetEntitiesSpeed(float dt) {};
};

#endif // __ENTITY_H__