#pragma once
#include "Object3d.h"
#include "WinApp.h"
#include "Vector3.h"
#include "Model.h"
#include <list>
#include "EnemyBullet.h"

//GameSceneの前方宣言
class GameScene;

class Enemy : public Object3d
{
public:
	//デストラクタ
	~Enemy();
	//初期化
	void EnemyInitialize();


	bool GetIsDead() const { return isDead_; }
	
	void Update(Vector3 velo,float t);
	void EnemyDraw(ViewProjection* viewProjection_);

	void Attack();

	void OnCollision(const CollisionInfo& info) override;
	void SetStagePoint(float pos_) { stagePoint = pos_; }
	float GetStagePoint(){return stagePoint;}
	Object3d* GetPointer() { return this; }
	bool GetIsInvisible() { return isInvisible; }

	//弾リストを取得
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullets() { return bullets_; }

private:

	//デスフラグ
	bool isDead_ = false;
	//
	bool isInvisible = true;

	//敵
	std::list<std::unique_ptr<EnemyBullet>> bullets_;

	// モデル
	Model* enemyModel = nullptr;
	float alpha;

	//半径
	int timer;
	float radius = 1.0f;
	int coolTime = 0;
	float stagePoint;

	bool isAttack;
	int timeCount;
};
