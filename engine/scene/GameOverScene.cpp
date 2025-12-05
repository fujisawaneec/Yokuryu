#include "GameOverScene.h"

GameOverScene::GameOverScene()
{
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Initialize()
{
	input_ = Input::GetInstance();
	dxCommon = DirectXCommon::GetInstance();


	//ƒJƒƒ‰‰Šú‰»
	viewProjection = new ViewProjection();
	viewProjection->Initialize();

	sprite = new Sprite();
	spriteCommon_ = sprite->SpriteCommonCreate(dxCommon->GetDevice());
}

void GameOverScene::Update()
{
	viewProjection->UpdateMatrix();
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		GameSceneManager::GetInstance()->ChangeScene("TITLE");
	}
}

void GameOverScene::Draw()
{
	// •`‰æ‘Oˆ—
	dxCommon->PreDraw();

	Object3d::PreDraw(dxCommon->GetCommandList());


	Object3d::PostDraw();


	// •`‰æŒãˆ—
	dxCommon->PostDraw();
}

void GameOverScene::Finalize()
{
}
