#pragma once

#include "GameBaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// シーン管理
/// </summary>
class GameSceneManager final
{
private: // メンバ関数
	// コンストラクタ
	GameSceneManager() = default;
	// デストラクタ
	~GameSceneManager();

public: // メンバ関数
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// シーンの解放
	/// </summary>
	void Destroy();

public:
	// インスタンスの作成
	static GameSceneManager* GetInstance();

private:
	// コピーコンストラクタを無効にする
	GameSceneManager(const GameSceneManager&) = delete;
	// 代入演算子を無効にする
	GameSceneManager& operator=(const GameSceneManager&) = delete;

public: // メンバ関数
	/// <summary>
	/// 次シーン予約
	/// </summary>
	/// <param name="sceneName">ファイル名</param>
	void ChangeScene(const std::string& sceneName);

	// シーンファクトリーのsetter
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private: // メンバ変数
	// 今のシーン(実行中シーン)
	GameBaseScene* scene_ = nullptr;
	// 次シーン
	GameBaseScene* nextScene_ = nullptr;
	// シーンファクトリー（借りてくる）
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

