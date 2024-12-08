#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "box2d/box2d.h"
#include "Animation.h"
#include "Vector2D.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void Walking(float dt);

	void Running(float dt);

	void Jumping(float dt);

	void Dashing(float dt);

	void TextureRendering();

	void SetPosition();

	void CameraFollow(float dt);

	void AnimationManager();

	float Lerp(float a, float b, float t);

	int canJump = 0;

	bool onFloor;
	
	void RayCast();

	void GodMode();
	bool godMode;

	enum class State
	{
		IDLE,
		WALKING,
		RUNNING,
		JUMPING,
		DASHING,
		CLIMBING,
		FALLING,
		DIED
	};
	enum class Facing
	{
		LEFT,
		RIGHT
	};

	State state;
	Facing facing;

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}
	float x, y;
	void SetPosition(Vector2D pos);

	Vector2D GetPosition();
public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	SDL_Texture* animations = NULL;

	int texW, texH;
	int aniW, aniH;

	float dashDuration = 60.0f;
	float currentTime = dashDuration;
	bool activateTimer = false;

	//Audio fx
	int pickCoinFxId;
	int jump;

	pugi::xml_node parameters;

	Animation* currentAnimation = nullptr;
	Animation idle, walkingright, walkingleft, jumpingright, jumpingleft;
	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	PhysBody* pbodyFoot;

};