#pragma once
#include "ViewProjection.h"
#include "Object3d.h"
#include "WinApp.h"
#include "Vector3.h"
#include "input.h"
#include "Spline.h"
#include "Player.h"

class RailCamera {
public:
	//インスタンス
	RailCamera();
	~RailCamera();
	//初期化
	void Initialize(Player* player_);
	//更新
	void Update(Player* player_, std::vector<Vector3>& point);
	void ViewUpdate();

	void ShakeCamera(float x, float y);

	void TitleR(Player* player_);
	void RailReset();

	//Setter
	void SetPlayer(Player* player_);
	void SetEye(Vector3 view);
	void SetTarget(Vector3 target_);
	void SetOnRail(bool onrail) { OnRail = onrail; }

	//ベクトルを取得
	void GetVec(Vector3 a, Vector3 b);

	//Getter
	ViewProjection* GetView() { return viewProjection; }
	bool GetIsEnd() { return isEnd; }
	bool GetOnRail() { return OnRail; }
	Vector3 GetFrontVec() { return frontVec; }
	Object3d* GetCamera() { return camera; }
	Vector3 GetCameraPos() { return camera->worldTransform_.position_; }
	float GetPasPoint() { return splineCam.GetT(); }
private:
	Input* input_ = nullptr;
	ViewProjection* viewProjection = nullptr;
	Object3d* camera = nullptr;
	Input* input = nullptr;

	//スプライン
	Spline spline_;
	Spline splineCam;

	Vector3 rightVec = { 0, 0, 0 };
	Vector3 leftVec = { 0, 0, 0 };
	Vector3 frontVec = { 0,0,0 };
	Vector3 oldCamera = { 0,0,0 };

	//カメラディレイ
	Vector3 playerMoveVel;
	Vector3 cameraDelay;

	//tmp
	Vector3 eyeTmp{};
	Vector3 targetTmp{};

	bool isEnd;
	bool OnRail;
};

