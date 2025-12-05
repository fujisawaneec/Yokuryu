#pragma once 
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <chrono>
#include <thread>

#pragma warning(push)
#pragma warning(disable:26813)
#include<DirectXTex.h>
#pragma warning(pop)

#include "WinApp.h"
#include "FPSFixed.h"

using namespace Microsoft::WRL;

// DirectX基盤
class DirectXCommon final
{
public: // メンバ関数
	// 初期化
	void Initialize(WinApp* winApp);
	// デバイスの初期化
	void InitializeDevice();
	// コマンド関連の初期化
	void InitializeCommand();
	// スワップチェーンの初期化
	void InitializeSwapchain();
	// レンダーターゲットビューの初期化
	void InitializeRenderTargetView();
	// 深度バッファの初期化
	void InitializeDepthBuffer();
	// フェンスの初期化
	void InitializeFence();
	// 描画前処理
	void PreDraw();
	// 描画後処理
	void PostDraw();
	// 終了処理
	void fpsFixedFinalize();

private: // メンバ関数
	template <class T>
	inline void safe_delete(T*& p) {
		delete p;
		p = nullptr;
	}

public:
	static DirectXCommon* GetInstance();

private:
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	DirectXCommon& operator=(const DirectXCommon&) = delete;
	

public: // Getter
	// デバイスの取得
	ID3D12Device* GetDevice() const { return device.Get(); }
	// コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	// バックバッファの数を取得
	size_t GetBackBufferCount() const { return backBuffers.size(); }

private: 
	// WindowsAPI
	WinApp* winApp = nullptr;
	// FPS
	FPSFixed* fpsFixed = nullptr;

	HRESULT result;
	// DirectX12デバイス
	ComPtr<ID3D12Device> device;
	// DXGIファクトリ
	ComPtr<IDXGIFactory7> dxgiFactory;
	// スワップチェイン
	ComPtr<IDXGISwapChain4> swapChain;
	// コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList;
	// コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;


	//バックバッファ
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	// フェンスの生成
	ComPtr<ID3D12Fence> fence;

	UINT rtvHD;
	UINT64 fenceVal = 0;
};