#pragma once

#include <Windows.h>
#include <D3dx12.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <cassert>
#pragma warning(push)
#pragma	warning(disable:26813)
#include <DirectXTex.h>
#pragma warning(pop)
#include <wrl.h>
#include <array>
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4.h"

#include "DirectXCommon.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;



// パイプラインセット
struct PipelineSet {
	// パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelinestate;
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsignature;
};

// スプライトの共通データ
struct SpriteCommon {
	// パイプラインセット
	PipelineSet pipelineSet;
	// 射影行列
	Matrix4 matProjection{};
	// テクスチャ用デスクリプタヒープの生成
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// SRVの最大枚数
	static const size_t kMaxSRVCount = 512;
	// テクスチャソース(テクスチャバッファ)の配列
	ComPtr<ID3D12Resource> texBuff[kMaxSRVCount];
	// テクスチャバッファ取得
	ID3D12Resource* GetTexBuffer(uint32_t index) const { return texBuff[index].Get(); }
};

// スプライト
class Sprite
{
public://サブクラス
	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		Vector4 color;
		Matrix4 mat;	// ３Ｄ変換行列
	};

	// 頂点データ
	struct VertexPosUv {
		Vector3 pos;
		Vector2 uv;
	};
public:
	Sprite();
	~Sprite();

	/// スプライト共通データ生成
	SpriteCommon SpriteCommonCreate(ID3D12Device* dev);

	/// 3Dオブジェクト用パイプライン生成
	PipelineSet SpriteCreateGraphicsPipeline(ID3D12Device* device);

	/// スプライト共通テクスチャ読み込み
	void LoadTexture(SpriteCommon& spriteCommon, UINT texnumber, 
		const wchar_t* filename, ID3D12Device* dev);

	/// スプライト生成
	void SpriteCreate(ID3D12Device* dev, int window_width, int window_height, 
		UINT texNumber, Vector2 anchorpoint, bool isFlipX, bool FlipY);

	/// スプライト単体頂点バッファの転送
	void SpriteTransferVertexBuffer(const Sprite& sprite, uint32_t texIndex_ = UINT32_MAX);

	/// スプライト単体更新
	void SpriteUpdate(Sprite& sprite, const SpriteCommon& spriteCommon);

	/// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon);

	/// 描画後処理
	static void PostDraw();

	/// スプライト単体描画
	void SpriteDraw(ID3D12GraphicsCommandList* cmdList_, const SpriteCommon& spriteCommon, 
		ID3D12Device* dev);

	/// 終了処理
	void Finalize();

public: // セッター
	// 座標設定
	void SetPosition(Vector3 position) { this->position = position; }
	// サイズ設定
	void SetScale(Vector2 scale) { this->scale = scale; }
	// 回転
	void SetRotation(float rotation) { this->rotation = rotation; }
	// 番号
	void SetTexNumber(UINT texNumber) { this->texNumber = texNumber; }
	// 色
	void SetColor(const Vector4& color) { this->color_ = color; }
	void SetAlpha(Sprite sprite, float alpha_);
	// フリップ
	void SetIsFlipX(bool isFlipX) { this->isFlipX_ = isFlipX; } // X
	void SetIsFlipY(bool isFlipY) { this->isFlipY_ = isFlipY; } // Y
	// テクスチャ左上座標
	void SetTexLeftTop(const Vector2& texLeftTop) { this->texLeftTop_ = texLeftTop; }
	// テクスチャ切り出しサイズ
	void SetTexSize(const Vector2& texSize) { this->texSize_ = texSize; }

public: // ゲッター
	// 座標
	Vector3 GetPosition() const { return position; }
	// サイズ
	Vector2 GetScale() const { return scale; }
	//ワールド行列
	Matrix4 GetMat() { return constMap->mat; }
	// 回転
	float GetRotation() const { return rotation; }
	// 番号
	UINT GetTexNumber() const { return texNumber; }
	// 色
	Vector4 GetColor() const { return color_; }
	// フリップ
	bool GetIsFlipX() const { return isFlipX_; } // X
	bool GetIsFlipY() const { return isFlipY_; } // Y
	// テクスチャ左上座標
	Vector2 GetTexLeftTop() const { return texLeftTop_; }
	// テクスチャ切り出しサイズ
	Vector2 GetTexSize() const { return texSize_; }

public:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_RESOURCE_DESC resDesc;

private:
	// DirectXCommonのインスタンス
	DirectXCommon* dXCommon = nullptr;
	SpriteCommon spriteCommon_;

	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12DescriptorHeap> descHeap;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0 = nullptr;
	// マッピング済みアドレス
	ConstBufferDataB0* constMap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	static const size_t kMaxSRVCount = 2056;
	// テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>, kMaxSRVCount> texBuffers_;

	// テクスチャ番号
	UINT texNumber = 0;
	uint32_t texIndex_ = 0;
	// 座標
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	// サイズ
	Vector2 scale = { 1.0f, 1.0f };
	// 回転
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	// ワールド行列
	Matrix4 matWorld;
	// アンカーポイント
	Vector2 anchorpoint = { 0.0f,0.0f };
	// テクスチャ左上座標設定
	Vector2 texLeftTop_ = { 50.0f, 50.0f };
	// テクスチャ切り出しサイズ
	Vector2 texSize_ = { 100.0f, 100.0f };
	// 色
	Vector4 color_ = { 1,1,1,1 };

	// Z軸回りの回転
	float rotation = 0.0f;
	// 左右反転
	bool isFlipX_ = false;
	// 上下反転
	bool isFlipY_ = false;
	// 非表示
	bool isInvisible = false;
};

