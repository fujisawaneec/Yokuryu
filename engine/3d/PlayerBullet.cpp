#include "PlayerBullet.h"
#include "BaseCollider.h"

void PlayerBullet::BulletInitialize(const Vector3& velocity) {

	Initialize();

	// OBJからモデルデータを読み込む
	bulletModel = Model::LoadFromOBJ("triangle_mat");
	// 3Dオブジェクト生成
	Create();
	// オブジェクトにモデルをひも付ける
	SetModel(bulletModel);

	//引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void PlayerBullet::Update() {

	SetPosition(GetPosition() + velocity_);

	worldTransform_.UpdateMatrix();

	//当たり判定更新
	if (collider)
	{
		collider->Update();
	}
	//時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

}

void PlayerBullet::HomingVec(Vector3 pos)
{
	if (isHoming == true) {
		velocity_ = lockObj->GetWorldPos() - GetPosition();
		velocity_ = velocity_ * 0.25f;
	}
}

void PlayerBullet::OnCollision(const CollisionInfo& info)
{
	//衝突相手の名前
	const char* str1 = "class Enemy";
	const char* str2 = "class EnemyBullet";
	const char* str3 = "class Boss";
	const char* str4 = "class BossBullet";

	//相手がenemy
	if (strcmp(toCollisionName, str1) == 0) {
		if (isDead_ == false) {
			isDead_ = true;
		}
	}

	//相手がenemyの弾
	if (strcmp(toCollisionName, str2) == 0) {
		if (isDead_ == false && isHoming == false) {
			isDead_ = true;
		}
	}
	//相手がボス
	if (strcmp(toCollisionName, str3) == 0) {
		if (isDead_ == false) {
			isDead_ = true;
		}
	}

	//相手がボスの弾
	if (strcmp(toCollisionName, str4) == 0) {
		if (isDead_ == false) {
			isDead_ = true;
		}
	}

}