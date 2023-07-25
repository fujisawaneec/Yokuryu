#include "GameScene.h"
#include "FbxLoader.h"
#include "FbxObject3d.h"
#include "SphereCollider.h"
#include "CollisionManager.h"


#include <cassert>
#include <fstream>
#include <sstream>




GameScene::GameScene()
{

}

GameScene::~GameScene() {
	delete player;
}

///-----変数の初期化-----///
void GameScene::Initialize(SpriteCommon& spriteCommon) {
	//基盤
	dxCommon = DirectXCommon::GetInstance();
	winApp = WinApp::GetInstance();
	input = Input::GetInstance();
	//当たり判定
	collisionManager = CollisionManager::GetInstance();
	
	//player
	player = new Player;
	player->PlayerInitialize();
	player->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 0.7f));
	//sky
	skyModel = Model::LoadFromOBJ("skydome");
	sky = Object3d::Create();
	sky->SetModel(skyModel);
	sky->SetScale(Vector3(5,5,5));
	sky->SetRotation(Vector3(0, 180, 0));
	//floor
	floorModel = Model::LoadFromOBJ("floor");
	floor = Object3d::Create();
	floor->SetModel(floorModel);
	floor->SetPosition({ 0,-20,0 });
	floor->SetScale(Vector3(500, 500, 500));

	railCamera = new RailCamera;
	railCamera->Initialize(player);

	xmViewProjection = new XMViewProjection;

	//FbxObjectの静的初期化
	//カメラをセット
	FbxObject3d::SetCamera(viewProjection);
	//グラフィックスパイプラインを初期化
	FbxObject3d::CreateGraphicsPipeline();

	// スプライトの初期化
	// スプライト
	sprite = new Sprite();
	spriteCommon_ = sprite->SpriteCommonCreate(dxCommon->GetDevice());

	//titleの画像
	titleGH.LoadTexture(spriteCommon_, 0, L"Resources/title.png", dxCommon->GetDevice());
	titleGH.SpriteCreate(dxCommon->GetDevice(), 50, 50, 0, Vector2(0.0f, 0.0f), false, false);
	titleGH.SetScale(Vector2(1280 * 1, 720 * 1));
	titleGH.SpriteTransferVertexBuffer(titleGH, 0);
	titleGH.SpriteUpdate(titleGH, spriteCommon_);

	//クロスヘアの画像
	crosshair.LoadTexture(spriteCommon_, 1, L"Resources/crosshair.png", dxCommon->GetDevice());
	crosshair.SpriteCreate(dxCommon->GetDevice(), 50, 50, 1, Vector2(0.0f, 0.0f), false, false);
	crosshair.SetPosition(Vector3(1100, 0, 0));
	crosshair.SetScale(Vector2(64 * 1, 64 * 1));
	crosshair.SetRotation(0.0f);
	crosshair.SpriteTransferVertexBuffer(crosshair, 1);
	crosshair.SpriteUpdate(crosshair, spriteCommon_);

	//HP用画像
	for (int i = 0; i < 5; i++) {
		hp[i].LoadTexture(spriteCommon_, 2, L"Resources/hitPoint.png", dxCommon->GetDevice());
		hp[i].SpriteCreate(dxCommon->GetDevice(), 50, 50, 2, Vector2(0.0f, 0.0f), false, false);
		hp[i].SetPosition(Vector3(0 + i * 68.0f, 0, 0));
		hp[i].SetScale(Vector2(64 * 1, 64 * 1));
		hp[i].SetRotation(0.0f);
		hp[i].SpriteTransferVertexBuffer(hp[i], 2);
		hp[i].SpriteUpdate(hp[i], spriteCommon_);
	}

	//gameclearの画像
	clearGH.LoadTexture(spriteCommon_, 3, L"Resources/clear.png", dxCommon->GetDevice());
	clearGH.SpriteCreate(dxCommon->GetDevice(), 1280, 720, 3, Vector2(0.0f, 0.0f), false, false);
	clearGH.SetScale(Vector2(1280 * 1, 720 * 1));
	clearGH.SpriteTransferVertexBuffer(clearGH, 3);
	clearGH.SpriteUpdate(clearGH, spriteCommon_);

	//gameoverの画像
	overGH.LoadTexture(spriteCommon_, 4, L"Resources/over.png", dxCommon->GetDevice());
	overGH.SpriteCreate(dxCommon->GetDevice(), 1280, 720, 4, Vector2(0.0f, 0.0f), false, false);
	overGH.SetScale(Vector2(1280 * 1, 720 * 1));
	overGH.SpriteTransferVertexBuffer(overGH, 4);
	overGH.SpriteUpdate(overGH, spriteCommon_);

	for (int i = 0; i < 10; i++) {
		lock[i].LoadTexture(spriteCommon_, 1, L"Resources/crosshair.png", dxCommon->GetDevice());
		lock[i].SpriteCreate(dxCommon->GetDevice(), 50, 50, 1, Vector2(0.0f, 0.0f), false, false);
		lock[i].SetScale(Vector2(128 * 1, 128 * 1));
		lock[i].SpriteTransferVertexBuffer(lock[i], 1);
		lock[i].SpriteUpdate(lock[i], spriteCommon_);
	}

	//パーティクル初期化
	particle = Particle::LoadParticleTexture("wood.png");
	pm_ = ParticleManager::Create();
	particle_ = Particle::LoadParticleTexture("crosshair.png");
	pm = ParticleManager::Create();
	//オブジェクトにモデルを紐付ける
	pm->SetParticleModel(particle);
	pm_->SetParticleModel(particle_);
	//カメラをセット
	pm->SetXMViewProjection(xmViewProjection);
	pm_->SetXMViewProjection(xmViewProjection);

	//モデル名を指定して読み込み
	obj = new FbxObject3d;
	obj->Initialize();
	model = FbxLoader::GetInstance()->LoadModelFlomFile("cube");
	obj->SetModel(model);
	obj->SetPosition(Vector3(1, 0, -10));
	obj->SetScale(Vector3((float)0.01, (float)0.01, (float)0.01));

	//boss
	boss = new Boss;
	boss->BossInitialize();
	boss->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 10.0f));


	//スプライン制御点の読み込み
	stageNum = 1;
	LoadStage(stageNum);
	LoadEnemy(stageNum);
	//
	isCheckPoint = false;
	isPlayable = true;
	 //
	sceneNum = TITLE;
	infos.clear();
}

///-----更新処理-----///
void GameScene::Update() {
	input->Update();
	Vector3 v = input->GetMousePos();

	//リセット
	if (input->TriggerKey(DIK_R)) {
		Reset();
	}

	Vector3 shotVec = { 0,0,0 };
	switch (sceneNum)
	{
	case GameScene::TITLE:
		if (input->TriggerKey(DIK_SPACE)) {
			sceneNum = GAME;
		}
		break;

	case GameScene::GAME:
		//マウスカーソルの場所にクロスヘアを表示
		crosshair.SetPosition(v - Vector3(32, 32, 0));
		crosshair.SpriteUpdate(crosshair, spriteCommon_);
		crosshair.SpriteTransferVertexBuffer(crosshair, 1);

		//当たり判定チェック
		collisionManager->CheckAllCollisions();

		//boss
		if (railCamera->GetOnRail() == false) {
			if (isCheckPoint == false) {
				isCheckPoint = true;
				boss->Pop();
			}
			else {
				if (boss->GetTimer() > 0) {
					if (isPlayable == true) {
						isPlayable = false;
					}
					railCamera->GetView()->SetTarget(boss->GetPosition());
					railCamera->GetView()->SetEye(Vector3(0, 5, -150));
				}
				else {
					if (isPlayable == false) {
						railCamera->GetView()->SetEye(Vector3(0, 10, -90));
						railCamera->GetCamera()->SetPosition(Vector3(0, 9, -95));
						railCamera->GetCamera()->SetRotation(Vector3(0, 180, 0));
						isPlayable = true;
					}
				}
			}
		}

		//clear
		if (player->GetHP() == 0) {
			sceneNum = OVER;
		}
		//over
		if (boss->GetIsDead() == true) {
			sceneNum = CLEAR;
		}
		//更新
		if (railCamera->GetIsEnd() == false) {
			railCamera->Update(player, points);
		}
		if (player->GetIsHit() == true) {
			railCamera->ShakeCamera();
		}
		if (input->PushMouseLeft()) {
			shotVec = GetScreenToWorldPos(crosshair, railCamera);
		}
		SerchEnemy();
		//ロックオン画像の更新
		for (int i = 0; i < infos.size(); i++) {
			lock[i].SetScale(GetWorldToScreenScale(infos[i].obj, railCamera));
			lock[i].SetPosition(GetWorldToScreenPos(infos[i].obj->GetWorldPos(), railCamera) - (Vector3(lock[i].GetScale().x, lock[i].GetScale().y, 0) / 2));
			lock[i].SpriteUpdate(lock[i], spriteCommon_);
			lock[i].SpriteTransferVertexBuffer(lock[i], 1);
		}
		if (isPlayable == true) {
			player->Update(shotVec,infos);
			LockedClear();
		}
		//デスフラグの立った敵を削除
		enemys_.remove_if([](std::unique_ptr < Enemy>& enemy_) {
			return enemy_->GetIsDead();
			});
		//敵キャラの更新
		for (const std::unique_ptr<Enemy>& enemy : enemys_) {
			enemy->Update(player->GetWorldPos(), railCamera->GetPasPoint());
		}
		sky->Update();
		floor->Update();
		pm->Update();
		pm_->Update();
		obj->Update();
		boss->Update();
		break;

	case GameScene::CLEAR:
		if (input->TriggerKey(DIK_SPACE)) {
			Reset();
			sceneNum = TITLE;
		}
		break;

	case GameScene::OVER:
		if (input->TriggerKey(DIK_SPACE)) {
			Reset();
			sceneNum = TITLE;
		}
		break;
	}
}

void GameScene::Draw() {
#pragma region 3Dオブジェクト描画

// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());

	if (sceneNum == GAME) {
		sky->Draw(railCamera->GetView());
		floor->Draw(railCamera->GetView());
		//敵キャラの描画
		for (const std::unique_ptr<Enemy>& enemy : enemys_) {
			enemy->EnemyDraw(railCamera->GetView());
		}
		boss->Draw(railCamera->GetView(), boss->GetAlpha());
		player->PlayerDraw(railCamera->GetView());
	}

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

#pragma endregion

#pragma region FBX3Dオブジェクト描画

	// 3Dオブジェクト描画前処理
	FbxObject3d::PreDraw(dxCommon->GetCommandList());

	/*obj->Draw(railCamera->GetView());*/

	// 3Dオブジェクト描画後処理
	FbxObject3d::PostDraw();

#pragma endregion

#pragma region パーティクル描画

	// パーティクル描画前処理
	ParticleManager::PreDraw(dxCommon->GetCommandList());

	///==== パーティクル描画 ====///
	//パーティクル
	/*pm->Draw();
	pm_->Draw();*/

	// パーティクル描画後処理
	ParticleManager::PostDraw();

#pragma endregion

#pragma region スプライト描画

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList(), spriteCommon_);

	///=== スプライト描画 ===///
	switch (sceneNum)
	{
	case GameScene::TITLE:
		titleGH.SpriteDraw(dxCommon->GetCommandList(), spriteCommon_, dxCommon->GetDevice());
		break;

	case GameScene::GAME:
		for (int i = 0; i < player->GetHP(); i++) {
			hp[i].SpriteDraw(dxCommon->GetCommandList(), spriteCommon_, dxCommon->GetDevice());
		}
		if (isPlayable == true) {
			crosshair.SpriteDraw(dxCommon->GetCommandList(), spriteCommon_, dxCommon->GetDevice());
		}
		for (int i = 0; i < infos.size(); i++) {
			lock[i].SpriteDraw(dxCommon->GetCommandList(), spriteCommon_, dxCommon->GetDevice());
		}
		break;

	case GameScene::CLEAR:
		clearGH.SpriteDraw(dxCommon->GetCommandList(), spriteCommon_, dxCommon->GetDevice());
		break;

	case GameScene::OVER:
		overGH.SpriteDraw(dxCommon->GetCommandList(), spriteCommon_, dxCommon->GetDevice());
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();
}

void GameScene::LoadStage(int stageNum) {
	points.clear();

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


		// 先頭文字列がｖなら頂点座標
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

	boss = new Boss;
	boss->BossInitialize();
	boss->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 10.0f));

	railCamera = new RailCamera;
	railCamera->Initialize(player);
	LoadEnemy(stageNum);
	isCheckPoint = false;
	isPlayable = true;
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
	if (input->PushKey(DIK_LSHIFT)) {
		Vector3 epos1 = GetWorldToScreenPos(boss->GetWorldPos(), railCamera);
		Vector3 cur = input->GetMousePos();
		if (pow((epos1.x - cur.x), 2) + pow((epos1.y - cur.y), 2) < 30) {
			if (boss->GetIsLocked() == false && infos.size() < 10) {
				LockInfo info;
				info.vec = boss->GetWorldPos();
				info.obj = boss;
				infos.push_back(info);
				boss->SetIsLocked(true);
			}
		}

		for (const std::unique_ptr<Enemy>& enemy : enemys_) {
			Vector3 epos2 = GetWorldToScreenPos(enemy->GetWorldPos(), railCamera);
			Vector3 len = enemy->GetWorldPos() - player->GetWorldPos();
			float len_ = len.length();
			if (pow((epos2.x - cur.x), 2) + pow((epos2.y - cur.y), 2) < (85 - len_)) {
				if (enemy->GetIsLocked() == false && infos.size() < 10) {
					LockInfo info;
					info.vec = enemy->GetWorldPos();
					info.obj = enemy->GetPointer();
					infos.push_back(info);
					enemy->SetIsLocked(true);
				}
			}
		}
		
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
	}
}

Vector3 GameScene::GetScreenToWorldPos(Sprite& sprite_, RailCamera* rail)
{
	if (rail == nullptr) {
		return Vector3(0,0,1);
	}

	//ビューポート行列生成
	Matrix4 viewPort = viewPort.ViewPortMat(WinApp::window_width,WinApp::window_height,Vector2(0.0f,0.0f));

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
	const float kDistanceTestObject = 0.01f;

	Vector3 pos = player->GetWorldPos();
	Vector3 translate = (posFar - posNear) * kDistanceTestObject;

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


	return Vector2(x / len, y /len);
}