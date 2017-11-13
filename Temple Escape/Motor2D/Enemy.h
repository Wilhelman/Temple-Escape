#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "p2Point.h"
#include "p2Animation.h"

struct SDL_Texture;
struct Collider;

class Enemy
{
protected:
	p2Animation* animation = nullptr;

	Collider* collider = nullptr;
	uint lives;
	uint currentTime;
	uint lastTime;

public:
	fPoint position;
	int type;
	p2Animation standard_left_jump;
	p2Animation standard_right_jump;
	p2Animation standard_right_fly;
	p2Animation standard_left_fly;

public:
	Enemy(int x, int y);
	virtual ~Enemy();

	const Collider* GetCollider() const;

	virtual void Move(float dt) {};
	virtual void Shoot(float dt) {};
	virtual void Draw(SDL_Texture* sprites);
	virtual void OnCollision(Collider* collider);
	virtual uint getLives() { return 0; }
};

#endif // __ENEMY_H__