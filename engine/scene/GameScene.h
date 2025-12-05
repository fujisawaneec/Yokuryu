#pragma once
#include "Sprite.h"
#include "Object3d.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "Object3d.h"
#include "Input.h"
#include "time.h"
#include <random>
#include "Model.h"
#include "ViewProjection.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "FbxObject3d.h"
#include "FbxModel.h"
#include "Player.h"
#include "RailCamera.h"
#include "Enemy.h"
#include "CollisionManager.h"
#include "Boss.h"
#include "JsonLoader.h"	
#include "energy.h"
#include "GameBaseScene.h"


class GameScene : public GameBaseScene {
public:
	//メンバ関数
	GameScene();

	~GameScene();

	void Initialize() override;
	/*void LoadObjFromLevelEditor(const std::string& fileName);*/

	void Update();

	void Draw();
	void Reset();
	void Finalize()override;

	void LoadStage(int stageNum);
	void LoadEnemy(int stageNum);
	void SerchEnemy();
	void LockedClear();
	void GetCrosshair();
	static void PopEnergy(Vector3 pos_);

	Vector3 GetScreenToWorldPos(Sprite& sprite_, RailCamera* rail);
	Vector3 GetWorldToScreenPos(Vector3 pos_, RailCamera* rail);
	Vector2 GetWorldToScreenScale(Object3d* obj, RailCamera* rail);

private:
	//base
	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp = nullptr;
	//入力のポインタ
	Input* input = nullptr;
	//スプライトのポインタ
	Sprite* sprite = new Sprite;
	SpriteCommon spriteCommon_;
	Sprite titleGH;
	Sprite crosshair[4];
	Sprite hp;
	Sprite clearGH;
	Sprite overGH;
	Sprite lock[10];
	Sprite fade;
	float fadeAlpha;
	Sprite bossHP;
	Sprite gage;
	Sprite gageBack;
	Sprite hpBack;
	Sprite fadeout;
	//オブジェクトのポインタ
	//3Dオブジェクト生成
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	Model* skyModel = nullptr;
	Model* seaModel = nullptr;
	Model* block01Model = nullptr;
	Model* block02Model = nullptr;
	Model* stoneModel = nullptr;
	Boss* boss = nullptr;
	//カメラ
	ViewProjection* viewProjection = nullptr;
	XMViewProjection* xmViewProjection = nullptr;
	RailCamera* railCamera = nullptr;
	Vector3 cameraTmpPos;
	Vector3 cameraTmpRot;
	//パーティクル
	Particle* particle = nullptr;
	ParticleManager* pm = nullptr;
	Particle* particle_ = nullptr;
	ParticleManager* pm_ = nullptr;
	//enemy
	std::list<std::unique_ptr<Enemy>> enemys_;
	//energy
	static std::list<std::unique_ptr<Energy>> energys_;
	static int popEnergyCount;
	//当たり判定
	CollisionManager* collisionManager = nullptr;
	//FBX
	FbxModel* model = nullptr;
	FbxObject3d* obj = nullptr;
	//カメラ移動用レール
	std::vector<Vector3> points{};
	std::vector<Vector3> pointsL;
	std::vector<Vector3> pointsR;
	std::vector<Vector3> bossPoint{};
	//ステージ
	int stageNum = 0;
	bool isCheckPoint = false;
	bool isPlayable = false;
	//ロックオン情報
	std::vector<LockInfo> infos;
	int gameTime;
	float cursorRotate;
	//ゲームステータス
	enum GameState {
		MAIN,
		BOSS,
		ULT
	};
	GameState gameState;
	GameState gameState_bak;
	int bossPass;
	bool isStart;
};