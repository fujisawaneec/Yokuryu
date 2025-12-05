#include "GameBaseScene.h"


void GameBaseScene::LoadObjFromLevelEditor(const std::string& fileName) {
	JsonLoader* file = nullptr;
	LevelData* levelData = file->LoadFile(fileName);

	//オブジェクト配置
	for (auto& objectData : levelData->objects) {
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) { model = it->second; }
		//モデルを指定して3DObjectを生成
		Object3d* newObject = Object3d::Create();
		newObject->Initialize();
		newObject->SetModel(model);
		//座標
		Vector3 pos;
		pos = Vector3(objectData.translation.x, objectData.translation.y, objectData.translation.z);
		newObject->SetPosition(pos);
		//回転角
		Vector3 rot;
		rot = Vector3(objectData.rotation.x, objectData.rotation.y, objectData.rotation.z);
		newObject->SetRotation(rot);
		//スケール
		Vector3 scale;
		scale = Vector3(objectData.scaling.x, objectData.scaling.y, objectData.scaling.z);
		newObject->SetScale(scale);
		//配列に登録
		objects.push_back(newObject);
	}
}
