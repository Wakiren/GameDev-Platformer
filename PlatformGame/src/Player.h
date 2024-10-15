#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"

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

	float Lerp(float a, float b, float t);

	int canJump = 0;

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

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
};