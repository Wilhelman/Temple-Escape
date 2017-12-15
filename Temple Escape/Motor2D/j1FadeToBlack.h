#ifndef __j1FADETOBLACK_H__
#define __j1FADETOBLACK_H__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"
#include "p2SString.h"

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	~j1FadeToBlack();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	bool FadeToBlack(float time = 2.0f);
	bool FadeToBlackBetweenModules(j1Module* module_off, j1Module* module_on, float time = 2.0f, bool andLoad = false);
	bool FadeToKnowBlack(p2SString lvlToFade, float time = 2.0f);
	bool FadeIsOver();

private:

	enum FadeStep
	{
		NONE_FADE,
		FADE_TO_BLACK,
		FADE_FROM_BLACK
	} current_step = FadeStep::NONE_FADE;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen = { 0, 0, 0, 0 };

	j1Module* moduleOff = nullptr;
	j1Module* moduleOn = nullptr;
	bool have_to_load = false;

public:
	p2SString lvlName;
	bool F1 = false;
};

#endif //__MODULEFADETOBLACK_H__