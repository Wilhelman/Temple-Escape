#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

#include "UIElement.h"


class UICheckBox : public UIElement
{
public:
	SDL_Rect cb_tick_normal;
	SDL_Rect cb_no_tick_normal;
	SDL_Rect cb_tick_pressed;
	SDL_Rect cb_no_tick_pressed;
	SDL_Rect cb_tick_focus;
	SDL_Rect cb_no_tick_focus;


private:
	uint pressed_fx = 0u, released_fx = 0u, hover_fx = 0u;
	bool check_box_state = false;

public:
	UICheckBox::UICheckBox(int x, int y, UI_Type type, SDL_Rect cb_tick_normal, SDL_Rect cb_no_tick_normal, SDL_Rect cb_tick_pressed, SDL_Rect cb_no_tick_pressed, SDL_Rect cb_tick_focus, SDL_Rect cb_no_tick_focus, j1Module* callback, UIElement* parent = nullptr);
	~UICheckBox();
	void UpdateCheckBoxWithSelfRect(SDL_Rect self_rect);
	bool GetCheckBoxState() const;
};

#endif //__UICHECKBOX_H__