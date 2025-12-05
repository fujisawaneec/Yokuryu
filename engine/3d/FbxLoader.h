#pragma once
#pragma warning(push)
#pragma	warning(disable:26495)
#pragma	warning(disable:26451)
#include <fbxsdk.h>
#pragma warning(pop)

#include "d3d12.h"
#include "d3dx12.h"
#include <string>
#include "FbxModel.h"
#include "Matrix4.h"
#pragma warning(push)
#pragma	warning(disable:26813)
#include "DirectXTex.h"
#pragma warning(pop)

class FbxLoader {
	//エイリアス
private:
	using string = std::string;

public:
	//シングルトンインスタンスの取得
	//<return>インスタンス</return>
	static FbxLoader* GetInstance();

	//初期化
	void Initialize(ID3D12Device* device);

	//fbxファイルからモデル読み込み
	FbxModel* LoadModelFlomFile(const string& modelName);

	//ノード解析
	void ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode,Node* parent = nullptr);

	//メッシュ解析
	void ParseMesh(FbxModel* model, FbxNode* fbxNode);
	//サブ
	void ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh);
	void ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh);
	void ParseMatrial(FbxModel* model, FbxNode* fbxNode);
	void LoadTexture(FbxModel* model, const std::string& fullPath);

	static void ConvertMatrixFromFbx(Matrix4 dst, const FbxAMatrix& src);

	//ファイル名抽出
	std::string ExtracFileName(const std::string& path);

	//後始末
	void Finalize();

	//スキニング情報の読み取り
	void ParseSkin(FbxModel* model, FbxMesh* fbxMesh);

private:
	//D3D12デバイス
	ID3D12Device* device = nullptr;
	//FBXマネージャー
	FbxManager* fbxManager = nullptr;
	//FBXインポーター
	FbxImporter* fbxImporter = nullptr;

private:
	//privateなコンストラクタ
	FbxLoader() = default;
	//privateなデストラクタ
	~FbxLoader() = default;
	//コピーコンストラクタを禁止
	FbxLoader(const FbxLoader& obj) = delete;
	//コピー代入演算子を禁止
	void operator=(const FbxLoader& obj) = delete;

//private静的関数
private:
	static const string defaultTextureFileName;
public:
	static const string baseDirectory;
};