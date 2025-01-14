#pragma once

#include "Module.h"
#include "GuiControl.h"

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

	void FadeToBlack(float fadeSpeed);
	bool inTransition;
	float introTimer = 0;
	bool startIntroTimer;

};

