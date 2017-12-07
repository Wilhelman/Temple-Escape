#include <iostream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1MainMenu.h"
#include "j1Collider.h"
#include "j1Map.h"
#include "j1Entities.h"
#include "j1Particles.h"
#include "j1Fonts.h"
#include "j1UI.h"
#include "j1Pathfinding.h"
#include "j1FadeToBlack.h"
#include "j1App.h"

#include "Brofiler\Brofiler.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	want_to_save = want_to_load = all_modules_loaded = false;
	load_game = "save_game";
	save_game = "save_game";

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	main_menu = new j1MainMenu();
	collider = new j1Collider();
	map = new j1Map();
	entities = new j1Entities();
	particles = new j1Particles();
	font = new j1Fonts();
	ui = new j1UI();
	pathfinding = new j1PathFinding();
	fadeToBlack = new j1FadeToBlack();

	//TODO : CHECK ALL TODO'S!!!

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(scene);
	AddModule(main_menu);
	AddModule(entities);
	AddModule(particles);
	AddModule(font);
	AddModule(ui);
	AddModule(collider);
	AddModule(fadeToBlack);

	// render last to swap buffer
	AddModule(render);

	//disable
	scene->active = false;
	entities->active = false;

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
	cap_state = true;

	config = LoadConfig(config_file);


	if (config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		game_title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());

		cap = app_config.attribute("framerate_cap").as_uint();

		if (cap > 0)
			capped_ms = 1000 / cap;

	}

	if (ret == true)
	{
		p2List_item<j1Module*>* item;
		item = modules.start;

		while (item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while (item != NULL && ret == true)
	{
		if (item->data->active == false) {
			item = item->next;
			continue;
		}
		ret = item->data->Start();
		item = item->next;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	BROFILER_CATEGORY("Update", Profiler::Color::Green)

		bool ret = true;
	PrepareUpdate();

	if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if (result == NULL)
		LOG("Could not load xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");
	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{

	perf_timer.Start();

}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	frame_count++;

	if (want_to_save == true)
		SavegameNow();

	if (want_to_load == true)
		LoadGameNow();

	// Framerate calculations --


	if (App->render->vsync_state)
		vsync_to_show = "on";
	else
		vsync_to_show = "off";

	if (cap_state)
		cap_to_show = "on";
	else
		cap_to_show = "off";

	float avg_fps = float(frame_count) / startup_time.ReadSec();

	float seconds_since_startup = simple_timer.Read();

	uint32 current_ms_frame = perf_timer.ReadMs();
	uint32 last_frame_ms = current_ms_frame;
	uint32 frames_on_last_update = 0;



	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		cap_state = !cap_state;

	if (cap_state) {
		capped_ms = 1000 / cap;

		if (current_ms_frame < capped_ms && !App->render->vsync_state)
			SDL_Delay(capped_ms - current_ms_frame);
	}

	double framerate = 1000.0f / perf_timer.ReadMs();

	dt = 1.0f / framerate;

	static char title[256];
	sprintf_s(title, 256, "%s - FPS: %.2f Av.FPS: %.2f Last Frame Ms: %u (Cap: %s  Vsync: %s) ",
		game_title.GetString(), framerate, avg_fps, last_frame_ms, cap_to_show.GetString(), vsync_to_show.GetString());

	App->win->SetTitle(title);

	if (!all_modules_loaded)
		all_modules_loaded = true;
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	BROFILER_CATEGORY("Preupdate", Profiler::Color::Orchid)
		bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		(all_modules_loaded) ? ret = item->data->Update(dt) : ret = item->data->Update(0);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	BROFILER_CATEGORY("PostUpdate", Profiler::Color::Blue)
	PERF_START(ptimer);
	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if (pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	//PERF_PEEK(ptimer);
	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return game_title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void j1App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

// ---------------------------------------
void j1App::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
}

// ---------------------------------------
void j1App::GetSaveGames(p2List<p2SString>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool j1App::LoadGameNow()
{
	BROFILER_CATEGORY("LoadGame", Profiler::Color::Orange)
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.GetString());

	if (result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.GetString());

		root = data.child("game_state");

		p2List_item<j1Module*>* item = modules.start;
		ret = true;

		while (item != NULL && ret == true)
		{
			ret = item->data->Load(root.child(item->data->name.GetString()));
			item = item->next;
		}

		data.reset();
		if (ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	BROFILER_CATEGORY("SaveGame", Profiler::Color::Purple)
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	p2List_item<j1Module*>* item = modules.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if (ret == true)
	{
		data.save_file(save_game.GetString());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}