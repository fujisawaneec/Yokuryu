#include "Enemy.h"
#include "string.h"
#include "BaseCollider.h"
#include "SphereCollider.h"
#include "GameScene.h"

//デストラクタ
Enemy::~Enemy() {
	delete enemyModel;
}

//初期化
void Enemy::EnemyInitialize()
{
	Initialize();
	// OBJからモデルデータを読み込む
	enemyModel = Model::LoadFromOBJ("triangle_mat");
	enemyModel->LoadTexture("Resources/red.png");
	// 3Dオブジェクト生成
	Create();
	// オブジェクトにモデルをひも付ける
	SetModel(enemyModel);
	isDead_ = false;
	isInvisible = true;
	timer = 0;
	isAttack = false;
	timeCount = 0;
	alpha = 0;
}

void Enemy::Update(Vector3 velo,float t) {
	//透明状態なら
	if (isInvisible == true) {
		float len = stagePoint - t + 1.0f;
 		if (len < 4.0f) {
			isInvisible = false;
		}
	}
	else {
		if (alpha < 1) {
			alpha += 0.05f;
		}
		float moveX;
		if (timeCount == 0) {
			moveX = -0.05f;
		}
		else if (timeCount == 1) {
			moveX = 0.05f;
		}
		else if (timeCount == 2) {
			moveX = 0.05f;
		}
		else {
			moveX = -0.05f;
		}

		if (timer < 75) {
			SetPosition(GetPosition() + Vector3(moveX, 0.005f, 0));
		}
		else if (timer < 150) {
			SetPosition(GetPosition() + Vector3(moveX, -0.005f, 0));
		}
		else {
			timer = 0;
			if (timeCount == 4) {
				timeCount = 0;
			}
			else {
				timeCount++;
			}
		}
		//playerが敵を追い越したら攻撃しない
		if (stagePoint < t + 1.0f) {
			if (isAttack == true) {
				isAttack = false;
			}
		}
		//攻撃
		if (isAttack == false) {
			Vector3 playerVec = velo - GetPosition();
			float len = playerVec.length();
			if (len < 70.0f) {
				isAttack = true;
			}
		}
		else {
			Attack();
		}

		for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
			bullet->Update(velo);
		}
		//デスフラグの立った敵を削除
		bullets_.remove_if([](std::unique_ptr <EnemyBullet>& bullets_) {
			return bullets_->IsDead();
			});
		//当たり判定更新
		if (collider)
		{
			collider->Update();
		}
		worldTransform_.UpdateMatrix();
		timer++;
	}
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	//衝突相手の名前
	const char* str1 = "class PlayerBullet";

	//相手がplayerの弾
	if (strcmp(toCollisionName, str1) == 0) {
		if (isInvisible == false) {
			if (isDead_ == false) {
				isDead_ = true;
				for (int i = 0; i < 3; i++) {
					GameScene::PopEnergy(GetPosition());
				}
			}
		}
	}
}

void Enemy::Attack() {

		if (coolTime == 0) {
			//弾を生成し初期化
		//複数
			std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();

			//単発													   
			newBullet->BulletInitialize(GetPosition());
			newBullet->SetCollider(new SphereCollider(Vector3{0,0,0},0.5f));

			//弾の登録										 
		   //複数
			newBullet->SetPosition(GetPosition());
			newBullet->SetScale({ 0.3f,0.3f,0.3f });
			bullets_.push_back(std::move(newBullet));


			//クールタイムを設定
			coolTime = 200;
		}
		else if (coolTime > 0) {
			coolTime--;
		}

}

void Enemy::EnemyDraw(ViewProjection* viewProjection_) {
	Draw(viewProjection_, alpha);
	//弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}