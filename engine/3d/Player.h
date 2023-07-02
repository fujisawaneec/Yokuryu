﻿#pragma once
#include "Object3d.h"
#include "WinApp.h"
#include "Vector3.h"
#include "input.h"
#include "Spline.h"
#include "Model.h"
#include "Particle.h"
#include "PlayerBullet.h"


class Player : public Object3d
{
public:
	//デストラクタ
	~Player();
	//初期化
	bool PlayerInitialize();

	void Update(Vector3 cameraPos,Vector3 velo);

	void Move();
	///</summary>
	void Attack(Vector3 cameraPos,Vector3 velo);
	void PlayerDraw(ViewProjection* viewProjection_);

	//衝突時コールバック関数
	/*void OnCollision(const CollisionInfo& info) override;
	void OffCollision(const CollisionInfo& info) override;*/


	//hp
	int GetHP() { return hp; }

	//fever
	float GetLen() { return len; }

	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }


private:
	Input* input = nullptr;
	// モデル
	Model* playerModel = nullptr;
	//パーティクル
	Particle* particle = nullptr;
	//弾 
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	//HP
	int hp = 3;
	int coolTime = 0;
	float len = 6;

};
