#pragma once

#include "AbstractSceneFactory.h"
#include "GameTitleScene.h"
#include "GameScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"

/// <summary>
/// このゲーム用のシーン工場
/// </summary>
class SceneFactory : public AbstractSceneFactory
{
public:
	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns></returns>
	GameBaseScene* CreateScene(const std::string& sceneName) override;
};

