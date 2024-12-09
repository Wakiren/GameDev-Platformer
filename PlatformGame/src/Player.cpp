#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include <iostream>

using namespace std;
Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {
	pbody->body->GetWorld()->DestroyBody(pbody->body);
}

bool Player::Awake() {
	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(0,0);
	return true;
}

bool Player::Start() {

	animations = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	// L08 TODO 5: Add physics to the player - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(animations, aniW, aniH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), aniW / 8 / 2, bodyType::DYNAMIC);

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	state = State::IDLE;
	facing = Facing::RIGHT;

	pbody->body->GetFixtureList()->SetFriction(0);

	b2PolygonShape sensorShape;
	sensorShape.SetAsBox(0.03f, 0.01f, b2Vec2(0, 0.2f), 0);

	b2FixtureDef sensorFixtureDef;
	sensorFixtureDef.shape = &sensorShape;
	sensorFixtureDef.isSensor = true;
	sensorFixtureDef.userData.pointer = (uintptr_t)ColliderType::FOOT_SENSOR;

	pbody->body->CreateFixture(&sensorFixtureDef);

	pbody->body->SetFixedRotation(true);

	//Sounds
	jump = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/jump.wav");
	gameSaved = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/confirmation_001.ogg");
	enemydead = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/enemydead.wav");
	death = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/death.wav");
	lComplete = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/win.mp3");

	return true;
}

bool Player::Update(float dt)
{
	Walking(dt);
	Jumping(dt);
	Dashing(dt);
	SetPosition();
	TextureRendering();
	CameraFollow(dt);
	AnimationManager();
	RayCast();
	GodMode();
	Death();

	return true;

}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	//Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().textures.get()->UnLoad(animations);
	return true;
}

void Player::Walking(float dt) 
{
	b2Vec2 velocity = b2Vec2(0, 0);
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		facing = Facing::LEFT;
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) 
		{
			Running(16);
			return;
		}
		velocity.x = -0.1 * 16;
		state = State::WALKING;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		facing = Facing::RIGHT;
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			Running(16);
			return;
		}
		velocity.x = 0.1 * 16;
		state = State::WALKING;
	}
	else 
	{
		if (onFloor && state != State::DIED && state != State::DASHING)
		{
			state = State::IDLE;
		}
	}
	velocity.y = pbody->body->GetLinearVelocity().y;
	pbody->body->SetLinearVelocity(velocity);
}

void Player::Jumping(float dt) 
{
	b2Vec2 velocity = b2Vec2(0, 0);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) 
	{
		if (canJump > 0)
		{
			velocity.y = -0.2 * 16;
			Engine::GetInstance().audio.get()->PlayFx(jump);
			canJump--;
			pbody->body->SetLinearVelocity(velocity);
		}
	}
}

void Player::Running(float dt) 
{
	b2Vec2 velocity = pbody->body->GetLinearVelocity();
	if (facing == Facing::LEFT) 
	{
		velocity.x = -0.2f * 16;
	}
	else 
	{
		velocity.x = 0.2f * 16;
	}

	pbody->body->SetLinearVelocity(velocity);
	state = State::RUNNING;
}

void Player::Dashing(float dt)
{
	b2Vec2 velocity = pbody->body->GetLinearVelocity();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN)
	{
		activateTimer = true;
		currentTime = dashDuration;
	}

	if (activateTimer && currentTime > 0)
	{
		currentTime -= dt;

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			velocity.x = -0.50f * 16;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			velocity.x = 0.50f * 16;
		}
		state = State::DASHING;
		pbody->body->SetLinearVelocity(velocity);
	}


}


void Player:: TextureRendering()
{
	int currentFrame;
	int maxFrames;
	
	SDL_Rect frame;

	const int tileSize = 16;
	int tilePosX = 0;
	int tilePosY = 13;
	frame.x = tileSize * tilePosX;
	frame.y = tileSize * tilePosY;
	frame.w = tileSize;
	frame.h = tileSize;


}

void Player::SetPosition() 
{
	int tileSize = 16;
	int numTilesInMapX = 20;
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - aniW / numTilesInMapX );
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - aniH / numTilesInMapX + tileSize/2);
}

void Player::CameraFollow(float dt)
{
	Engine::GetInstance().render.get()->camera.x = Lerp(Engine::GetInstance().render.get()->camera.x, -position.getX() * 3 + 200, 0.1f);
}

float Player::Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

void Player::RayCast()
{
	float rayLength = 20.0f; 

	b2Transform transform;
	transform.SetIdentity();

	b2Vec2 rayStart = pbody->body->GetPosition();
	b2Vec2 rayEnd = rayStart + b2Vec2(0, -rayLength);

	b2RayCastOutput output;
	b2RayCastInput input;
	input.p1 = rayStart;
	input.p2 = rayEnd;

	bool hitGround = false;
	for (b2Fixture* fixture = pbody->body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		if (fixture->GetShape()->RayCast(&output, input, transform, 0))
		{
			
				hitGround = true;
				break; 
			
		}
	}

	if (hitGround)
	{
		cout << "canJump";
	}

}

void Player::AnimationManager()
{
	switch (state)
	{
	case Player::State::IDLE:
		idle.LoadAnimations(parameters.child("animations").child("idle"));
		currentAnimation = &idle;
		break;
	case Player::State::WALKING:
		if (facing == Facing::RIGHT) 
		{
			walkingright.LoadAnimations(parameters.child("animations").child("walkingright"));
			currentAnimation = &walkingright;
		}
		else 
		{
			walkingleft.LoadAnimations(parameters.child("animations").child("walkingleft"));
			currentAnimation = &walkingleft;
		}

		break;
	case Player::State::RUNNING:
		if (facing == Facing::RIGHT)
		{
			walkingright.LoadAnimations(parameters.child("animations").child("walkingright"));
			currentAnimation = &walkingright;
		}
		else
		{
			walkingleft.LoadAnimations(parameters.child("animations").child("walkingleft"));
			currentAnimation = &walkingleft;
		}
		break;
	case Player::State::JUMPING:
		if (facing == Facing::RIGHT)
		{
			jumpingright.LoadAnimations(parameters.child("animations").child("jumpingright"));
			currentAnimation = &jumpingright;
		}
		else
		{
			jumpingleft.LoadAnimations(parameters.child("animations").child("jumpingleft"));
			currentAnimation = &jumpingleft;
		}
		break;
	case Player::State::DASHING:

		break;
	case Player::State::CLIMBING:

		break;
	case Player::State::FALLING:

		break;
	case Player::State::DIED:

		break;
	default:
		break;
	}
	Engine::GetInstance().render.get()->DrawTexture(animations, (int)position.getX() - 3, (int)position.getY() - 16, &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();
}

void Player::GodMode() 
{
	b2Vec2 velocity = pbody->body->GetLinearVelocity();
	if((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN))
	{
		godMode = !godMode;

	}
	if(godMode)
	{
		pbody->body->SetGravityScale(0);

		if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT))
		{
			velocity.y = -0.2f * 16;
		}
		if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT))
		{
			velocity.y = 0.2f * 16;
		}
		pbody->body->SetLinearVelocity(velocity);
	}
	else 
	{
		pbody->body->SetGravityScale(1);
	}



}

void Player::EndLevel()
{

}


void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM: {

		if (physA->body->GetFixtureList()->IsSensor() == true)
		{
			canJump = 2;
			onFloor = true;
		}

		break;
	}
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::ENEMY:
		
		if (pbody->body->GetLinearVelocity().y > 0.4) 
		{
			cout << "Enemy dead :D";
			if (canCollide) 
			{
				canCollide = false;
				Engine::GetInstance().audio.get()->PlayFx(enemydead);
				Engine::GetInstance().physics.get()->DeletePhysBody(physB);
				collidetimer = 10;

			}
		}
		else 
		{
			cout << "Player dead :<";
			dead = true;
		}


		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::END:
		cout << "AAA";
		Engine::GetInstance().audio.get()->PlayFx(lComplete);
		break;
	case ColliderType::CHECKPOINT:
		Engine::GetInstance().audio.get()->PlayFx(gameSaved);
		Engine::GetInstance().scene.get()->SaveState();
		break;
	default:
		state = State::JUMPING;
		break;
	}
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void Player::Death()
{
	if (collidetimer <= 0)
	{
		canCollide = true;
	}
	else
	{
		collidetimer--;

	}

	if (GetPosition().getY() > 16 * 12) 
	{
		dead = true;
	}

	if (dead)
	{
		Engine::GetInstance().audio.get()->PlayFx(death);
		dead = false;
		Engine::GetInstance().scene.get()->LoadState();
	}
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

