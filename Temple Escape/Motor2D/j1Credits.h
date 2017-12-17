#ifndef __j1CREDITS_H__
#define __j1CREDITS_H__

#include "j1Module.h"
#include "p2DynArray.h"

#define BUTTON_HOVER_OFFSET 6
#define BUTTON_PUSH_OFFSET 3

class UIButton;
class UILabel;
class UIImage;
class UISlider;
class UICheckBox;

class j1Credits : public j1Module
{
public:

	j1Credits();

	// Destructor
	virtual ~j1Credits();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	void OnUITrigger(UIElement* elementTriggered, UI_State ui_state);

public:

private:
	int scroll_speed = 1;
	p2DynArray<UIButton*> buttons;
	p2DynArray<UILabel*> labels;
	p2DynArray<UIElement*> settings_elements;

	UILabel* hold_lbl = nullptr;
	UILabel* licence_lbl = nullptr;
	UILabel* team_lbl = nullptr;

	UIButton *git_ggs_btn = nullptr;

	bool credits_over = false;
};


#endif // __j1MAINMENU_H__