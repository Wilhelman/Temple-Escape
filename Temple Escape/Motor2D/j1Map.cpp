#include "p2Defs.h"
#include "p2Log.h"
#include "p2SString.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collider.h"
#include "j1Entities.h"
#include "j1Scene.h"
#include <math.h>


j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
	spawn = iPoint(-1, -1);
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	pugi::xml_document data;

	pugi::xml_node lvlData = App->LoadConfig(data);

	for (lvlData = lvlData.child("map").child("scene").child("data"); lvlData && ret; lvlData = lvlData.next_sibling("data"))
	{
		if (lvlData.attribute("start").as_bool()) {
			sceneName = (p2SString)lvlData.attribute("name").as_string();
			lvlData.attribute("currentLvl").set_value(true);
		}
		else {
			lvlData.attribute("currentLvl").set_value(false);
		}
	}
	data.save_file("config.xml");

	//Set up animations map
	spritesheetName.create(config.child("spritesheetSource").attribute("name").as_string());

	//set all the animations
	for (pugi::xml_node animations = config.child("spritesheetSource").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
	{
		p2SString tmp(animations.attribute("name").as_string());

		if (tmp == "lava_waterfall") 
			LoadMapAnimation(animations, &lava_waterfall);
		if (tmp == "lava_animation")
			LoadMapAnimation(animations, &lava_animation);
		if (tmp == "water_animation") 
			LoadMapAnimation(animations, &water_animation);
		if (tmp == "waterfall")
			LoadMapAnimation(animations, &waterfall);

	}

	return ret;
}

void j1Map::LayersSetUp()
{
	p2List_item<TileSet*>* tilesetsToCheck = this->data.tilesets.end;

	while (tilesetsToCheck != NULL)
	{
		if (tilesetsToCheck->data->name == "environment-tiles")
			tilesetsToCheck->data->tileset_type = PLATFORM;
			
		if (tilesetsToCheck->data->name == "flag")
			tilesetsToCheck->data->tileset_type = FLAG;
		
		p2List_item<MapLayer*>* layersToCheck = this->data.layers.start;

		while (layersToCheck != NULL)
		{
			for (uint i = 0; i < layersToCheck->data->width; i++)
			{
				for (uint j = 0; j < layersToCheck->data->height; j++)
				{
					if (layersToCheck->data->Get(i, j) != 0)
					{
						if (layersToCheck->data->name == "background")
							layersToCheck->data->layer_type = BACKGROUND;
						else if (layersToCheck->data->name == "parallax")
							layersToCheck->data->layer_type = PARALLAX;
						else if (layersToCheck->data->name == "ground")
							layersToCheck->data->layer_type = GROUND_1;
						else if (layersToCheck->data->name == "ground2")
							layersToCheck->data->layer_type = GROUND_2;
						else if (layersToCheck->data->name == "dead_zone")
							layersToCheck->data->layer_type = DEAD_ZONE;
						else if (layersToCheck->data->name == "logic")
							layersToCheck->data->layer_type = LOGIC;
						else if (layersToCheck->data->name == "collisions")
							layersToCheck->data->layer_type = COLLISIONS;
						else if (layersToCheck->data->name == "enemies")
							layersToCheck->data->layer_type = ENEMIES;
					}
				}
			}
			layersToCheck = layersToCheck->next;
		}
		tilesetsToCheck = tilesetsToCheck->prev;
	}
}

void j1Map::setAllLogicForMap()
{
	p2List_item<TileSet*>* tilesetsBlit = data.tilesets.end;

	while (tilesetsBlit != NULL)
	{
		p2List_item<MapLayer*>* layersBlit = data.layers.start;

		while (layersBlit != NULL)
		{
			for (uint i = 0; i < layersBlit->data->width; i++) 
			{
				for (uint j = 0; j < layersBlit->data->height; j++) 
				{
					if (layersBlit->data->Get(i, j) != 0) 
					{
						int id = layersBlit->data->Get(i, j);
						if (layersBlit->data->layer_type == LOGIC && (tilesetsBlit->data->tileset_type == PLATFORM || tilesetsBlit->data->tileset_type == FLAG))
						{
							if(id == 434)
								spawn = MapToWorld(i, j);
							if (id == 167) 
							{
								SDL_Rect rect = tilesetsBlit->data->GetTileRect(id);
								iPoint world = MapToWorld(i, j);
								rect.x = world.x;
								rect.y = world.y;
								App->collider->AddCollider(rect, COLLIDER_LVL_END);
							}
						}
						if (layersBlit->data->layer_type == ENEMIES && tilesetsBlit->data->tileset_type == PLATFORM)
						{
							if (id == 631) { //bat
								App->entities->SpawnEntity(MapToWorld(i, j).x, MapToWorld(i, j).y, BAT);
							}
							if (id == 127) { //slime
								App->entities->SpawnEntity(MapToWorld(i, j).x, MapToWorld(i, j).y, SLIME);
							}
							if (id == 589) { //coin
								App->entities->SpawnEntity(MapToWorld(i, j).x, MapToWorld(i, j).y, COIN);
							}
						}
						if (layersBlit->data->layer_type == COLLISIONS && tilesetsBlit->data->tileset_type == PLATFORM)
						{
							SDL_Rect rect = tilesetsBlit->data->GetTileRect(id);
							iPoint world = MapToWorld(i, j);
							rect.x = world.x;
							rect.y = world.y;

							switch (id)
							{
							case 43:
								App->collider->AddCollider(rect, COLLIDER_GROUND);
								break;
							case 596:
								App->collider->AddCollider(rect, COLLIDER_DEAD);
								break;
							default:
								break;
							}
						}
					}
				}
			}
			if (layersBlit->data->layer_type == COLLISIONS)
				collisionLayer = layersBlit->data;
			layersBlit = layersBlit->next;
		}
		tilesetsBlit = tilesetsBlit->prev;
	}
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	p2List_item<TileSet*>* tilesetsBlit = data.tilesets.end;
	
	while (tilesetsBlit != NULL)
	{
		p2List_item<MapLayer*>* layersBlit = data.layers.start;

		while (layersBlit != NULL) 
		{
			for (uint i = 0; i < layersBlit->data->width; i++) 
			{
				for (uint j = 0; j < layersBlit->data->height; j++) 
				{
					if (layersBlit->data->Get(i, j) != 0) 
					{
						SDL_Rect rect = tilesetsBlit->data->GetTileRect(layersBlit->data->Get(i, j));

						iPoint world = MapToWorld(i, j);

						if (layersBlit->data->layer_type != LOGIC && layersBlit->data->layer_type != COLLISIONS) 
						{
							if (layersBlit->data->layer_type == GROUND_1) 
							{
								if (layersBlit->data->Get(i, j) != 10 && layersBlit->data->Get(i, j) != 16)
									App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &rect, 1.0f);

								if (layersBlit->data->Get(i, j) == 10)
									(App->scene->IsGamePaused()) ? App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &lava_animation.frames[0], 1.0f):
									App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &lava_animation.GetCurrentFrame(), 1.0f);
								if (layersBlit->data->Get(i, j) == 16)
									(App->scene->IsGamePaused()) ? App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &water_animation.frames[0], 1.0f) :
									App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &water_animation.GetCurrentFrame(), 1.0f);
							}

							if (layersBlit->data->layer_type == GROUND_2) {
								App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &rect, 1.0f);
								if (layersBlit->data->Get(i, j) == 94)
									(App->scene->IsGamePaused()) ? App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &lava_waterfall.frames[0], 1.0f) :
									App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &lava_waterfall.GetCurrentFrame(), 1.0f);
								if (layersBlit->data->Get(i, j) == 97)
									(App->scene->IsGamePaused()) ? App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &waterfall.frames[0], 1.0f) :
									App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &waterfall.GetCurrentFrame(), 1.0f);
								
							}

							if (layersBlit->data->layer_type == PARALLAX)
								App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &rect, 0.7f);
								
							if (layersBlit->data->layer_type == BACKGROUND)
								App->render->Blit(tilesetsBlit->data->texture, world.x, world.y, &rect, 0.1f); 
						}
					}
				}
			}

			layersBlit = layersBlit->next;
		}
		tilesetsBlit = tilesetsBlit->prev;
	}
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	switch (data.type)
	{
	case MapTypes::MAPTYPE_ORTHOGONAL:
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		ret.x = (x - y)*(data.tile_width / 2);
		ret.y = (x + y)*(data.tile_height / 2);
		break;
	default:
		break;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const {

	iPoint ret;

	switch (data.type)
	{
	case MapTypes::MAPTYPE_ORTHOGONAL:
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:

		ret.x = (x/data.width) + (y/data.tile_height);
		ret.y = (y / data.tile_height) - (x / data.width);
		break;
	default:
		break;
	}
	
	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}

	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item1;
	item1 = data.layers.start;

	while (item1 != NULL)
	{
		RELEASE(item1->data);
		item1 = item1->next;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* set = new MapLayer();

		if (ret == true)
		{
			ret = LoadLayer(layer, set);
		}

		data.layers.add(set);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}
		
		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();
	LoadProperties(node, layer->properties);

	layer->data = new uint[layer->width * layer->height];

	memset(layer->data, 0, layer->width * layer->height);
	
	int i = 0;

	for (pugi::xml_node tile_gid = node.child("data").child("tile"); tile_gid; tile_gid =  tile_gid.next_sibling("tile")) {
		layer->data[i++] = tile_gid.attribute("gid").as_uint();
	}

	return ret;
}

inline uint MapLayer::Get(int x, int y) const {

	return data[width * y + x];
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.add(p);
		}
	}

	return ret;
}

void j1Map::LoadMapAnimation(pugi::xml_node animation_node, p2Animation * animation)
{
	bool ret = true;

	for (pugi::xml_node frame = animation_node.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
		animation->PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

	animation->speed = animation_node.attribute("speed").as_float();
	animation->loop = animation_node.attribute("loop").as_bool();
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	item = data.layers.start;

	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		//TODO CHECK THIS
		if (layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
					map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	p2List_item<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
}

int Properties::Get(const char* value, int default_value) const
{
	p2List_item<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}