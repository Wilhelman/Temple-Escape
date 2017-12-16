#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#include "j1Fonts.h"
#include "UIElement.h"
#include "UIButton.h"

class UISlider : public UIElement
{
public:
	SDL_Rect slider_rect;

private:
	uint slider_value = 0u;
	UIButton* slider_btn = nullptr;

public:
	UISlider(int x, int y, UI_Type type, SDL_Rect slider_rect, j1Module* callback, UIElement* parent = nullptr);

	void SetSliderButtons(UIButton* slider_btn);
	void SetSliderValue(uint slider_btn_pos);
	void SetSliderValueStart(int slider_value);

	uint GetSliderValue() const;
	UIButton* GetSliderButton() const;
};

#endif //__UISLIDER_H__