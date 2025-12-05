#include "GameTitleScene.h"

using namespace DirectX;

GameTitleScene::GameTitleScene()
{
	//// サウンドの静的初期化
	//Sound::StaticInitialize();
}

GameTitleScene::~GameTitleScene()
{
}

void GameTitleScene::Initialize()
{
	input_ = Input::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	sprite = new Sprite();
	spriteCommon_ = sprite->SpriteCommonCreate(dxCommon_->GetDevice());

	//titleの画像
	title.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 0, Vector2(0.0f, 0.0f), false, false);
	title.SetScale(Vector2(1280 * 1, 720 * 1));
	title.SetPosition({ -354,-32,0 });
	title.SpriteTransferVertexBuffer(title, 0);
	title.SpriteUpdate(title, spriteCommon_);
	title.LoadTexture(spriteCommon_, 0, L"Resources/title.png", dxCommon_->GetDevice());
	//title背景
	for (int i = 0; i < 3; i++) {
		titleBack[i].SpriteCreate(dxCommon_->GetDevice(), 50, 50, i + 1, Vector2(0.0f, 0.0f), false, false);
		titleBack[i].SetScale(Vector2(1280 * 1.1, 720 * 1.7));
		titleBack[i].SetPosition({ -418,-192,0 });
		titleBack[i].SpriteTransferVertexBuffer(titleBack[i], i + 1);
		titleBack[i].SpriteUpdate(titleBack[i], spriteCommon_);
	}
	titleBack[0].LoadTexture(spriteCommon_, 1, L"Resources/titleBack.png", dxCommon_->GetDevice());
	titleBack[1].LoadTexture(spriteCommon_, 2, L"Resources/titleBack2.png", dxCommon_->GetDevice());
	titleBack[2].LoadTexture(spriteCommon_, 3, L"Resources/titleBack3.png", dxCommon_->GetDevice());
	//カーソル
	for (int i = 0; i < 9; i++) {
		cursor[i].SpriteCreate(dxCommon_->GetDevice(), 50, 50, 4, Vector2(0.5f, 0.5f), false, false);
		cursor[i].SetScale(Vector2(48 * 1, 48 * 1));
		cursor[i].SetPosition(Input::GetInstance()->GetMousePos());
		cursor[i].SetAlpha(cursor[i], 1 - (i * 0.1f));
		cursor[i].SpriteTransferVertexBuffer(cursor[i], 4);
		cursor[i].SpriteUpdate(cursor[i], spriteCommon_);
		cursor[i].LoadTexture(spriteCommon_, 4, L"Resources/cursor.png", dxCommon_->GetDevice());
	}
	//クリック
	for (int i = 0; i < 2; i++) {
		click[i].SpriteCreate(dxCommon_->GetDevice(), 50, 50, 5+i, Vector2(0.5f, 0.5f), false, false);
		click[i].SetScale(Vector2(312.0f * 1.2f, 52.0f * 1.2f));
		click[i].SetPosition({ 640,640,0 });
		click[i].SpriteTransferVertexBuffer(click[i], 5+i);
		click[i].SpriteUpdate(click[i], spriteCommon_);
	}
	click[0].LoadTexture(spriteCommon_, 5, L"Resources/click1.png", dxCommon_->GetDevice());
	click[1].LoadTexture(spriteCommon_, 6, L"Resources/click2.png", dxCommon_->GetDevice());
	//カーソルエフェクト
	circle.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 7, Vector2(0.5f, 0.5f), false, false);
	circle.SetScale(Vector2(48 * circleSize, 48 * circleSize));
	circle.SetPosition({ 0,0,0 });
	circle.SpriteTransferVertexBuffer(circle, 7);
	circle.SpriteUpdate(circle, spriteCommon_);
	circle.LoadTexture(spriteCommon_, 7, L"Resources/circle.png", dxCommon_->GetDevice());
	//フェードイン
	fade.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 8, Vector2(0.0f, 0.0f), false, false);
	fade.SetScale(Vector2(1280 * 1, 1120 * 1));
	fade.SetPosition({ 0,-1120,0 });
	fade.SpriteTransferVertexBuffer(fade, 8);
	fade.SpriteUpdate(fade, spriteCommon_);
	fade.LoadTexture(spriteCommon_, 8, L"Resources/fade.png", dxCommon_->GetDevice());
	//loading
	loading.SpriteCreate(dxCommon_->GetDevice(), 50, 50, 9, Vector2(0.0f, 0.0f), false, false);
	loading.SetScale(Vector2(256 * 1, 48 * 1));
	loading.SetPosition({ 1000,600,0 });
	loading.SpriteTransferVertexBuffer(loading, 9);
	loading.LoadTexture(spriteCommon_, 9, L"Resources/loading.png", dxCommon_->GetDevice());


	//player
	player = new Player;
	player->PlayerInitialize();
	player->SetPosition({ 0.0f,33.3f,288.0f });
	player->SetRotation({ 0,90,0 });

	//カメラ初期化
	viewProjection = new ViewProjection();
	viewProjection->Initialize();
	viewProjection->SetEye({ 3.5f,34.5f,298.0f });
	viewProjection->SetTarget({0,40,-400});

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
	LoadObjFromLevelEditor("title");
	gameTimer = 0;
	isBackNum = 0;
	onCursor = false;
	isNext = false;
	circleSize = 1.0f;
}

void GameTitleScene::Update()
{
	if (isNext == false) {
		Vector3 cur = Input::GetInstance()->GetMousePos();
		cursor[8].SetPosition(cursor[7].GetPosition());
		cursor[7].SetPosition(cursor[6].GetPosition());
		cursor[6].SetPosition(cursor[5].GetPosition());
		cursor[5].SetPosition(cursor[4].GetPosition());
		cursor[4].SetPosition(cursor[3].GetPosition());
		cursor[3].SetPosition(cursor[2].GetPosition());
		cursor[2].SetPosition(cursor[1].GetPosition());
		cursor[1].SetPosition(cursor[0].GetPosition());
		cursor[0].SetPosition(cur);
		circle.SetPosition(cur);
		if (Input::GetInstance()->TriggerMouseLeft() == true) {
			circleSize = 1.0f;
		}
		else if (circleSize < 6.0f) {
			circleSize += 0.1f;
		}
		else {
			circleSize = 1.0f;
		}
		circle.SetScale(Vector2(64 * circleSize, 64 * circleSize));
		circle.SpriteTransferVertexBuffer(circle, 7);
		circle.SpriteUpdate(circle, spriteCommon_);
		for (int i = 0; i < 9; i++) {
			cursor[i].SpriteUpdate(cursor[i], spriteCommon_);
		}
		//クリック判定
		if (cur.x > click[0].GetPosition().x - 156 && cur.x < click[0].GetPosition().x + 156) {
			if (cur.y > click[0].GetPosition().y - 26 && cur.y < click[0].GetPosition().y + 26) {
				if (onCursor == false) {
					onCursor = true;
				}
				if (Input::GetInstance()->TriggerMouseLeft()) {
					isNext = true;
					gameTimer = 0;
				}
			}
			else {
				if (onCursor == true) {
					onCursor = false;
				}
			}
		}
		else {
			if (onCursor == true) {
				onCursor = false;
			}
		}
		//タイトル演出
		if (gameTimer % 8 == 0) {
			if (isBackNum < 2) {
				isBackNum++;
			}
			else if (isBackNum == 2) {
				isBackNum = 0;
			}
		}
		title.SpriteUpdate(title, spriteCommon_);
		for (int i = 0; i < 3; i++) {
			titleBack[i].SpriteUpdate(titleBack[i], spriteCommon_);
		}
		//クリック
		for (int i = 0; i < 2; i++) {
			click[i].SpriteUpdate(click[i], spriteCommon_);
		}

		//更新
		for (auto& object : objects) {
			object->Update();
		}
		if (gameTimer < 50) {
			player->SetPosition(player->GetPosition() + Vector3(0, 0.005f, 0));
		}
		else if (gameTimer < 100) {
			player->SetPosition(player->GetPosition() + Vector3(0, -0.005f, 0));
		}
		gameTimer++;
		if (gameTimer > 100) {
			gameTimer = 0;
		}
	}
	else {
		//シーン遷移
		if (gameTimer < 10) {
			player->SetPosition(player->GetPosition() + Vector3(0, 0.0075f, 0));
		}
		else if (gameTimer < 25) {
			player->SetPosition(player->GetPosition() + Vector3(0, -0.01f, 0));
		}
		else if (gameTimer < 75) {
			player->SetPosition(player->GetPosition() + Vector3(0, -0.0025f, 0));
		}
		else if(player->GetPosition().y < 60.0f) {
			player->SetPosition(player->GetPosition() + Vector3(0, 0.8f, 0));
			viewProjection->SetTarget(viewProjection->GetTarget() + Vector3(0,3.0f,0));
		}
		else if (fade.GetPosition().y < 0) {
			fade.SetPosition(fade.GetPosition() + Vector3(0, +40, 0));
			fade.SpriteUpdate(fade, spriteCommon_);
		}
		else {
			loading.SpriteUpdate(loading, spriteCommon_);
			//次シーンへ
			GameSceneManager::GetInstance()->ChangeScene("GAMEPLAY");
		}
		gameTimer++;
	}
	//更新
	player->worldTransform_.UpdateMatrix();
	viewProjection->UpdateMatrix();
}

void GameTitleScene::Draw()
{
	dxCommon_->PreDraw();

	Object3d::PreDraw(dxCommon_->GetCommandList());

	player->Draw(viewProjection);
	//背景オブジェクト
	for (auto& object : objects) {
		object->Draw(viewProjection);
	}

	Object3d::PostDraw();

	Sprite::PreDraw(dxCommon_->GetCommandList(), spriteCommon_);
	if (isNext == false) {
		titleBack[isBackNum].SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		title.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		if (onCursor == false) {
			click[0].SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		}
		else {
			click[1].SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		}
		circle.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		for (int i = 0; i < 9; i++) {
			cursor[i].SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		}
   }
	else {
		fade.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
		loading.SpriteDraw(dxCommon_->GetCommandList(), spriteCommon_, dxCommon_->GetDevice());
	}

	Sprite::PostDraw();

	dxCommon_->PostDraw();
}

void GameTitleScene::Finalize()
{

}