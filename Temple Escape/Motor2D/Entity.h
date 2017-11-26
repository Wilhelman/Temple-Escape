#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"
#include "p2Animation.h"

struct SDL_Texture;
struct Collider;

class Entity
{
public:
	fPoint position = fPoint(0, 0);
	int type = 0;
	bool to_destroy = false;

protected:
	p2Animation* animation = nullptr;
	Collider* collider = nullptr;
	uint lives = 0u;
	uint currentTime = 0u;
	uint lastTime = 0u;
	bool key_entities_speed = false;

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

	virtual void LoadAnimation(pugi::xml_node animation_node, p2Animation* animation) {};
	
	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}
};

#endif // __ENTITY_H__