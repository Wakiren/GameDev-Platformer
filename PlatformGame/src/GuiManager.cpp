#include "GuiManager.h"
#include "Engine.h"
#include "Textures.h"

#include "GuiControlButton.h"
#include "Audio.h"

GuiManager::GuiManager() :Module()
{
	name = "guiManager";
}

GuiManager::~GuiManager() {}

bool GuiManager::Start()
{
	IntroScreen  = Engine::GetInstance().textures.get()->Load("Assets/Textures/IntroScreen");
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

	switch (state)
	{
		case GuiManager::INTRO:
			Engine::GetInstance().render.get()->DrawTexture(IntroScreen, 0, 0);
			break;
		case GuiManager::TITLE:
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



