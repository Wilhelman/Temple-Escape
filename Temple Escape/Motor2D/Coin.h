#ifndef __Coin_H__
#define __Coin_H__

#include "Entity.h"
#include "p2Animation.h"
#include "p2DynArray.h"

class Coin : public Entity
{
private:
	iPoint original_pos = iPoint(0, 0);

	//animations
	p2Animation idle = p2Animation();

	//animations speeds
	uint idle_vel = 0u;

public:

	Coin(int x, int y);
	void OnCollision(Collider* collider);
	void Update(float dt);

	bool Save(pugi::xml_node&) const;
	void LoadAnimation(pugi::xml_node animation_node, p2Animation* animation);
	void SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position);
	void SetEntitiesSpeed(float dt);

};

#endif