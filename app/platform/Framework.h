#pragma once

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "Model.h"
#include "Object3d.h"
#include "GameScene.h"
#include "ParticleManager.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include"WinApp.h"
#include"DirectXCommon.h"

#include<windows.h>
#include<cassert>
#include<vector>
#include<string>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<dinput.h>
#pragma warning(push)
#pragma warning(disable:26813)
#include<DirectXTex.h>
#pragma warning(pop)
#include "PostEffect.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")

class Framework
{

public:
	// 実行
	void Run();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 更新
	void Draw();

	// 終了
	void Finalize();

public:
	//　終了チェック
	bool IsEndRequest() { return endRequest_; }

protected:
	WinApp* winApp = nullptr;
	DirectXCommon* dXCommon = nullptr;
	Input* input = nullptr;
	SpriteCommon spriteCommon;
	GameScene* gameScene = nullptr;
	PostEffect postEffect;
	ParticleManager* particleManager = nullptr;

private:
	// ゲーム終了フラグ
	bool endRequest_ = false;
};
