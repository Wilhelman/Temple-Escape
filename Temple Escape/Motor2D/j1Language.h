#ifndef __j1LANGUAGE_H__
#define __j1LANGUAGE_H__

#include "j1Module.h"
#include "p2SString.h"
#include "p2DynArray.h"


struct Dictionary {
	p2SString new_game_btn;
};

class j1Language : public j1Module
{
public:

	j1Language();

	// Destructor
	virtual ~j1Language();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	p2SString current_language;
	p2DynArray<p2SString> posible_languages;

	Dictionary GetDictionary()const;

private:

	Dictionary dictionary;

};

#endif // __j1AUDIO_H__