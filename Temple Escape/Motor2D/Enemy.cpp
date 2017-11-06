#include "j1App.h"
#include "Enemy.h"
#include "j1Collider.h"
#include "j1Enemies.h"
#include "j1Render.h"

Enemy::Enemy(int x, int y) : position(x, y)
{}

Enemy::~Enemy()
{
	if (collider != nullptr) {
		App->collider->EraseCollider(collider);
		collider = nullptr;
	}
}

const Collider* Enemy::GetCollider() const
{
	return collider;
}

void Enemy::Draw(SDL_Texture* sprites)
{
	if (App->enemies->bossDestroyed) {
		return;
	}
	if (collider != nullptr)
		collider->SetPos(position.x, position.y);

	if (animation != nullptr)
		App->render->Blit(sprites, position.x, position.y, &(animation->GetCurrentFrame()));
}

void Enemy::OnCollision(Collider* collider) 
{

}