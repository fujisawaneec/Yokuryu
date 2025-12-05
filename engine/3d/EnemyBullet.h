#pragma once
#include "Vector3.h"
#include <Model.h>
#include "ViewProjection.h"
#include <Model.h>
#include <WorldTransform.h>
#include <cassert>
#include "Object3d.h"

class EnemyBullet : public Object3d
{
public:
	///< summary>
	///初期化
	///</summary>
	void BulletInitialize(const Vector3& position);

	///< summary>
	///初期化
	///</summary>
	void Update(const Vector3& playerPos_);


	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision(const CollisionInfo& info) override;


public:
	bool IsDead() const { return isDead_; }
	WorldTransform* GetWorldTransForm() { return &worldTransform_; }

private:
	//モデル
	Model* bulletModel = nullptr;

	//速度
	Vector3 playerPos;
	Vector3 velocity;

	//寿命
	static const int32_t kLifeTime = 60 * 3;
	//デスタイマー
	int32_t deathTimer_ = kLifeTime;
	int bulletTime;
	//デスフラグ
	bool isDead_ = false;

};
