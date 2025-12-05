#pragma once
#include "Object3d.h"
#include "WinApp.h"
#include "Vector3.h"
#include "Model.h"
#include <list>

//GameScene‚Ì‘O•ûéŒ¾
class GameScene;

class Energy : public Object3d
{
public:

	void EnergyInitialize();
	
	void Update(Vector3 pos, Vector3 rot);

	void OnCollision(const CollisionInfo& info) override;

	bool GetIsDead() const { return isDead; }

private:
	// ƒ‚ƒfƒ‹
	Model* energyModel = nullptr;
	
	bool isDead;
};
