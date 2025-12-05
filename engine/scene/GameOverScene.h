#pragma once

#include "Input.h"
#include "DirectXCommon.h"
#include "GameBaseScene.h"
#include "Sprite.h"
#include "GameSceneManager.h"
#include "GameTitleScene.h"
#include "Object3d.h"

#include <DirectXMath.h>

class GameOverScene : public GameBaseScene
{
public: // メンバ関数
	// コンストラクタ
	GameOverScene();
	// デストラクタ
	~GameOverScene();

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
	Input* input_ = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Sprite* cross = nullptr;
	//カメラ
	ViewProjection* viewProjection = nullptr;
	//score
	Sprite* sprite = new Sprite;
	SpriteCommon spriteCommon_;
	Sprite clearGH;
	int scores[6];
	float numPosX;
	int showTime;
	//title
	Sprite* title01 = nullptr;
	Sprite* title02 = nullptr;
	//retry
	Sprite* retry01 = nullptr;
	Sprite* retry02 = nullptr;
	bool isNext;
	bool curHit01;
	bool curHit02;
};

