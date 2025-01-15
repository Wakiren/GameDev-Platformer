#pragma once

#include "Module.h"
#include "GuiControl.h"
#include "GuiControlButton.h"

#include <list>

class GuiManager : public Module
{
public:

	// Constructor
	GuiManager();

	// Destructor
	virtual ~GuiManager();

	// Called before the first frame
	 bool Start();

	 // Called each loop iteration
	 bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 });

public:

	std::list<GuiControl*> guiControlsList;
	SDL_Texture* texture;

	enum UIState 
	{
		INTRO,
		TITLE,
		GAME,
		PAUSE,
		CREDIT,
		ENDLEVEL,
		END,
		GAMEOVER
	};

	UIState state = INTRO;

private:
	SDL_Texture* IntroScreen;
	SDL_Texture* TitleScreen;
	SDL_Texture* GameOverScreen;
	SDL_Texture* NextLevelScreen;
	SDL_Texture* CreditScreen;
	SDL_Texture* BlackRectangle;

	GuiControlButton* Play;
	GuiControlButton* Credits;
	GuiControlButton* Options;

	SDL_Rect rect;
	SDL_Rect pBt;

	Uint8 alpha;
	float fadeSpeed;


	float introTimer = 0;
	float introTimerTime = 1200;
	bool startIntroTimer;

};

