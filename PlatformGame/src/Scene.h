#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Vector2D.h"
#include "GuiControlButton.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}
	// Return the player position
	Vector2D GetPlayerPosition();

	//L15 TODO 1: Implement the Load function
	void LoadState();
	//L15 TODO 2: Implement the Save function
	void SaveState();

	// Handles multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);


private:
	SDL_Texture* img;
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	std::vector<Enemy*> enemyList;

	GuiControlButton* guiBt;
};