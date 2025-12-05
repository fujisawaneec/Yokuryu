#include "BaseScene.h"

//void BaseScene::Initialize()
//{
//	// 基底クラスの初期化
//	Framework::Initialize();
//
//	dXCommon = DirectXCommon::GetInstance();
//
//	// ゲームシーンの生成と初期化
//	scene_ = new GameScene();
//	scene_->Initialize(spriteCommon);
//}
//
//void BaseScene::Update()
//{
//	// 基底クラスの更新処理
//	Framework::Update();
//
//	// ゲームシーンの更新
//	scene_->Update();
//
//}
//
//void BaseScene::Draw()
//{
//#pragma region ゲームシーン描画
//
//	//=== ゲームシーン描画 ===//
//	scene_->Draw();
//
//#pragma endregion
//
//#pragma region 描画
//	// 描画前処理
//	dXCommon->PreDraw();
//
//	// 描画後処理
//	dXCommon->PostDraw();
//#pragma endregion
//}
//
//void BaseScene::Finalize()
//{
//	// ゲームシーンの解放
//	delete scene_;
//
//	// 基底クラスの終了処理
//	Framework::Finalize();
//}
