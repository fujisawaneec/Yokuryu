#include "WinApp.h"

WinApp* WinApp::GetInstance() 
{
	static WinApp instance;

	return &instance;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	// メッセージで分岐
	switch (msg) {
	case WM_DESTROY: // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

void WinApp::Initialize()
{
	// ウィンドウクラスの設定
	//WNDCLASSEX w{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WindowProc;    // ウィンドウプロシージャを設定
	wc.lpszClassName = L"DirectXGame";       // ウィンドクラス名
	wc.hInstance = GetModuleHandle(nullptr); // ウィンドハンドル
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);// カーソル指定

	// ウィンドクラスをOSに登録する
	RegisterClassEx(&wc);
	// ウィンドサイズ{X座標　Y座標　横幅　縦幅}
	RECT wrc = { 0, 0, window_width, window_height };
	// 自動でサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(wc.lpszClassName, // クラス名
		L"DirectXGame",       // タイトルバーの文字
		WS_OVERLAPPEDWINDOW,  // 標準的なウィンドウスタイル
		CW_USEDEFAULT,        // 標準X座標 (05に任せる)
		CW_USEDEFAULT,        // 標準Y座標 (05に任せる)
		wrc.right - wrc.left, // ウィンドウ横幅
		wrc.bottom - wrc.top, // ウィンドウ縦幅
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	// ウィンドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);

}

void WinApp::Finalize()
{
	// ウィンドウクラスを登録解除
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}

bool WinApp::ProcessMessage()
{
	MSG msg{};// メッセージ

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg); //キー入力メッセージの処理
		DispatchMessage(&msg);  //プロシージャーにメッセージを送る
	}

	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}
