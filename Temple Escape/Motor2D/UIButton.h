#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#include "j1Fonts.h"
#include "UIElement.h"
#include "UILabel.h"

class UIButton : public UIElement
{
public:
	SDL_Rect btn_normal;
	SDL_Rect btn_focused;
	SDL_Rect btn_pressed;
	UILabel* button_lbl = nullptr;

private:
	uint pressed_fx = 0u, released_fx = 0u, hover_fx = 0u;

public:
	UIButton(int x, int y, UI_Type type, SDL_Rect normal_rect, SDL_Rect focused_rect, SDL_Rect pressed_rect, j1Module* callback, UIElement* parent = nullptr);
	~UIButton();
	void UpdateButtonWithSelfRect(SDL_Rect self_rect);
};

#endif //__UIIMAGE_H__