#ifndef __j1APP_H__
#define __j1APP_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1Collider;
class j1Scene;
class j1Entities;
class j1Particles;
class j1Map;
class j1PathFinding;
class j1FadeToBlack;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	void GetSaveGames(p2List<p2SString>& list_to_fill) const;

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

private:

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	j1Window*			win = nullptr;
	j1Input*			input = nullptr;
	j1Render*			render = nullptr;
	j1Textures*			tex = nullptr;
	j1Audio*			audio = nullptr;
	j1Collider*			collider = nullptr;
	j1Scene*			scene = nullptr;
	j1Map*				map = nullptr;
	j1PathFinding*		pathfinding = nullptr;
	j1Entities*			entities = nullptr;
	j1Particles*		particles = nullptr;
	j1FadeToBlack*		fadeToBlack = nullptr;


private:

	p2List<j1Module*>	modules;
	int					argc = 0;
	char**				args = nullptr;

	p2SString			game_title;
	p2SString			organization;
	p2SString			vsync_to_show;
	p2SString			cap_to_show;

	mutable bool		want_to_save = false;
	bool				want_to_load = false;
	mutable p2SString	save_game;
	p2SString			load_game;

	j1PerfTimer			perf_timer;
	j1Timer				simple_timer;

	j1PerfTimer			ptimer;
	uint64				frame_count = 0;
	uint				cap = 0u;
	j1Timer				startup_time;
	float				dt = 0.0f;
	int					capped_ms = -1;
	bool				all_modules_loaded = false;
	bool				cap_state = false;
};

extern j1App* App; // No student is asking me about that ... odd :-S

#endif