#include "FbxLoader.h"
#include <cassert>
#include "Vector3.h"
#include "DirectXMath.h"

//静的変数の実体
const std::string FbxLoader::baseDirectory =
"Resources/";
const std::string FbxLoader::defaultTextureFileName =
"white1x1.png";

//インスタンスを取得
FbxLoader* FbxLoader::GetInstance() {
	static FbxLoader instance;
	return &instance;
}

//初期化
void FbxLoader::Initialize(ID3D12Device* device) {

	//再初期化チェック
	assert(fbxManager == nullptr);
	//引数からメンバ変数に代入
	this->device = device;
	//FBXマネージャーの生成
	fbxManager = FbxManager::Create();
	//FBXマネージャーの入出力設定
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);
	//FBXインポーターの生成
	fbxImporter = FbxImporter::Create(fbxManager, "");
}

//モデル読み込み
FbxModel* FbxLoader::LoadModelFlomFile(const string& modelName) {
	const string directoryPath = baseDirectory + modelName + "/";
	const string fileName = modelName + ".fbx";
	//連結
	const string fullPath = directoryPath + fileName;

	//ファイル名を指定して読み込み
	if (!fbxImporter->Initialize(fullPath.c_str(), -1, fbxManager->GetIOSettings())) {
		assert(0);
	}
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");

	//ファイル情報をシーンにインポート
	fbxImporter->Import(fbxScene);

	//モデル生成
	FbxModel* model = new FbxModel();
	model->name = modelName;
	//ノード数を取得
	int nodeCount = fbxScene->GetNodeCount();

	model->nodes.reserve(nodeCount);

	ParseNodeRecursive(model, fbxScene->GetRootNode());
	model->fbxScene = fbxScene;

	//バッファ生成
	model->CreateBuffers(device);
	assert(model->indices.size());

	return model;
}

//ノード解析
void FbxLoader::ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent) {

	model->nodes.emplace_back();
	Node& node = model->nodes.back();
	//ノード名を取得
	node.name = fbxNode->GetName();

	//fbxノードのローカル移動情報
	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();

	//形式変換して代入
	node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
	node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
	node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

	//回転角をDegreeからラジアンに変換
	node.rotation.x = Matrix4::ConvertToRadian(node.rotation.x);
	node.rotation.y = Matrix4::ConvertToRadian(node.rotation.y);
	node.rotation.z = Matrix4::ConvertToRadian(node.rotation.z);

	//角行列の計算
	Matrix4 matScaling, matRotation, matTranslation;
	Matrix4 matRotX, matRotY, matRotZ;
	Vector3 scl = { node.scaling.x,node.scaling.y ,node.scaling.z };
	matScaling = Matrix4::identity();
	matScaling.scale(scl);
	matRotation = Matrix4::identity();
	matRotation *= matRotZ.rotateZ(Matrix4::ConvertToRadian(node.rotation.z));
	matRotation *= matRotX.rotateX(Matrix4::ConvertToRadian(node.rotation.x));
	matRotation *= matRotY.rotateY(Matrix4::ConvertToRadian(node.rotation.y));
	Vector3 trl = { node.translation.x,node.translation.y,node.translation.z };
	matTranslation = Matrix4::identity();
	matTranslation.translate(trl);

	//ローカル変換行列の計算
	node.tranform = Matrix4::identity();
	node.tranform *= matScaling;
	node.tranform *= matRotation;
	node.tranform *= matTranslation;

	//グローバル変形行列の計算
	node.globalTransform = node.tranform;
	if (parent) {
		node.parent = parent;
		//親の変形を乗算
		node.globalTransform *= parent->globalTransform;
	}

	//メッシュ解析
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute) {
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
			model->meshNode = &node;
			ParseMesh(model, fbxNode);
		}
	}

	//子ノードに対して再起呼び出し
	for (int i = 0; i < fbxNode->GetChildCount(); i++) {
		ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
	}


}

void FbxLoader::ParseMesh(FbxModel* model, FbxNode* fbxNode) {
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//頂点座標読み取り
	ParseMeshVertices(model, fbxMesh);
	//面
	ParseMeshFaces(model, fbxMesh);
	//マテリアル
	ParseMatrial(model, fbxNode);
	//スキニング情報読み取り
	ParseSkin(model, fbxMesh);

}
//頂点情報読み取り
void FbxLoader::ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh) {
	auto& vertices = model->vertices;

	//頂点座標のデータの数
	const int controlPointsCount = fbxMesh->GetControlPointsCount();

	//必要数だけ頂点データを確保
	FbxModel::VertexPosNormalUvSkin vert{};
	model->vertices.resize(controlPointsCount, vert);

	//fbxメッシュの頂点座標配列を取得
	FbxVector4* pCoord = fbxMesh->GetControlPoints();

	//fbxメッシュの全頂点座標をモデル内の配列にコピーする
	for (int i = 0; i < controlPointsCount; i++) {
		FbxModel::VertexPosNormalUvSkin& vertex = vertices[i];
		//座標のコピー
		vertex.pos.x = (float)pCoord[i][0];
		vertex.pos.y = (float)pCoord[i][1];
		vertex.pos.z = (float)pCoord[i][2];
	}
}
//面情報読み取り
void FbxLoader::ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh) {
	auto& vertices = model->vertices;
	auto& indices = model->indices;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);

	//面の数
	const int polygonCount = fbxMesh->GetPolygonCount();
	//uvデータの数
	const int textureUVCount = fbxMesh->GetTextureUVCount();
	//uv名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	//面ごとの情報読み取り
	for (int i = 0; i < polygonCount; i++) {
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		for (int j = 0; j < polygonSize; j++) {
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);
			//頂点法線読み込み
			FbxModel::VertexPosNormalUvSkin& vertex = vertices[index];
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal)) {
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}
			//テクスチャuv読み込み
			if (textureUVCount > 0) {
				FbxVector2 uvs;
				bool lUnmappedUV;
				//0番決め打ちで読み込み
				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV)) {
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			//インデックス配列に頂点インデックスを追加
			if (j < 3) {
				indices.push_back(index);
			}
			//4頂点
			else {
				int index2 = indices[indices.size() - 1];
				int index3 = index;
				int index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}
		}
	}

}
//マテリアル読み込み
void FbxLoader::ParseMatrial(FbxModel* model, FbxNode* fbxNode) {
	const int materialCount = fbxNode->GetMaterialCount();
	if (materialCount > 0) {
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		bool textureLoaded = false;

		if (material) {
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
				FbxSurfaceLambert* lambert =
					static_cast<FbxSurfaceLambert*>(material);

				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				model->ambient.x = (float)ambient.Get()[0];
				model->ambient.y = (float)ambient.Get()[1];
				model->ambient.z = (float)ambient.Get()[2];

				//拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				model->diffuse.x = (float)diffuse.Get()[0];
				model->diffuse.y = (float)diffuse.Get()[1];
				model->diffuse.z = (float)diffuse.Get()[2];
			}
			//ディフューズテクスチャを取り出す
			const FbxProperty diffuseProperty =
				material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid()) {
				const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture) {
					const char* filepath = texture->GetFileName();
					//ファイルパスからファイル名抽出
					string path_str(filepath);
					string name = ExtracFileName(path_str);
					//テクスチャ読み込み
					LoadTexture(model, baseDirectory + model->name + "/" + name);
					textureLoaded = true;
				}
			}
		}
		//テクスチャがない場合白テクスチャを貼る
		if (!textureLoaded) {
			LoadTexture(model, baseDirectory + defaultTextureFileName);
		}
	}
}

//テクスチャ読み込み
void FbxLoader::LoadTexture(FbxModel* model, const std::string& fullPath) {
	HRESULT result = S_FALSE;
	//WICテクスチャのロード
	DirectX::TexMetadata& metadata = model->metaData;
	DirectX::ScratchImage& scratchImage = model->scratchImg;
	//ユニコード文字列に変換
	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilepath, _countof(wfilepath));
	result = DirectX::LoadFromWICFile(wfilepath, DirectX::WIC_FLAGS_NONE, &metadata, scratchImage);

	if (FAILED(result)) {
		assert(0);
	}
}

//ファイル名抽出
std::string FbxLoader::ExtracFileName(const std::string& path) {
	size_t pos1;
	//区切り文字""が出てくる一番最後の部分を検索
	pos1 = path.rfind("\\");
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	//区切り文字'/'
	pos1 = path.rfind('/');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

//後始末
void FbxLoader::Finalize() {
	//各種インスタンスの破棄
	fbxImporter->Destroy();
	fbxManager->Destroy();
}

void FbxLoader::ConvertMatrixFromFbx(Matrix4 dst, const FbxAMatrix& src) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			dst.m[i][j] = (float)src.Get(i, j);
		}
	}
}

void FbxLoader::ParseSkin(FbxModel* model, FbxMesh* fbxMesh) {
	FbxSkin* fbxSkin =
	static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	//スキニング情報がなければ終了
	if (fbxSkin == nullptr) {
		return;
	}
	//ボーン配列の参照
	std::vector<FbxModel::Bone>& bones = model->bones;
	//ボーンの数
	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++) {
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		//ボーン自体のノードの名前を取得
		const char* boneName = fbxCluster->GetLink()->GetName();
		//新しくボーンを追加し追加したボーンの参照を得る
		bones.emplace_back(FbxModel::Bone(boneName));
		FbxModel::Bone& bone = bones.back();
		//自作ボーンとFbxのボーンを紐づける
		bone.fbxCluster = fbxCluster;
		//FBXから初期姿勢行列を取得する
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);
		//Matrix4型に変換する
		Matrix4 initialPose;
		ConvertMatrixFromFbx(initialPose, fbxMat);
		//初期姿勢行列の逆行列を得る
		bone.inInitialPose.MakeInverse();
	}
	//ボーン番号とスキンウェイトのペア
	struct WeightSet {
		UINT index;
		float weight;
	};

	//二次元配列(ジャグ配列)
	//list:頂点が影響を受けるボーンの全リスト
	//vector:それを全頂点分
	std::vector<std::list<WeightSet>>
	weightLists(model->vertices.size());
	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++) {
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		//このボーンに影響を受ける頂点
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
		//このボーンに影響を受ける頂点の配列
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();
		//影響を受ける全頂点について
		for (int j = 0; j < controlPointIndicesCount; j++) {
			int vertIndex = controlPointIndices[j];
			//スキンウェイト
			float weight = (float)controlPointWeights[j];
			//その頂点の影響を受けるボーンリストにボーンとウェイトのペアを追加
			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}

		auto& vertices = model->vertices;
		for (int i = 0; i < vertices.size(); i++) {
			auto& weightList = weightLists[i];
			weightList.sort([](auto const& lhs, auto const& rhs) {
			//左の要素の方が大きければtrue そうでなければfalseを返す
			return lhs.weight > rhs.weight;
			});

			int weightArrayIndex = 0;
			//降順ソート済みのウェイトリストから
			for (auto& weightSet : weightList) {
				vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
				vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;
				//4つに達したら終了
				if (++weightArrayIndex >= FbxModel::MAX_BONE_INDICES) {
					float weight = 0.0f;
					//2番目以降のウェイトを合計
					for (int j = 1; j < FbxModel::MAX_BONE_INDICES; j++) {
						weight += vertices[i].boneWeight[j];
					}
					//合計で1.0f(100%)になるように調整
					vertices[i].boneWeight[0] = 1.0f - weight;
					break;
				}
			}
		}
	}
}