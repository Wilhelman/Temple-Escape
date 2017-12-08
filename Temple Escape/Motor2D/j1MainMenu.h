#ifndef __j1MAINMENU_H__
#define __j1MAINMENU_H__

#include "j1Module.h"
#include "p2DynArray.h"

class UIButton;
class UILabel;

class j1MainMenu : public j1Module
{
public:

	j1MainMenu();

	// Destructor
	virtual ~j1MainMenu();

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

private:

	UIButton* new_game_btn = nullptr;

	p2DynArray<UIButton*> buttons;
	p2DynArray<UILabel*> labels;
};


#endif // __j1MAINMENU_H__