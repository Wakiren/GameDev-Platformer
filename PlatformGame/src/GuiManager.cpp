#include "GuiManager.h"
#include "Engine.h"
#include "Textures.h"

#include "GuiControlButton.h"
#include "Audio.h"
#include "Timer.h"

#include "SDL2/SDL.h"
#include <iostream>
using namespace std;


GuiManager::GuiManager() :Module()
{
	name = "guiManager";
}

GuiManager::~GuiManager() {}

bool GuiManager::Start()
{
	IntroScreen  = Engine::GetInstance().textures.get()->Load("Assets/Textures/UI/IntroScreen.png");
	TitleScreen = Engine::GetInstance().textures.get()->Load("Assets/Textures/UI/TitleScreen.png");
	BlackRectangle = Engine::GetInstance().textures.get()->Load("Assets/Textures/UI/BlackRectangle.png");

	rect = { 0, 0,2370, 1335 };
	pBt = { 520, 350, 120,120 };

	alpha = 0;
	fadeSpeed = 4;

	return true;
}

// L16: TODO 1: Implement CreateGuiControl function that instantiates a new GUI control and add it to the list of controls
GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds)
{
	GuiControl* guiControl = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:
		guiControl = new GuiControlButton(id, bounds, text);
		break;
	}

	//Set the observer
	guiControl->observer = observer;

	// Created GuiControls are add it to the list of controls
	guiControlsList.push_back(guiControl);

	return guiControl;
}


bool GuiManager::Update(float dt)
{	
	for (const auto& control : guiControlsList)
	{
		control->Update(dt);
	}

	float cameraX = Engine::GetInstance().render.get()->camera.x;
	float cameraY = Engine::GetInstance().render.get()->camera.y;

	switch (state)
	{
		case GuiManager::INTRO:


			Engine::GetInstance().render.get()->DrawTexture(IntroScreen, 2 , 0, &rect,1, 0, 0, 0, 0.18);
			Engine::GetInstance().render.get()->DrawTexture(BlackRectangle, 2, 0, &rect, 1, 0, 0, 0, 0.18, alpha);
			if (alpha < 255 - fadeSpeed) { alpha += fadeSpeed; }


			startIntroTimer = true;
			if (startIntroTimer) 
			{
				startIntroTimer = false;
				introTimer += dt;
			}
			if (introTimer >= introTimerTime) 
			{
				state = TITLE;
				introTimer = 0;
			}


			break;
		case GuiManager::TITLE:

			Engine::GetInstance().render.get()->DrawTexture(TitleScreen, 2, 0, &rect, 1, 0, 0, 0, 0.18);
			Engine::GetInstance().render.get()->DrawTexture(BlackRectangle, 2, 0, &rect, 1, 0, 0, 0, 0.18, alpha);

			if (alpha > 0 + fadeSpeed) 
			{
				alpha -= fadeSpeed;
			}


			Play = (GuiControlButton*)CreateGuiControl(GuiControlType::BUTTON, 1, "PlayButton", pBt, this);

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) 
			{
				state = GAME;
			}
			break;
		case GuiManager::GAME:
			break;
		case GuiManager::PAUSE:
			break;
		case GuiManager::CREDIT:
			break;
		case GuiManager::ENDLEVEL:
			break;
		case GuiManager::END:
			break;
		case GuiManager::GAMEOVER:
			break;
		default:
			break;
	}

	return true;
}

bool GuiManager::CleanUp()
{
	for (const auto& control : guiControlsList)
	{
		delete control;
	}

	return true;
}



