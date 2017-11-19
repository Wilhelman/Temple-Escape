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
	j1Window*			win;
	j1Input*			input;
	j1Render*			render;
	j1Textures*			tex;
	j1Audio*			audio;
	j1Collider*			collider;
	j1Scene*			scene;
	j1Map*				map;
	j1PathFinding*		pathfinding;
	j1Entities*			entities;
	j1Particles*		particles;
	j1FadeToBlack*		fadeToBlack;


private:

	p2List<j1Module*>	modules;
	int					argc;
	char**				args;

	p2SString			game_title;
	p2SString			organization;
	p2SString			vsync_to_show;
	p2SString			cap_to_show;

	mutable bool		want_to_save;
	bool				want_to_load;
	mutable p2SString	save_game;
	p2SString			load_game;

	j1PerfTimer			perf_timer;
	j1Timer				simple_timer;

	j1PerfTimer			ptimer;
	uint64				frame_count = 0;
	uint				cap;
	j1Timer				startup_time;
	j1Timer				frame_time;
	j1Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;
	bool				all_modules_loaded;
	bool				cap_state;
};

extern j1App* App; // No student is asking me about that ... odd :-S

#endif