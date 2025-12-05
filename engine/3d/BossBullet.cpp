#include "BossBullet.h"
#include "BaseCollider.h"

void BossBullet::BulletInitialize(const Vector3& position)
{
	Initialize();

	// OBJからモデルデータを読み込む
	bulletModel = Model::LoadFromOBJ("triangle_mat");
	bulletModel->LoadTexture("Resources/blue.png");
	// 3Dオブジェクト生成
	Create();
	// オブジェクトにモデルをひも付ける
	SetModel(bulletModel);

	//体力指定
	hp = 2;
	hitTime = 0;
	isHit = false;
}

void BossBullet::Update(const Vector3& playerPos_)
{
	//移動ベクトルを計算
	velocity = playerPos_ - GetPosition();
	velocity.normalize();
	//座標を加算
	SetPosition(GetPosition() + velocity);
	//更新
	worldTransform_.UpdateMatrix();

	//当たり判定更新
	if (collider)
	{
		collider->Update();
	}
	//ヒット時クールタイム
	if (hitTime > 0) {
		hitTime--;
		if (isHit == false) {
			isHit = true;
			bulletModel->LoadTexture("Resources/red.png");
		}
	}
	else {
		if (isHit == true) {
			isHit = false;
			bulletModel->LoadTexture("Resources/blue.png");
		}
	}
	//時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void BossBullet::OnCollision(const CollisionInfo& info)
{
	//衝突相手の名前
	const char* str1 = "class Player";
	const char* str2 = "class PlayerBullet";

	//相手がplayer
	if (strcmp(toCollisionName, str1) == 0) {
		if (isDead_ == false) {
			isDead_ = true;
		}
	}
	//相手がplayerの弾
	if (strcmp(toCollisionName, str2) == 0) {
		if (hitTime == 0) {
			if (hp == 2) {
				hp--;
			}
			else {
				isDead_ = true;
				hitTime = 10;
			}
		}
	}
}
