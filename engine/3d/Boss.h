#pragma once
#pragma once
#include "Object3d.h"
#include "WinApp.h"
#include "Vector3.h"
#include "Model.h"
#include "BossBullet.h"

//GameSceneの前方宣言
class GameScene;

class Boss : public Object3d
{
public:
	//デストラクタ
	~Boss();
	//初期化
	void BossInitialize();

	void Update(Vector3 velo);
	void Pop();
	void Attack();
	void Move();
	void ChangeState();
	
	void BossDraw(ViewProjection* viewProjection_);

	bool GetIsDead() const { return isDead_; }
	//当たり判定
	void OnCollision(const CollisionInfo& info) override;
	//Getter
	bool GetIsInvisible() { return isInvisible; }
	float GetAlpha() { return bossAlpha; }
	int GetTimer() { return appearTimer; }
	float GetHP() { return hp; }
	int GetPartsNum() { return PARTS_NUM; }
	Object3d* GetParts(int i) { return parts[i]; }
	//演出スキップ
	void SkipMovie();

	

private:

	//デスフラグ
	bool isDead_ = false;
	bool isInvisible = true;
	//タイマー
	int appearTimer = 0;
	float bossAlpha = 0.0f;
	float hp;
	bool isHit;
	int hitTimer;
	int timeCount;
	int timer;
	
	//ボス行動情報
	enum State {
		WAIT,
		SHOT
	};
	State state;

	// モデル
	Model* bossModel = nullptr;
	//弾
	std::list<std::unique_ptr<BossBullet>> bullets_;
	static const int PARTS_NUM = 5;
	Object3d* parts[PARTS_NUM];
};
