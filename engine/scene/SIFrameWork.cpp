#include "SIFrameWork.h"

void SIFrameWork::Initialize()
{
	// WindowsAPIの初期化
	winApp = WinApp::GetInstance();
	winApp->Initialize();

	// DirectXの初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize(winApp);
	//3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);
	// ビュープロジェクションの初期化
	ViewProjection::StaticInitialize(dxCommon->GetDevice());
	// シーンマネージャの生成
	sceneManager_ = GameSceneManager::GetInstance();
	// パーティクル静的初期化
	ParticleManager::StaticInitialize(dxCommon->GetDevice());
	//fbx汎用初期化
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());
	//
	FbxObject3d::StaticInitialize(dxCommon->GetDevice());

	/*postEffect.Initialize(dxCommon->GetDevice());*/
	
}

void SIFrameWork::Finalize()
{
	sceneManager_->Destroy();
	//シーンファクトリの解放
	delete sceneFactory_;

	// WindowsAPIの終了処理
	winApp->Finalize();

	// DirectX解放
	dxCommon->fpsFixedFinalize();
}

void SIFrameWork::Update()
{
	// Windowsのメッセージ処理
	if (winApp->ProcessMessage()) {
		// ゲームループを抜ける
		endRequest = true;
	}

	// 入力の更新
	input->Update();

	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		endRequest = true;
	}

	// シーンマネージャの更新
	sceneManager_->Update();
}

void SIFrameWork::Run()
{
	// ゲームの初期化
	Initialize();

	while (true) // ゲームループ 
	{
		// 毎フレーム更新
		Update();

		// 終了リクエストが来たらループを抜ける
		if (IsEndRequest()) {
			// ゲームループを抜ける
			break;
		}

		// 描画
		Draw();
	}

	// ゲームの終了
	Finalize();
}
