#pragma once
#include <Windows.h>

// WindowsAPI
class WinApp final
{
public: // 静的メンバ関数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: // メンバ関数
	// WindowsAPIの初期化
	void Initialize();

	// 終了
	void Finalize();

	// getter
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHInstance() const { return wc.hInstance; }

	// メッセージの処理
	bool ProcessMessage();

public:
	static WinApp* GetInstance();

private:
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;

public: // 定数
	// ウィンドウ横幅
	static const int window_width = 1280;
	// ウィンドウ縦幅
	static const int window_height = 720;

private:
	// ウィンドウハンドル
	HWND hwnd = nullptr;
	// ウィンドウクラスの設定
	WNDCLASSEX wc{};
};