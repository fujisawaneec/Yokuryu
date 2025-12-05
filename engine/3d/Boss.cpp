#include "Boss.h"
#include "SphereCollider.h"
#include "time.h"
#include "stdlib.h"
#include "GameScene.h"

Boss::~Boss()
{
	delete bossModel;
}

void Boss::BossInitialize()
{
	Initialize();
	// OBJからモデルデータを読み込む
	bossModel = Model::LoadFromOBJ("fighter");
	// 3Dオブジェクト生成
	Create();
	// オブジェクトにモデルをひも付ける
	SetModel(bossModel);
	SetPosition({ -75,65,-200 });
	SetScale({ 10,10,10 });
	//パーツの初期化
	for (int i = 0; i < PARTS_NUM; i++) {
		parts[i] = Create();
		parts[i]->worldTransform_.SetParent3d(&worldTransform_);
		parts[i]->SetCollider(new SphereCollider());
	}
	parts[0]->SetPosition({ -2.0f,0.0f,-0.5f });
	parts[1]->SetPosition({ 2.0f,0.0f,-0.5f });
	parts[2]->SetPosition({ 0.0f,0.0f,1.0f });
	parts[3]->SetPosition({ -3.0f,0.0f,-1.0f });
	parts[4]->SetPosition({ 3.0f,0.0f,-1.0f });

	isDead_ = false;
	isInvisible = true;
	//タイマー
	appearTimer = 0;
	bossAlpha = 0.0f;
	hp = 300;
	isHit = false;
	hitTimer = 0;
	timer = 0;
	timeCount = 0;
	state = WAIT;
	//乱数
	srand((unsigned int)time(NULL));
}

void Boss::Update(Vector3 velo)
{
	//登場時
	if (appearTimer > 0) {
		if (appearTimer > 150) {
			SetPosition(GetPosition() + Vector3(0.5f, -0.1f, 0));
		}
		if (appearTimer > 100) {
			bossAlpha += 0.02f;
		}
		appearTimer--;
	}
	//基本挙動
	Move();
	ChangeState();

	//弾があるなら更新
	for (std::unique_ptr<BossBullet>& bullet : bullets_) {
		bullet->Update(velo);
	}
	//デスフラグの立った敵を削除
	bullets_.remove_if([](std::unique_ptr <BossBullet>& bullets_) {
		return bullets_->IsDead();
		});
	
	 //ダメージ判定
	if (hitTimer > 0) {
		hitTimer--;
		if (hitTimer == 0) {
			isHit = false;
		}
	}
	//HPが0なら死亡
	if (hp <= 0) {
		isDead_ = true;
	}
	//更新
	worldTransform_.UpdateMatrix();
	//当たり判定更新
	if (collider)
	{
		collider->Update();
	}
	//ボスパーツアップデート
	for (int i = 0; i < PARTS_NUM; i++) {
		parts[i]->Update();
	}
	//カウントリセット
	if (timeCount == 4) {
		timeCount = 0;
	}
}

void Boss::Pop()
{
	if (isInvisible == true) {
		isInvisible = false;
	}
	appearTimer = 300;
}

void Boss::Attack()
{
	//弾を生成し初期化
	std::unique_ptr<BossBullet> newBullet = std::make_unique<BossBullet>();

	//単発													   
	newBullet->BulletInitialize(GetPosition());
	newBullet->SetCollider(new SphereCollider(Vector3{ 0,0,0 }, 5.0f));

	//弾の登録										 
   //複数
	newBullet->SetPosition(GetPosition());
	newBullet->SetScale({ 1.2f,1.2f,1.2f });
	bullets_.push_back(std::move(newBullet));
}

void Boss::Move()
{
	//ボス登場後
	if (isInvisible == false) {
		if (timer < 75) {
			SetPosition(GetPosition() + Vector3(0.0f, 0.01f, 0.0f));
		}
		else if (timer < 150) {
			SetPosition(GetPosition() + Vector3(0.0f, -0.01f, 0.0f));
		}
		else {
			timer = 0;
			timeCount++;
		}
		timer++;
	}
}

void Boss::ChangeState()
{
	//待機状態
	if (state == WAIT) {
		if (timeCount == 4) {
			//乱数により行動を決定
			int random = rand() % 1 + 1;
			//抽選された行動
			state = (State)random;
		}						 
	}
	//射撃状態
	else if (state == SHOT) {
		Attack();
		state = WAIT;
	}
}

void Boss::BossDraw(ViewProjection* viewProjection_)
{
	Draw(viewProjection_, bossAlpha);
	//弾描画
	for (std::unique_ptr<BossBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}

void Boss::OnCollision(const CollisionInfo& info)
{
	//衝突相手の名前
	const char* str1 = "class PlayerBullet";

	//相手がplayerの弾
	if (strcmp(toCollisionName, str1) == 0) {
		if (isHit == false && isInvisible == false) {
			isHit = true;
			hitTimer = 30;
			hp-= 5;
			for (int i = 0; i < PARTS_NUM; i++) {
				if (parts[i]->GetIsLocked() == true) {
					parts[i]->SetIsLocked(false);
					GameScene::PopEnergy(parts[i]->GetWorldPos());
					hp -= 5;
				}
			}
		}
	}
}

void Boss::SkipMovie()
{
	appearTimer = 0;
	SetPosition({ 0.0f,49.99f,-200.0f });
	bossAlpha = 1.0f;
}
