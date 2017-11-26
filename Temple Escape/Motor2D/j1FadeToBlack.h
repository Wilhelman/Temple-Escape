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
	bool FadeToKnowBlack(p2SString lvlToFade, float time = 2.0f);
	bool FadeIsOver();

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen = { 0, 0, 0, 0 };

	p2SString lvlName;

public:
	bool F1 = false;
};

#endif //__MODULEFADETOBLACK_H__