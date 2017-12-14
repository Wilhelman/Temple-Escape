#ifndef __j1MAINMENU_H__
#define __j1MAINMENU_H__

#include "j1Module.h"
#include "p2DynArray.h"

#define BUTTON_HOVER_OFFSET 6
#define BUTTON_PUSH_OFFSET 3

class UIButton;
class UILabel;
class UIImage;
class UISlider;
class UICheckBox;

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

	iPoint GetPointToCenter(int w_to_center, int h_to_center, int w_ref, int h_ref);

public:
	bool move_camera = false;
	bool capped_checkbox = false;

private:

	UIButton* new_game_btn = nullptr;
	UIButton* continue_btn = nullptr;
	UIButton* settings_btn = nullptr;
	UIButton* quit_game_btn = nullptr;
	UIButton* close_settings_btn = nullptr;
	UIButton* music_slider_btn = nullptr;
	UIButton* fx_slider_btn = nullptr;
	UIButton* language_right_btn = nullptr;
	UIButton* language_left_btn = nullptr;

	UIImage* settings_menu = nullptr;

	UISlider* music_volume_slider = nullptr;
	UISlider* fx_volume_slider = nullptr;

	UILabel* music_volume_slider_lbl = nullptr;
	UILabel* music_volume_lbl = nullptr;
	UILabel* fx_volume_slider_lbl = nullptr;
	UILabel* fx_volume_lbl = nullptr;
	UILabel* cap_lbl = nullptr;
	UILabel* language_lbl = nullptr;
	UILabel* selected_language_lbl = nullptr;

	UICheckBox* cap_checkbox = nullptr;

	p2DynArray<UIButton*> buttons;
	p2DynArray<UILabel*> labels;
	p2DynArray<UIElement*> settings_elements;

	bool quit_btn_pressed = false;
	bool settings_up = false;
	bool settings_down = false;

	int camera_limit = 0;
	int camera_step_move = 0;
	int game_btn_original_pos = 0;
	int settings_menu_original_pos = 0;
	int game_btn_final_pos = 0;
	int settings_menu_final_pos = 0;
};


#endif // __j1MAINMENU_H__