#ifndef __j1UI_H__
#define __j1UI_H__

#include "j1Module.h"
#include "p2DynArray.h"

#define CURSOR_WIDTH 2

struct SDL_Texture;
struct SDL_Rect;
class p2SString;
class UIElement;

enum Color
{
	RED,
	GREEN,
	BLUE,
	YELLOW,
	GREY,
	BLACK,
	
	COLOR_NOT_DEF
};

enum UI_Type
{
	LABEL,
	BUTTON,
	IMAGE,
	SLIDER,

	UI_NOT_DEF
};

// ---------------------------------------------------
class j1UI : public j1Module
{

public:

	j1UI();

	// Destructor
	virtual ~j1UI();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions

	const SDL_Texture* GetAtlas() const;

	bool DeleteUIElement(UIElement &element);
	bool DeleteAllUIElements();

	UIElement* AddUIImage(int position_x, int position_y, SDL_Rect rect, j1Module* callback = nullptr, UIElement* parent = nullptr);
	UIElement* AddUIButton(int position_x, int position_y, SDL_Rect normal_rect,SDL_Rect focused_rect,SDL_Rect pressed_rect ,j1Module* callback = nullptr, UIElement* parent = nullptr);
	UIElement* AddUILabel(int position_x, int position_y, p2SString text, Color color = COLOR_NOT_DEF, UIElement* parent = nullptr);
	UIElement* AddUISlider(int position_x, int position_y, SDL_Rect slider_rect, j1Module* callback = nullptr, UIElement* parent = nullptr);
	UIElement* GetElementUnderMouse(int x, int y);

private:

	p2DynArray<UIElement*> ui_elements;

	SDL_Texture* atlas = nullptr;
	p2SString atlas_file_name;
};

#endif // __j1GUI_H__