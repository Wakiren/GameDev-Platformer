#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{

}

Enemy::~Enemy() {
	pbody->body->GetWorld()->DestroyBody(pbody->body);
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	if (parameters.attribute("gravity").as_bool() == true) 
	{
		pbody->body->SetGravityScale(8);
	}

	// Initialize pathfinding
	pathfinding = new Pathfinding();


	ResetPath();

	return true;
}

bool Enemy::Update(float dt)
{
		

		Vector2D target = Engine::GetInstance().scene.get()->GetPlayerPosition();

		distance.setX(abs(target.getX() - GetPosition().getX()));
		distance.setY(abs(target.getY() - GetPosition().getY()));

		visionLimit = Engine::GetInstance().map.get()->MapToWorld(16, 16);

		if (IsInVision())
		{
			if (check < 20)
			{
				pathfinding->PropagateAStar(SQUARED);
				check += 1;
			}
			else {
				Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(GetPosition().getX(), GetPosition().getY());
				pathfinding->ResetPath(tilePos);
				check = 0;
			}
			if (pathfinding->pathTiles.size() > 0) {
				Vector2D nextTile = pathfinding->pathTiles.front();
				Vector2D nextPos = Engine::GetInstance().map->MapToWorld(nextTile.getX(), nextTile.getY());
				Vector2D direction = nextPos - GetPosition();
				direction.normalized();
				if (parameters.attribute("gravity").as_bool() == false)
				{
					eVelocity = b2Vec2(direction.getX() * 0.02f, direction.getY() * 0.02f);
				}
				else 
				{
					eVelocity = b2Vec2(direction.getX() * 0.02f, 0);
				}


				pbody->body->SetLinearVelocity(eVelocity);
			}
			else {
				pbody->body->SetLinearVelocity(b2Vec2_zero);
			}
		}
		else
		{
			pbody->body->SetLinearVelocity(b2Vec2_zero);
		}

		// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() - 2, &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();

		if (Engine::GetInstance().physics.get()->debug)
		{
			// Draw pathfinding 
			pathfinding->DrawPath();
		}


		return true;
	
}
bool Enemy::CleanUp()
{
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}
void Enemy::PropagatePath()
{
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	{
		propagatePath = true;
	}

	if (propagatePath)
	{
		ResetPath();
		while (pathfinding->pathTiles.empty())
		{
			pathfinding->PropagateAStar(SQUARED);
		}

		FollowPath();
	}


}
void Enemy::FollowPath() 
{
	if (pathfinding->breadcrumbs.size() > 1) 
	{
		for (size_t i = 0; i < pathfinding->breadcrumbs.size(); i++)
		{
			float posx = pathfinding->breadcrumbs[i].getX();
			float posy = pathfinding->breadcrumbs[i].getY();
			Vector2D pos = Engine::GetInstance().map->MapToWorld(posx, posy);
			b2Vec2 vel = b2Vec2((pos.getX()/100) , pos.getY()/100);
			pbody->body->SetLinearVelocity(vel);
		}
	}

}

void Enemy::Patroll() 
{
	
}

float Enemy::Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

bool Enemy::IsInVision()
{
	return distance.getX() <= visionLimit.getX() && distance.getY() <= visionLimit.getY();
}


