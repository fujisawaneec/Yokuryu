#include "GameScene.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "GameSceneManager.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <map>

std::list<std::unique_ptr<Energy>> GameScene::energys_;
int GameScene::popEnergyCount = 0;
GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player;
}

///-----変数の初期化-----///
void GameScene::Initialize() {
	//基盤
	dxCommon_ = DirectXCommon::GetInstance();
	winApp = WinApp::GetInstance();
	input = Input::GetInstance();
	//当たり判定
	collisionManager = CollisionManager::GetInstance();

	//player
	player = new Player;
	player->PlayerInitialize();
	player->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 0.7f));

	//モデル読み込み
	skyModel = Model::LoadFromOBJ("sky");
	seaModel = Model::LoadFromOBJ("sea");
	block01Model = Model::LoadFromOBJ("block01");
	block02Model = Model::LoadFromOBJ("block02");
	stoneModel = Model::LoadFromOBJ("stone");
	models.insert(std::make_pair("sky", skyModel));
	models.insert(std::make_pair("sea", seaModel));
	models.insert(std::make_pair("block01", block01Model));
	models.insert(std::make_pair("block02", block02Model));
	models.insert(std::make_pair("stone", stoneModel));
	//レベルエディタ読み込み
	LoadObjFromLevelEditor("scene");

	railCamera = new RailCamera;
	railCamera->Initialize(player);

	xmViewProjection = new XMViewProjection;

	////FbxObjectの静的初期化
	////カメラをセット
	//FbxObject3d::SetCamera(viewProjection);
	////グラフィックスパイプラインを初期化
	//FbxObject3d::CreateGraphicsPipeline();

	// スプライトの初期化
	// スプライト
	sprite = new Sprite();
	spriteCommon_ = sprite->SpriteCommonCreate(dxCommon_->GetDevice());

	//titleの画像
	titleGH.LoadTexture(spriteCommon_, 0, L"Resources/title.png", dxCommon_->GetDevice());
	titleGH.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 0, Vector2(0.0f, 0.0f), false, false);
	titleGH.SetScale(Vector2(1280 * 1, 720 * 1));
	titleGH.SpriteTransferVertexBuffer(titleGH, 0);
	titleGH.SpriteUpdate(titleGH, spriteCommon_);

	//クロスヘアの画像
	for (int i = 0; i < 4; i++) {
		crosshair[i].LoadTexture(spriteCommon_, 1, L"Resources/crosshair.png", dxCommon_->GetDevice());
		crosshair[i].SpriteCreate(dxCommon_->GetDevice(), 1280, 720, 1, Vector2(0.5f, 0.5f), false, false);
		crosshair[i].SetPosition(Vector3(1100, 0, 0));
		crosshair[i].SetScale(Vector2(24.0f * (i + 1.0f), 24.0f * (i + 1.0f)));
		crosshair[i].SpriteTransferVertexBuffer(crosshair[i], 1);
		crosshair[i].SpriteUpdate(crosshair[i], spriteCommon_);
	}

	//HP用画像
	hp.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 2, Vector2(0.0f, 0.0f), false, false);
	hp.SetPosition(Vector3(28, 650, 0));
	hp.SetScale(Vector2(4 * 1, 48 * 1));
	hp.LoadTexture(spriteCommon_, 2, L"Resources/life.png", dxCommon_->GetDevice());
	hp.SpriteTransferVertexBuffer(hp, 2);
	hp.SpriteUpdate(hp, spriteCommon_);

	//gameclearの画像
	clearGH.LoadTexture(spriteCommon_, 3, L"Resources/clear.png", dxCommon_->GetDevice());
	clearGH.SpriteCreate(dxCommon_->GetDevice(), 1280, 720, 3, Vector2(0.0f, 0.0f), false, false);
	clearGH.SetScale(Vector2(1280 * 1, 720 * 1));
	clearGH.SpriteTransferVertexBuffer(clearGH, 3);
	clearGH.SpriteUpdate(clearGH, spriteCommon_);

	//gameoverの画像
	overGH.LoadTexture(spriteCommon_, 4, L"Resources/over.png", dxCommon_->GetDevice());
	overGH.SpriteCreate(dxCommon_->GetDevice(), 1280, 720, 4, Vector2(0.0f, 0.0f), false, false);
	overGH.SetScale(Vector2(1280 * 1, 720 * 1));
	overGH.SpriteTransferVertexBuffer(overGH, 4);
	overGH.SpriteUpdate(overGH, spriteCommon_);

	for (int i = 0; i < 10; i++) {
		lock[i].LoadTexture(spriteCommon_, 1, L"Resources/crosshair.png", dxCommon_->GetDevice());
		lock[i].SpriteCreate(dxCommon_->GetDevice(), 50, 50, 1, Vector2(0.0f, 0.0f), false, false);
		lock[i].SetScale(Vector2(128 * 1, 128 * 1));
		lock[i].SpriteTransferVertexBuffer(lock[i], 1);
		lock[i].SpriteUpdate(lock[i], spriteCommon_);
	}

	//fade
	fade.LoadTexture(spriteCommon_, 5, L"Resources/black.png", dxCommon_->GetDevice());
	fade.SpriteCreate(dxCommon_->GetDevice(), 1280, 720, 5, Vector2(0.0f, 0.0f), false, false);
	fade.SetScale(Vector2(1280 * 1, 720 * 1));
	fade.SpriteTransferVertexBuffer(fade, 5);
	fade.SpriteUpdate(fade, spriteCommon_);
	//alpha
	fadeAlpha = 1.0f;
	fade.SetAlpha(fade, fadeAlpha);
	//boosHP
	bossHP.LoadTexture(spriteCommon_, 6, L"Resources/hp.png", dxCommon_->GetDevice());
	bossHP.SpriteCreate(dxCommon_->GetDevice(), 1280, 720, 6, Vector2(0.0f, 0.5f), false, false);
	bossHP.SetPosition(Vector3(25, 50, 0));
	bossHP.SetScale(Vector2(2 * 1, 48 * 1));
	bossHP.SpriteTransferVertexBuffer(bossHP, 6);
	bossHP.SpriteUpdate(bossHP, spriteCommon_);
	//gage
	gage.LoadTexture(spriteCommon_, 7, L"Resources/green.png", dxCommon_->GetDevice());
	gage.SpriteCreate(dxCommon_->GetDevice(), 1280, 720, 7, Vector2(0.0f, 0.5f), false, false);
	gage.SetPosition(Vector3(28, 641, 0));
	gage.SetScale(Vector2(2 * 1, 18 * 1));
	gage.SpriteTransferVertexBuffer(gage, 7);
	gage.SpriteUpdate(gage, spriteCommon_);
	//
	gageBack.LoadTexture(spriteCommon_, 8, L"Resources/green.png", dxCommon_->GetDevice());
	gageBack.SpriteCreate(dxCommon_->GetDevice(), 1280, 720, 8, Vector2(0.0f, 0.5f), false, false);
	gageBack.SetPosition(Vector3(28, 641, 0));
	gageBack.SetScale(Vector2(2 * 178, 18 * 1));
	gageBack.SetAlpha(gageBack, 0.5f);
	gageBack.SpriteTransferVertexBuffer(gageBack, 8);
	gageBack.SpriteUpdate(gageBack, spriteCommon_);
	//
	hpBack.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 9, Vector2(0.0f, 0.0f), false, false);
	hpBack.SetPosition(Vector3(28, 650, 0));
	hpBack.SetScale(Vector2(396, 48 * 1));
	hpBack.LoadTexture(spriteCommon_, 9, L"Resources/life.png", dxCommon_->GetDevice());
	hpBack.SetAlpha(hpBack, 0.5f);
	hpBack.SpriteTransferVertexBuffer(hpBack, 9);
	hpBack.SpriteUpdate(hpBack, spriteCommon_);
	//フェードアウト
	fadeout.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 10, Vector2(1.0f, 0.0f), false, false);
	fadeout.SetScale(Vector2(1280 * 1, 1120 * 1));
	fadeout.SetPosition({ 0,100,0 });
	fadeout.SetRotation(45);
	fadeout.SpriteTransferVertexBuffer(fadeout, 10);
	fadeout.SpriteUpdate(fadeout, spriteCommon_);
	fadeout.LoadTexture(spriteCommon_, 10, L"Resources/fade.png", dxCommon_->GetDevice());

	//パーティクル初期化
	particle = Particle::LoadParticleTexture("blue.png");
	pm_ = ParticleManager::Create();
	particle_ = Particle::LoadParticleTexture("crosshair.png");
	pm = ParticleManager::Create();
	//オブジェクトにモデルを紐付ける
	pm->SetParticleModel(particle);
	pm_->SetParticleModel(particle_);
	//カメラをセット
	pm->SetXMViewProjection(xmViewProjection);
	pm_->SetXMViewProjection(xmViewProjection);

	////モデル名を指定して読み込み
	//obj = new FbxObject3d;
	//obj->Initialize();
	//model = FbxLoader::GetInstance()->LoadModelFlomFile("cube");
	//obj->SetModel(model);
	//obj->SetPosition(Vector3(1, 0, -10));
	//obj->SetScale(Vector3((float)0.01, (float)0.01, (float)0.01));

	//boss
	boss = new Boss;
	boss->BossInitialize();
	boss->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 10.0f));


	//スプライン制御点の読み込み
	stageNum = 1;
	LoadStage(stageNum);
	LoadEnemy(stageNum);
	//変数
	isCheckPoint = false;
	isPlayable = false;
	gameState = MAIN;
	infos.clear();
	gameTime = 150;
	cursorRotate = 0.001f;
	bossPass = 0;
	cameraTmpPos = { 0,0,0 };
	cameraTmpRot = { 0,0,0 };
	isStart = false;
}

///-----更新処理-----///
void GameScene::Update() {
	//クロスヘアを更新
	GetCrosshair();
	//リセット
	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		Reset();
		gameState = MAIN;
		gameTime = 150;
		railCamera->GetView()->SetEye(Vector3(-1, 0.5f, 490.0f));
		railCamera->GetView()->SetTarget(Vector3(0.0f, 0.5f, 495));
		player->SetPosition({ 0,0.5f,495 });
		Vector3 cursor = GetWorldToScreenPos(Vector3(0, 0, 0), railCamera);
		input->SetMousePos({ cursor.x,cursor.y });
	}
	if (isStart == false) {
		fadeAlpha = 0.0f;
		fade.SetAlpha(fade, fadeAlpha);
		gameTime = 150;
		railCamera->GetView()->SetEye(Vector3(-1, 0.5f, 490.0f));
		railCamera->GetView()->SetTarget(Vector3(0.0f, 0.5f, 495));
		player->SetPosition({ 0,0.5f,495 });
		Vector3 cursor = GetWorldToScreenPos(Vector3(-230, 85, 0), railCamera);
		input->SetMousePos({ cursor.x,cursor.y });
		for (auto& object : objects) {
			object->Update();
		}
		isStart = true;
	}
	//メインゲーム開始時フェードアウト
	//if (fadeout.GetPosition().y < 400) {
	//	fadeout.SetPosition(fadeout.GetPosition() + Vector3(0, 40, 0));
	//	fadeout.SpriteUpdate(fadeout, spriteCommon_);
	//}

	Vector3 shotVec = { 0,0,0 };


  //操作可能状態ならHPを更新
	if (isPlayable == true) {
		//playerhp
		float playerHp_ = player->GetHP() - (hp.GetScale().x / 4);
		if (playerHp_ > 0) {
			if ((playerHp_) > 4) {
				hp.SetScale(hp.GetScale() + Vector2(16.0f, 0.0f));
			}
			else if ((playerHp_) > 2) {
				hp.SetScale(hp.GetScale() + Vector2(8.0f, 0.0f));
			}
			else if ((playerHp_) > 1) {
				hp.SetScale(hp.GetScale() + Vector2(4.0f, 0.0f));
			}
		}
		else if (playerHp_ < 0) {
			if ((playerHp_) < 4) {
				hp.SetScale(hp.GetScale() - Vector2(8.0f, 0.0f));
			}
			else if ((playerHp_) < 2) {
				hp.SetScale(hp.GetScale() - Vector2(4.0f, 0.0f));
			}
			else if ((playerHp_) < 1) {
				hp.SetScale(hp.GetScale() - Vector2(2.0f, 0.0f));
			}
		}

		hp.SpriteTransferVertexBuffer(hp, 2);
		hp.SpriteUpdate(hp, spriteCommon_);
		//gage
		float gage_ = player->GetEnergy() - (gage.GetScale().x / 4);
		if (gage_ > 0) {
			if (player->GetEnergy() < 100) {
				if ((gage_) > 4) {
					gage.SetScale(gage.GetScale() + Vector2(16.0f, 0.0f));
				}
				else if ((gage_) > 2) {
					gage.SetScale(gage.GetScale() + Vector2(8.0f, 0.0f));
				}
				else if ((gage_) > 1) {
					gage.SetScale(gage.GetScale() + Vector2(4.0f, 0.0f));
				}
			}
		}
		else if (gage_ < 0) {
			if ((gage_) < 4) {
				gage.SetScale(gage.GetScale() - Vector2(8.0f, 0.0f));
			}
			else if ((gage_) < 2) {
				gage.SetScale(gage.GetScale() - Vector2(4.0f, 0.0f));
			}
			else if ((gage_) < 1) {
				gage.SetScale(gage.GetScale() - Vector2(2.0f, 0.0f));
			}
		}

		gage.SpriteTransferVertexBuffer(gage, 7);
		gage.SpriteUpdate(gage, spriteCommon_);
	}

	//メインゲーム中
	switch (gameState)
	{
		//メインゲーム
	case GameScene::MAIN:
		//ゲームスタート時演出
		if (gameTime > 0) {
			//SPACEで演出スキップ
			if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
				gameTime = 1;
			}
			railCamera->GetView()->SetEye(railCamera->GetView()->GetEye() + Vector3(0, 0.0f, 0.05f));
			gameTime--;
			if (gameTime <= 0) {
				player->SetPosition(Vector3(0, -1.0f, -5.5f));
				player->SetAlpha(0.0f);
			}
			player->worldTransform_.UpdateMatrix();
		}
		//ゲーム中
		if (gameTime == 0) {
			//操作不可状態を解除
			if (isPlayable == false) {
				isPlayable = true;
			}
			//boss戦へ
			if (railCamera->GetOnRail() == false) {
				if (isCheckPoint == false) {
					isCheckPoint = true;
					boss->Pop();
					gameState = BOSS;
					delete railCamera;
					railCamera = new RailCamera;
					railCamera->Initialize(player);
				}
			}
			/////デバック用(ボスまでスキップ)/////
			if (input->TriggerKey(DIK_B)) {
				railCamera->SetOnRail(false);
			}

			//player更新(カメラの前)
			if (input->PushMouseLeft()) {
				shotVec = GetScreenToWorldPos(crosshair[0], railCamera);
			}
			if (isPlayable == true) {
				SerchEnemy();
				player->Update(shotVec, infos);
				if (gameState == BOSS && railCamera->GetOnRail() == true) {
					player->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
				}
				LockedClear();
			}
			//更新
			railCamera->Update(player, points);
			//ダメージをくらったときに画面シェイク
			if (player->GetIsHit() == true) {
				railCamera->ShakeCamera(-0.2f, 0.2f);
			}
		}
		break;
		//ボス戦
	case GameScene::BOSS:
		if (boss->GetTimer() > 0) {
			player->SetIsHit(false);
			//playerを操作不可に
			if (isPlayable == true) {
				isPlayable = false;
				railCamera->GetView()->SetEye(Vector3(-40, 55, -150));
			}
			//SPACEで演出スキップ
			if (input->TriggerKey(DIK_SPACE)) {
				boss->SkipMovie();
			}
			//演出
			railCamera->GetView()->SetTarget(boss->GetPosition());
			if (boss->GetTimer() == 150) {
				railCamera->GetView()->SetEye(Vector3(-80, 55, -300));
			}
			else if (boss->GetTimer() < 150) {
				railCamera->GetView()->SetEye(railCamera->GetView()->GetEye() + Vector3(0.5f, 0.0f, 0.05f));
			}
		}
		else {
			//操作可能状態に
			if (isPlayable == false) {
				railCamera->GetView()->SetEye(Vector3(0, 60, -95));
				railCamera->GetView()->SetTarget(Vector3(0, 52, -200));
				railCamera->GetCamera()->SetPosition(Vector3(0, 59, -100));
				railCamera->GetCamera()->SetRotation(Vector3(0, 180, 0));
				player->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
				fadeAlpha = 1.0f;
				fade.SetAlpha(fade, fadeAlpha);
				isPlayable = true;
			}
			//BossHP
			float bossHp_ = boss->GetHP() - (bossHP.GetScale().x / 4);
			if (bossHp_ > 0) {
				if ((bossHp_) > 4) {
					bossHP.SetScale(bossHP.GetScale() + Vector2(16.0f, 0.0f));
				}
				else if ((bossHp_) > 2) {
					bossHP.SetScale(bossHP.GetScale() + Vector2(8.0f, 0.0f));
				}
				else if ((bossHp_) > 1) {
					bossHP.SetScale(bossHP.GetScale() + Vector2(4.0f, 0.0f));
				}
			}
			else if (bossHp_ < 0) {
				if ((bossHp_) < 4) {
					bossHP.SetScale(bossHP.GetScale() - Vector2(8.0f, 0.0f));
				}
				else if ((bossHp_) < 2) {
					bossHP.SetScale(bossHP.GetScale() - Vector2(4.0f, 0.0f));
				}
				else if ((bossHp_) < 1) {
					bossHP.SetScale(bossHP.GetScale() - Vector2(2.0f, 0.0f));
				}
			}
			bossHP.SpriteTransferVertexBuffer(bossHP, 6);
			bossHP.SpriteUpdate(bossHP, spriteCommon_);
			//player更新(カメラの前)
			if (input->PushMouseLeft()) {
				shotVec = GetScreenToWorldPos(crosshair[0], railCamera);
			}
			if (isPlayable == true) {
				SerchEnemy();
				player->Update(shotVec, infos);
				if (gameState == BOSS && railCamera->GetOnRail() == true) {
					player->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
				}
				LockedClear();
			}

			//カメラ更新
			if (railCamera->GetOnRail() == false) {
				gameTime++;
				if (gameTime == 300) {
					railCamera->SetOnRail(true);
					gameTime = 0;
				}
			}
			railCamera->Update(player, bossPoint);
			railCamera->GetView()->SetTarget(boss->GetPosition());
			railCamera->GetCamera()->SetRotation(railCamera->GetView()->GetTarget());
			//カメラ制御
			if (bossPass == 0) {
				if (railCamera->GetPasPoint() + 1.0f > 3.0f) {
					railCamera->SetOnRail(false);
					bossPass = 1;
				}
			}
			else if (bossPass == 1) {
				if (railCamera->GetPasPoint() + 1.0f > 5.0f) {
					railCamera->SetOnRail(false);
					bossPass = 2;
				}
			}
			else if (bossPass == 2) {
				if (railCamera->GetPasPoint() + 1.0f > 7.0f) {
					railCamera->SetOnRail(false);
					bossPass = 3;
				}
			}
			else if (bossPass == 3) {
				if (railCamera->GetPasPoint() + 1.0f >= 8.96f) {
					railCamera->SetOnRail(false);
					railCamera->RailReset();
					bossPass = 0;
				}
			}
		}
		//fadein
		if (fadeAlpha > 0.0f) {
			fadeAlpha -= 0.005f;
			fade.SetAlpha(fade, fadeAlpha);
		}
		//gameclear
		if (boss->GetIsDead() == true) {
			LockedClear();
			GameSceneManager::GetInstance()->ChangeScene("CLEAR");
		}
		//更新
		boss->Update(player->GetWorldPos());
		//ダメージをくらったときに画面シェイク
		if (player->GetIsHit() == true) {
			railCamera->ShakeCamera(-2.0f, 2.0f);
		}
		break;
	case GameScene::ULT:
		if (isPlayable == true) {
			isPlayable = false;
		}
		player->Ultimate();
		railCamera->SetTarget(player->GetWorldPos());
		if (player->GetIsUltimate() == false) {
			player->BackRail();
			railCamera->SetEye(cameraTmpPos);
			railCamera->SetTarget(cameraTmpRot);
			gameState = gameState_bak;
		}
		break;
	}
	//////////////操作可能なら更新///////////////////
	if (player->GetIsUltimate() == true && gameState != ULT) {
		cameraTmpPos = railCamera->GetView()->GetEye();
		cameraTmpRot = railCamera->GetView()->GetTarget();
		railCamera->SetEye(player->GetWorldPos() + Vector3(-2, 0, -3));
		railCamera->SetTarget(player->GetWorldPos());
		gameState_bak = gameState;
		gameState = ULT;
	}
	//デスフラグの立った敵を削除
	enemys_.remove_if([](std::unique_ptr < Enemy>& enemy_) {
		return enemy_->GetIsDead();
		});
	//敵キャラの更新
	for (const std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Update(player->GetWorldPos(), railCamera->GetPasPoint());
	}
	//必殺技エネルギー
	for (const std::unique_ptr<Energy>& energy : energys_) {
		energy->Update(player->GetWorldPos(), railCamera->GetCamera()->GetRotation());
	}
	//デスフラグの立った敵を削除
	energys_.remove_if([](std::unique_ptr <Energy>& energys_) {
		return energys_->GetIsDead();
		});
	//gameover
	if (player->GetHP() == 0) {
		LockedClear();
		GameSceneManager::GetInstance()->ChangeScene("OVER");
	}
	//当たり判定チェック
	collisionManager->CheckAllCollisions();
}

void GameScene::Draw() {
#pragma region 3Dオブジェクト描画

	dxCommon_->PreDraw();

	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//背景オブジェクト
	for (auto& object : objects) {
		object->Draw(railCamera->GetView());
	}
	//敵キャラの描画
	for (const std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->EnemyDraw(railCamera->GetView());
	}
	for (const std::unique_ptr<Energy>& energy : energys_) {
		energy->Draw(railCamera->GetView());
	}
	//ボス
	if (gameState == BOSS) {
		boss->BossDraw(railCamera->GetView());
	}
	//プレイヤー
	player->PlayerDraw(railCamera->GetView());

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

#pragma endregion

#pragma region FBX3Dオブジェクト描画

	//// 3Dオブジェクト描画前処理
	//FbxObject3d::PreDraw(dxCommon_->GetCommandList());

	//// 3Dオブジェクト描画後処理
	//FbxObject3d::PostDraw();

#pragma endregion

#pragma region パーティクル描画

	// パーティクル描画前処理
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	///==== パーティクル描画 ====///
	//パーティクル
	/*pm->Draw();
	pm_->Draw();*/

	// パーティクル描画後処理
	ParticleManager::PostDraw();

#pragma endregion

#pragma region スプライト描画

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList(), spriteCommon_);

	///=== スプライト描画 ===///

	if (gameState == BOSS) {
		fade.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		if (boss->GetTimer() == 0) {
			//ボスのHP
			bossHP.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		}
	}
	if (isPlayable == true) {
		hpBack.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		gageBack.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		hp.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		gage.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
	}
	if (isPlayable == true) {
		for (int i = 0; i < 4; i++) {
			crosshair[i].SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		}
	}
	for (int i = 0; i < infos.size(); i++) {
		lock[i].SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
	}
	fadeout.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());

	// スプライト描画後処理
	Sprite::PostDraw();

	dxCommon_->PostDraw();
}

void GameScene::LoadStage(int stageNum) {
	points.clear();
	bossPoint.clear();

	//ファイルを開く
	std::ifstream file;
	file.open("Resources/csv/stagePop.csv");
	assert(file.is_open());

	HRESULT result = S_FALSE;

	std::string num;
	num = stageNum + 48;

	// １行ずつ読み込む
	string line;
	while (getline(file, line)) {

		// １行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スパース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');


		// 先頭文字列がstならステージ
		if (key == "st" + num) {
			// X,Y,Z座標読み込み
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			// 座標データに追加
			points.emplace_back(position);
		}
		if (stageNum == 10) {
			if (key == "st10") {
				// X,Y,Z座標読み込み
				Vector3 position{};
				line_stream >> position.x;
				line_stream >> position.y;
				line_stream >> position.z;
				// 座標データに追加
				points.emplace_back(position);
			}
		}
		else if (stageNum > 10) {
			if (key == "st1" + stageNum - 10) {
				// X,Y,Z座標読み込み
				Vector3 position{};
				line_stream >> position.x;
				line_stream >> position.y;
				line_stream >> position.z;
				// 座標データに追加
				points.emplace_back(position);
			}
		}
		//先頭文字がboならボス
		if (key == "bo" + num) {
			// X,Y,Z座標読み込み
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			// 座標データに追加
			bossPoint.emplace_back(position);
		}
		if (stageNum == 10) {
			if (key == "st10") {
				// X,Y,Z座標読み込み
				Vector3 position{};
				line_stream >> position.x;
				line_stream >> position.y;
				line_stream >> position.z;
				// 座標データに追加
				bossPoint.emplace_back(position);
			}
		}
		else if (stageNum > 10) {
			if (key == "st1" + stageNum - 10) {
				// X,Y,Z座標読み込み
				Vector3 position{};
				line_stream >> position.x;
				line_stream >> position.y;
				line_stream >> position.z;
				// 座標データに追加
				bossPoint.emplace_back(position);
			}
		}
	}
	// ファイルと閉じる
	file.close();
}

void GameScene::Reset() {
	delete player;
	delete railCamera;
	delete enemy;
	delete boss;

	//player
	player = new Player;
	player->PlayerInitialize();
	player->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 0.7f));
	player->SetPosition({ 0,0.5f,495 });
	hp.SetScale(Vector2(0, 48 * 1));
	//boss
	boss = new Boss;
	boss->BossInitialize();
	boss->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 10.0f));
	bossHP.SetScale(Vector2(12 * 1, 48 * 1));
	//camera
	railCamera = new RailCamera;
	railCamera->Initialize(player);
	//enemy
	LoadEnemy(stageNum);
	//変数
	isCheckPoint = false;
	isPlayable = false;
	cursorRotate = 0.001f;
	LockedClear();
	infos.clear();
	gameState = MAIN;
	gameTime = 150;
	bossPass = 0;
	fadeAlpha = 1.0f;
	fade.SetAlpha(fade, fadeAlpha);
	popEnergyCount = 0;
}

void GameScene::Finalize()
{
}

void GameScene::LoadEnemy(int stageNum) {

	Spline spline;
	spline.Initialize();

	pointsL = points;
	pointsR = points;

	for (int i = 0; i < points.size(); i++)
	{
		pointsL[i] += Vector3(-2, 0, 0);
		pointsR[i] += Vector3(2, 0, 0);
	}

	enemys_.clear();

	//ファイルを開く
	std::ifstream file;
	file.open("Resources/csv/EnemyPop.csv");
	assert(file.is_open());

	HRESULT result = S_FALSE;

	std::string num;
	num = stageNum + 48;

	// １行ずつ読み込む
	string line;
	while (getline(file, line)) {

		// １行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スパース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		string word;
		getline(line_stream, word, ' ');

		if (stageNum < 10) {
			// 先頭文字列がｖなら頂点座標
			if (key == "ea" + num) {
				//敵の生成
				std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
				//敵の初期化
				newEnemy->EnemyInitialize();
				////コライダーの追加
				newEnemy->SetCollider(new SphereCollider());
				// X,Y,Z座標読み込み
				Vector3 position{};
				float t;

				if (word.find("L") == 0)
				{
					line_stream >> t;
					newEnemy->SetStagePoint(t);
					position = spline.EnemyPosition(pointsL, t);
				}
				else if (word.find("M") == 0)
				{
					line_stream >> t;
					newEnemy->SetStagePoint(t);
					position = spline.EnemyPosition(points, t);
				}
				else if (word.find("R") == 0)
				{
					line_stream >> t;
					newEnemy->SetStagePoint(t);
					position = spline.EnemyPosition(pointsR, t);
				}

				// 座標データに追加
				newEnemy->SetPosition(position);
				newEnemy->SetScale({ 0.6f,0.6f,0.6f });
				//登録
				enemys_.push_back(std::move(newEnemy));
			}
		}
	}
	// ファイルと閉じる
	file.close();
}

void GameScene::SerchEnemy()
{
	Vector3 cur = input->GetMousePos();

	if (input->PushKey(DIK_LSHIFT)) {
		for (int i = 0; i < boss->GetPartsNum(); i++) {
			Vector3 epos1 = GetWorldToScreenPos(boss->GetParts(i)->GetWorldPos(), railCamera);
			if (boss->GetIsInvisible() == false) {
				if (pow((epos1.x - cur.x), 2) + pow((epos1.y - cur.y), 2) < pow(50, 2)) {
					if (boss->GetParts(i)->GetIsLocked() == false && infos.size() < 10) {
						LockInfo info;
						info.vec = boss->GetParts(i)->GetWorldPos();
						info.obj = boss->GetParts(i);
						infos.push_back(info);
						boss->GetParts(i)->SetIsLocked(true);
					}
				}
			}
		}

		for (const std::unique_ptr<Enemy>& enemy : enemys_) {
			Vector3 epos2 = GetWorldToScreenPos(enemy->GetWorldPos(), railCamera);
			Vector3 len = enemy->GetWorldPos() - player->GetWorldPos();
			float len_ = len.length();
			if (pow((epos2.x - cur.x), 2) + pow((epos2.y - cur.y), 2) < pow(30, 2)) {
				if (enemy->GetIsLocked() == false && infos.size() < 10) {
					LockInfo info;
					info.vec = enemy->GetWorldPos();
					info.obj = enemy->GetPointer();
					infos.push_back(info);
					enemy->SetIsLocked(true);
				}
			}
		}
		if (cursorRotate < 0.005f) {
			cursorRotate += 0.0001f;
		}
	}
	else {
		if (cursorRotate > 0.001f) {
			cursorRotate -= 0.0001f;
		}
	}
	//ロックオン画像の更新
	for (int i = 0; i < infos.size(); i++) {
		lock[i].SetScale(GetWorldToScreenScale(infos[i].obj, railCamera));
		lock[i].SetPosition(GetWorldToScreenPos(infos[i].obj->GetWorldPos(), railCamera) - (Vector3(lock[i].GetScale().x, lock[i].GetScale().y, 0) / 2));
		lock[i].SpriteUpdate(lock[i], spriteCommon_);
		lock[i].SpriteTransferVertexBuffer(lock[i], 1);
	}
}

void GameScene::LockedClear()
{
	if (player->GetIsShooted() == true) {
		if (boss->GetIsLocked() == true) {
			boss->SetIsLocked(false);
		}

		for (const std::unique_ptr<Enemy>& enemy : enemys_) {
			if (enemy->GetIsLocked() == true) {
				enemy->SetIsLocked(false);
			}
		}
		infos.clear();
	}
}

void GameScene::GetCrosshair()
{
	Vector3 mPos = input->GetMousePos();
	//マウスカーソルの場所にクロスヘアを表示
	if (gameState == MAIN) {
		for (int i = 0; i < 4; i++) {
			if (i == 0) {
				crosshair[i].SetPosition(mPos);
			}
			else if (i == 1) {
				crosshair[i].SetPosition(mPos - (mPos - GetWorldToScreenPos(player->GetWorldPos(), railCamera)) * 0.1f);
			}
			else if (i == 2) {
				crosshair[i].SetPosition(mPos - (mPos - GetWorldToScreenPos(player->GetWorldPos(), railCamera)) * 0.25f);
			}
			else if (i == 3) {
				crosshair[i].SetPosition(mPos - (mPos - GetWorldToScreenPos(player->GetWorldPos(), railCamera)) * 0.55f);
			}
			if (i % 2 == 0) {
				crosshair[i].SetRotation(crosshair[i].GetRotation() + cursorRotate);
			}
			else {
				crosshair[i].SetRotation(crosshair[i].GetRotation() - cursorRotate);
			}
			if (crosshair[i].GetRotation() == 1.0f) {
				crosshair[i].SetRotation(0.0f);
			}
			crosshair[i].SpriteTransferVertexBuffer(crosshair[i], 1);
			crosshair[i].SpriteUpdate(crosshair[i], spriteCommon_);
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			crosshair[i].SetPosition(mPos);
			if (i % 2 == 0) {
				crosshair[i].SetRotation(crosshair[i].GetRotation() + cursorRotate);
			}
			else {
				crosshair[i].SetRotation(crosshair[i].GetRotation() - cursorRotate);
			}
			if (crosshair[i].GetRotation() == 1.0f) {
				crosshair[i].SetRotation(0.0f);
			}
			crosshair[i].SpriteTransferVertexBuffer(crosshair[i], 1);
			crosshair[i].SpriteUpdate(crosshair[i], spriteCommon_);
		}
	}
}

void GameScene::PopEnergy(Vector3 pos_)
{
	//乱数生成装置
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float>dist(-1.5f, 1.5f);
	std::uniform_real_distribution<float>dist2(-2.5f, 2.5f);
	std::uniform_real_distribution<float>dist3(-1.5f, 1.5f);
	//弾を生成し初期化
	std::unique_ptr<Energy> newEnergy = std::make_unique<Energy>();

	//単発													   
	newEnergy->EnergyInitialize();
	newEnergy->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 2.0f));
	newEnergy->SetPosition(pos_ + Vector3(dist(engine), dist2(engine), dist3(engine)));
	energys_.push_back(std::move(newEnergy));
}

Vector3 GameScene::GetScreenToWorldPos(Sprite& sprite_, RailCamera* rail)
{
	if (rail == nullptr) {
		return Vector3(0, 0, 1);
	}

	//ビューポート行列生成
	Matrix4 viewPort = viewPort.ViewPortMat(WinApp::window_width, WinApp::window_height, Vector2(0.0f, 0.0f));

	//ビュープロジェクションビューポート合成行列
	Matrix4 invViewPort = viewPort;
	invViewPort.MakeInverse();
	//プロジェクション行列//
	float fovAngleY = 45.0f * (3.141592f / 180.0f);;
	float aspectRatio = (float)WinApp::window_width / WinApp::window_height;
	//プロジェクション行列生成
	Matrix4 projection = projection.ProjectionMat(fovAngleY, aspectRatio, 0.1f, 200.0f);
	Matrix4 invProjection = projection;
	invProjection.MakeInverse();
	//ビュー行列//
	Matrix4 view = railCamera->GetView()->GetMatView();
	Matrix4 invView = view;
	invView.MakeInverse();
	////合成行列の逆行列を計算する
	Matrix4 matInverseVPV = invViewPort * invProjection * invView;
	//スクリーン座標系からワールド座標系
	Matrix4 mat1, mat2;
	//w除算
	Vector3 posNear = Vector3(sprite_.GetPosition().x, sprite_.GetPosition().y, 0);
	Vector3 posFar = Vector3(sprite_.GetPosition().x, sprite_.GetPosition().y, 1);
	posNear = mat1.transform(posNear, matInverseVPV);
	posFar = mat2.transform(posFar, matInverseVPV);

	//マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = mouseDirection.normalize();
	//カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 0.05f;

	Vector3 pos = player->GetWorldPos();
	Vector3 translate = (posFar - pos) * kDistanceTestObject;

	return translate;
}

Vector3 GameScene::GetWorldToScreenPos(Vector3 pos_, RailCamera* rail)
{
	if (rail == nullptr) {
		return Vector3(0, 0, 0);
	}

	//ビュー行列//
	Matrix4 view = railCamera->GetView()->GetMatView();
	//プロジェクション行列//
	float fovAngleY = 45.0f * (3.141592f / 180.0f);;
	float aspectRatio = (float)WinApp::window_width / WinApp::window_height;
	//プロジェクション行列生成
	Matrix4 projection = projection.ProjectionMat(fovAngleY, aspectRatio, 0.1f, 200.0f);
	//ビューポート行列生成
	Matrix4 viewPort = viewPort.ViewPortMat(WinApp::window_width, WinApp::window_height, Vector2(0.0f, 0.0f));

	Matrix4 matVPV = view * projection * viewPort;

	Matrix4 mat;
	Vector3 posScreen = pos_;
	posScreen = mat.transform(posScreen, matVPV);
	posScreen.z = 0;

	return posScreen;
}

Vector2 GameScene::GetWorldToScreenScale(Object3d* obj, RailCamera* rail)
{
	if (rail == nullptr) {
		return Vector2(0, 0);
	}

	Vector3 v = obj->GetPosition() - rail->GetView()->GetEye();
	v.normalize();
	float len = v.length();

	float x = 64;
	x *= obj->GetScale().x;
	float y = 64;
	y *= obj->GetScale().y;


	return Vector2(x / len, y / len);
}

//void GameScene::LoadObjFromLevelEditor(const std::string& fileName) {
//	JsonLoader* file = nullptr;
//	LevelData* levelData = file->LoadFile(fileName);
//
//	//オブジェクト配置
//	for (auto& objectData : levelData->objects) {
//		//ファイル名から登録済みモデルを検索
//		Model* model = nullptr;
//		decltype(models)::iterator it = models.find(objectData.fileName);
//		if (it != models.end()) { model = it->second; }
//		//モデルを指定して3DObjectを生成
//		Object3d* newObject = Object3d::Create();
//		newObject->Initialize();
//		newObject->SetModel(model);
//		//座標
//		Vector3 pos;
//		pos = Vector3(objectData.translation.x, objectData.translation.y, objectData.translation.z);
//		newObject->SetPosition(pos);
//		//回転角
//		Vector3 rot;
//		rot = Vector3(objectData.rotation.x, objectData.rotation.y, objectData.rotation.z);
//		newObject->SetRotation(rot);
//		//スケール
//		Vector3 scale;
//		scale = Vector3(objectData.scaling.x, objectData.scaling.y, objectData.scaling.z);
//		newObject->SetScale(scale);
//		//配列に登録
//		objects.push_back(newObject);
//	}
//}