#include "j1App.h"
#include "Entity.h"
#include "j1Collider.h"
#include "j1Entities.h"
#include "j1Render.h"

Entity::Entity(int x, int y) : position(x, y)
{}

Entity::~Entity()
{
	if (collider != nullptr) {
		App->collider->EraseCollider(collider);
		collider = nullptr;
	}
}

const Collider* Entity::GetCollider() const
{
	return collider;
}

void Entity::Draw(SDL_Texture* sprites)
{
	SDL_Rect r = animation->GetCurrentFrame();

	if (collider != nullptr)
		collider->SetPos(position.x, position.y );

	if (animation != nullptr) {
		(this->type == PLAYER) ? App->render->Blit(sprites, position.x, position.y - r.h, &(animation->GetCurrentFrame())) :
			App->render->Blit(sprites, position.x, position.y, &(animation->GetCurrentFrame()));
	}
}

void Entity::OnCollision(Collider* collider)
{

}