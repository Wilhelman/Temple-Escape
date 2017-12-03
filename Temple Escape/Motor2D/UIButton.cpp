#include "j1App.h"
#include "UIButton.h"
#include "p2Log.h"


UIButton::UIButton(int x, int y, UI_Type type, SDL_Rect normal_rect, SDL_Rect focused_rect, SDL_Rect pressed_rect, j1Module* callback, UIElement* parent) : UIElement(x, y, type, parent)
{
	this->callback = callback;

	btn_normal = normal_rect;
	btn_focused = focused_rect;
	btn_pressed = pressed_rect;
	current_rect = normal_rect;

	LOG("UIButton created in x:%i, y:%i", x, y);
}

void UIButton::UpdateButtonWithSelfRect(SDL_Rect self_rect)
{
	current_rect = self_rect;
}