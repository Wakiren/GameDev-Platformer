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
	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(0, 0);
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/player1.png");

	// L08 TODO 5: Add physics to the player - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;



	return true;
}

bool Player::Update(float dt)
{
	Walking(dt);
	Jumping();
	SetPosition();
	TextureRendering();
	CameraFollow(dt);
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::Walking(float dt) 
{
	b2Vec2 velocity = b2Vec2(0, 0);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * dt;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * dt;
	}
	velocity.y = pbody->body->GetLinearVelocity().y;
	pbody->body->SetLinearVelocity(velocity);
}

void Player::Jumping() 
{
	b2Vec2 velocity = b2Vec2(0, 0);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) 
	{
		if (canJump > 0)
		{
			velocity.y = -4;
			canJump--;
			pbody->body->SetLinearVelocity(velocity);
		}
	}

}

void Player:: TextureRendering()
{
	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());
}

void Player::SetPosition() 
{
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
}

void Player::CameraFollow(float dt) 
{
	//No need for delta time because it is already implemented in horizontal velocity
	Engine::GetInstance().render.get()->camera.x = Lerp(Engine::GetInstance().render.get()->camera.x,
	-METERS_TO_PIXELS(pbody->body->GetPosition().x), 0.01f);
}

float Player::Lerp(float a, float b, float t)
{
		return a + t * (b - a);
}
// L08 TODO 6: Define OnCollision function for the player. 
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