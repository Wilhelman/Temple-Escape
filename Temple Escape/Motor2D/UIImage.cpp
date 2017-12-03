#include "j1App.h"
#include "UIImage.h"
#include "p2Log.h"


UIImage::UIImage(int x, int y, UI_Type type, SDL_Rect rect, j1Module* callback, UIElement* parent) : UIElement(x, y, type, parent)
{

	this->callback = callback;

	img_normal = rect;
	current_rect = rect;

	LOG("UIImage created in x:%i, y:%i", x, y);
}

void UIImage::UpdateImageWithCoords(SDL_Rect rect) 
{
	current_rect = rect;
}