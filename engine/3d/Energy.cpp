#include "Energy.h"
#include "string.h"
#include "BaseCollider.h"
#include "SphereCollider.h"

void Energy::EnergyInitialize()
{
	Initialize();
	// OBJからモデルデータを読み込む
	energyModel = Model::LoadFromOBJ("panel");
	// 3Dオブジェクト生成
	Create();
	// オブジェクトにモデルをひも付ける
	SetModel(energyModel);
	SetScale(Vector3(0.5f, 0.5f, 0.5f));
	isDead = false;
}

void Energy::Update(Vector3 pos,Vector3 rot)
{
	//playerのもとへ
	Vector3 velo = pos - GetPosition();
	velo = velo.normalize();
	SetPosition(GetPosition() + velo);
	SetRotation(rot + Vector3(0,90,0));

	worldTransform_.UpdateMatrix();

	//当たり判定更新
	if (collider)
	{
		collider->Update();
	}
}

void Energy::OnCollision(const CollisionInfo& info)
{
	//衝突相手の名前
	const char* str1 = "class Player";

	//相手がplayerの弾
	if (strcmp(toCollisionName, str1) == 0) {
		if (isDead == false) {
			isDead = true;
		}
	}
}
