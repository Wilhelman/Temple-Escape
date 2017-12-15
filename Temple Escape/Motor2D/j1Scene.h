#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "PugiXml/src/pugixml.hpp"
#include "j1Module.h"
#include "p2DynArray.h"
#include "p2Animation.h"

#define BUTTON_HOVER_OFFSET 6
#define BUTTON_PUSH_OFFSET 3

struct SDL_Texture;
class UIButton;
class UIImage;
class UILabel;
class p2Animation;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

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

	bool IsGamePaused();

	void LoadSceneAnimation(pugi::xml_node animation_node, p2Animation * animation);

private:
	void putPlayerToSpawn();

	p2DynArray<UIButton*> buttons;
	p2DynArray<UIButton*> hud_elements;

	UIImage* pause_menu = nullptr;
	UIImage* player_lives = nullptr;
	UIImage* coin_ui = nullptr;

	UILabel* score_lbl = nullptr;
	UILabel* continue_lbl = nullptr;
	UILabel* save_game_lbl = nullptr;
	UILabel* load_game_lbl = nullptr;
	UILabel* main_menu_lbl = nullptr;
	UILabel* quit_lbl = nullptr;

	UIButton* continue_btn = nullptr;
	UIButton* load_game_btn = nullptr;
	UIButton* save_game_btn = nullptr;
	UIButton* main_menu_btn = nullptr;
	UIButton* quit_btn = nullptr;

	p2Animation heart_reward_anim;
	p2Animation coin_reward_anim;

	p2SString spritesheetName;

	const SDL_Texture* atlas_tex = nullptr;

	int last_score = 0;

	bool paused = false;
	bool quit_btn_pressed = false;
	bool rewarded = false;
	
};

#endif // __j1SCENE_H__