#pragma once

#include "DirectXCommon.h"
#include "Input.h"
#include "Sprite.h"
#include "GameBaseScene.h"
#include "GameSceneManager.h"
#include "GameScene.h"
#include "viewProjection.h"
#include "Player.h"


#include <DirectXMath.h>

class GameTitleScene : public GameBaseScene
{
public: // メンバ関数
	// コンストラクタ
	GameTitleScene();

	// デストラクタ
	~GameTitleScene();

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="dXCommon"></param>
	void Draw() override;

	/// <summary>
	/// 解放
	/// </summary>
	void Finalize() override;

private: // メンバ変数
	// 入力
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;


	//カメラ
	ViewProjection* viewProjection = nullptr;

	//カーソル
	Sprite* sprite = new Sprite;
	SpriteCommon spriteCommon_;
	Sprite title;
	Sprite titleBack[3];
	Sprite cursor[9];
	Sprite click[2];
	Sprite circle;
	Sprite fade;
	Sprite loading;

	Model* skyModel = nullptr;
	Model* seaModel = nullptr;
	Model* block01Model = nullptr;
	Model* block02Model = nullptr;
	Model* stoneModel = nullptr;
	Player* player = nullptr;

	int gameTimer;
	int isBackNum;
	bool onCursor;
	bool isNext;
	float circleSize;
};
