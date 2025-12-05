#pragma once
#include <string>
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#pragma warning(push)
#pragma warning(disable:26813)
#include<DirectXTex.h>
#pragma warning(pop)
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#pragma warning(push)
#pragma	warning(disable:26495)
#pragma	warning(disable:26451)
#include <fbxsdk.h>
#pragma warning(pop)

//ノード
struct Node {
	//名前
	std::string name;
	//スケール
	Vector4 scaling = { 1,1,1,0 };
	//回転角
	Vector4 rotation = { 0,0,0,0 };
	//ローカル移動
	Vector4 translation = { 0,0,0,1 };
	//ローカル変形行列
	Matrix4 tranform;
	//グローバル変形行列
	Matrix4 globalTransform;
	//親ノード
	Node* parent = nullptr;
};

class FbxModel {
public:
	friend class FbxLoader;
public://定数
	//ボーンインデックスの最大数
	static const int MAX_BONE_INDICES = 4;
//サブクラス
public:
	struct VertexPosNormalUvSkin {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};
	//ボーン
	struct Bone {
		std::string name;
		//初期姿勢の逆行列
		Matrix4 inInitialPose;
		//クラスター
		FbxCluster* fbxCluster;
		//コンストラクタ
		Bone(const std::string& name) {
			this->name = name;
		}
	};

private://エイリアス
	template <class T> using ComPtr =
		Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using TexMetadata = DirectX::TexMetadata;
	using sSratchImage = DirectX::ScratchImage;
	//std::を省略
	using string = std::string;
	template <class T> using vector =
		std::vector<T>;
private:
	//モデルの名前
	std::string name;
	//ノード配列
	std::vector<Node> nodes;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ
	std::vector<VertexPosNormalUvSkin> vertices;
	//頂点インデックス
	std::vector<unsigned int>indices;
	//アンビエント
	Vector3 ambient = { 1,1,1 };
	//ディフューズ
	Vector3 diffuse = { 1,1,1 };
	//テクスチャメタデータ
	DirectX::TexMetadata metaData = {};
	//スクラッチイメージ
	DirectX::ScratchImage scratchImg = {};
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV用デスクリプターヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//ボーン配列
	std::vector<Bone> bones;
	//FBXシーン
	FbxScene* fbxScene = nullptr;

public://関数
	~FbxModel();
	void CreateBuffers(ID3D12Device* device);
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//モデルの変形行列取得
	const Matrix4& GetModelTransform() { return meshNode->globalTransform; }
	//getter
	std::vector<Bone>& GetBones() { return bones; }
	FbxScene* GetFbxScene() { return fbxScene; }
};