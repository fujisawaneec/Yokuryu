#pragma once
#include "Object3d.h"
#include "WinApp.h"
#include "Vector3.h"
#include "input.h"
#include "Spline.h"
#include "Model.h"
#include "Particle.h"
#include "PlayerBullet.h"

struct LockInfo {
	Vector3 vec = {0,0,0};
	 Object3d* obj = nullptr;
};

class Player : public Object3d
{
public:
	//デストラクタ
	~Player();
	//初期化
	bool PlayerInitialize();

	void Update(Vector3 velo, std::vector<LockInfo>& info);

	void Move();
	///</summary>
	void Attack(Vector3 velo);
	void LockAttack(std::vector<LockInfo>& info);
	void Ultimate();
	void PlayerDraw(ViewProjection* viewProjection_);
	void BackRail();

	//衝突時コールバック関数
	void OnCollision(const CollisionInfo& info) override;


	/////getter/////
	//hp
	float GetHP() { return hp; }
	float GetEnergy() { return energy; }
	bool GetIsHit() { return isHit; }
	bool GetIsShooted() { return isShooted; }
	float GetLen() { return len; }
	bool GetIsUltimate() { return isUltimate; }
	Vector3 GetMove() { return move; }
	//setter
	void SetIsHit(bool isHit_) { isHit = isHit_; }
	void SetAlpha(float a) { alpha = a; }
	void SetIsUltimate(bool ult) { isUltimate = ult; }

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
	float hp;
	int coolTime = 0;
	float len = 6;
	int pTimer = 0;
	bool isHit;
	bool isShooted;
	int hitTime;
	float alpha;
	float energy;
	bool isUltimate;
	int ultTime;
	Vector3 pos_;
	Vector3 rot_;
	Vector3 move;
};
