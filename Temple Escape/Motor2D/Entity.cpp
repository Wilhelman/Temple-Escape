#include "j1App.h"
#include "Entity.h"
#include "j1Collider.h"
#include "j1Entities.h"
#include "j1Render.h"
#include "j1Scene.h"

Entity::Entity(int x, int y) : position(x, y)
{
}

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
		(this->type == PLAYER) ? collider->SetPos(position.x, position.y - r.h) :
		collider->SetPos(position.x, position.y);

	//todo: this should be done setting to 0 the dt in draw's update ... no time to do it the right way!
	if (animation != nullptr && !App->scene->IsGamePaused()) 
		(this->type == PLAYER) ? App->render->Blit(sprites, position.x, position.y - r.h, &(animation->GetCurrentFrame())) :
			App->render->Blit(sprites, position.x, position.y, &(animation->GetCurrentFrame()));
	else if (animation != nullptr && App->scene->IsGamePaused()) {
		(this->type == PLAYER) ? App->render->Blit(sprites, position.x, position.y - r.h, &(animation->frames[0])) :
			App->render->Blit(sprites, position.x, position.y, &(animation->frames[0]));
	}
}

void Entity::OnCollision(Collider* collider)
{

}