#ifndef __j1LANGUAGE_H__
#define __j1LANGUAGE_H__

#include "j1Module.h"
#include "p2SString.h"
#include "p2DynArray.h"


struct Dictionary {
	p2SString new_game_btn;
	//p2SString continue_btn	todo:2
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

	void ChangeCurrentLanguage(p2SString new_language);

	Dictionary GetDictionary()const;

public:
	p2SString current_language;
	p2DynArray<p2SString> posible_languages;

private:

	Dictionary dictionary;

};

#endif // __j1AUDIO_H__