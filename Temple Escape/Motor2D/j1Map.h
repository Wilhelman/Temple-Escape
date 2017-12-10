#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "p2Animation.h"

class p2String;
// ----------------------------------------------------

enum LayerTypes
{
	BACKGROUND = 0,
	PARALLAX,
	DEAD_ZONE,
	GROUND_1,
	GROUND_2,
	LOGIC,
	COLLISIONS,
	ENEMIES,

	LAYER_NOT_DEF
};

enum TileSetTypes
{
	PLATFORM = 0,
	FLAG,
	

	TILESET_NOT_DEF
};

struct Properties
{
	struct Property
	{
		p2SString name;
		int value = 0;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

struct MapLayer {

	p2SString name;
	uint width = 0; //number of tiles in the x axis
	uint height = 0; //number of tiles in the y axis
	Properties	properties;

	uint* data = nullptr;
	uint size_data = 0;

	LayerTypes layer_type = LAYER_NOT_DEF;

	~MapLayer() {
		RELEASE(data);
	}

	inline uint Get(int x, int y) const;
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid = 0;
	int					margin = 0;
	int					spacing = 0;
	int					tile_width = 0;
	int					tile_height = 0;
	SDL_Texture*		texture = nullptr;
	int					tex_width = 0;
	int					tex_height = 0;
	int					num_tiles_width = 0;
	int					num_tiles_height = 0;
	int					offset_x = 0;
	int					offset_y = 0;

	TileSetTypes		tileset_type = TILESET_NOT_DEF;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

// ----------------------------------------------------
struct MapData
{
	int					width = 0;
	int					height = 0;
	int					tile_width = 0;
	int					tile_height = 0;
	SDL_Color			background_color = { 0, 0, 0, 0 };
	MapTypes			type = MAPTYPE_UNKNOWN;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>   layers;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;

	iPoint WorldToMap(int x, int y) const;

	void setAllLogicForMap();
	void LayersSetUp();

	TileSet* GetTilesetFromTileId(int id) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;


private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	void LoadMapAnimation(pugi::xml_node animation_node, p2Animation* animation);

public:

	MapData data;
	p2SString sceneName;
	iPoint spawn = iPoint({ -1, -1 });

	//for comodity c:
	MapLayer* collisionLayer = nullptr;

private:

	pugi::xml_document	map_file;

	p2SString			folder;
	p2SString			spritesheetName;

	bool				map_loaded = false;

	p2Animation			lava_waterfall = p2Animation();
	p2Animation			lava_animation = p2Animation();
	p2Animation			water_animation = p2Animation();
	p2Animation			waterfall = p2Animation();

};

#endif // __j1MAP_H__