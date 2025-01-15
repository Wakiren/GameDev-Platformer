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

void GuiManager::FadeToBlack(float fadeSpeed)
{
		SDL_Renderer* myRenderer = Engine::GetInstance().render.get()->renderer;
		float screenW = Engine::GetInstance().render.get()->viewport.w;
		float screenH = Engine::GetInstance().render.get()->viewport.h;
		SDL_Rect fadeRect = { 0, 0, screenW, screenH };
		Uint8 alpha = 0;
		while (alpha < 255) {
			alpha += fadeSpeed; 
			if (alpha > 255) alpha = 255; 

			Engine::GetInstance().render.get()->DrawRectangle(fadeRect, 0,0,0,alpha, true, false);
			cout << alpha << endl;

		}
}

bool GuiManager::Update(float dt)
{	
	for (const auto& control : guiControlsList)
	{
		control->Update(dt);
	}
	SDL_Rect rect = { 0, 0, 1580*1.5f, 890*1.5f };
	float cameraX = Engine::GetInstance().render.get()->camera.x;
	float cameraY = Engine::GetInstance().render.get()->camera.y;

	switch (state)
	{
		case GuiManager::INTRO:



			Engine::GetInstance().render.get()->DrawTexture(IntroScreen, 2 , 0, &rect,1, 0, 0, 0, 0.18 );
			//if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN && !inTransition)
			//{
			//	inTransition = true;
			//	FadeToBlack(1.f);
			//}

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



