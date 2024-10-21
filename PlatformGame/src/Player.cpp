#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {
	position = Vector2D(30, 80);
	return true;
}

bool Player::Start() {

	animations = Engine::GetInstance().textures.get()->Load("Assets/Maps/1-bitPack/Tilemap/monochrome_tilemap_transparent_torch_modified.png");

	Engine::GetInstance().textures.get()->GetSize(animations, aniW, aniH);

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), aniW / 20 / 2, bodyType::DYNAMIC);

	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;

	state = State::IDLE;
	facing = Facing::RIGHT;

	pbody->body->GetFixtureList()->SetFriction(0);
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
	return true;

}

bool Player::CleanUp()
{
	LOG("Cleanup player");
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
			Running(dt);
			return;
		}
		velocity.x = -0.1 * dt;
		state = State::WALKING;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		facing = Facing::RIGHT;
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			Running(dt);
			return;
		}
		velocity.x = 0.1 * dt;
		state = State::WALKING;
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
			state = State::JUMPING;
			velocity.y = -0.2 * dt;
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
		velocity.x = -0.2f * dt;
	}
	else 
	{
		velocity.x = 0.2f * dt;
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
			velocity.x = -0.50f * dt;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			velocity.x = 0.50f * dt;
		}

		pbody->body->SetLinearVelocity(velocity);
	}

	state = State::DASHING;
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

	Engine::GetInstance().render.get()->DrawTexture(animations, (int)position.getX() + 8, (int)position.getY() + 4 ,&frame,1.0f, 0.0f, frame.w/2, frame.h/2, 1);
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

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		canJump = 2;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}