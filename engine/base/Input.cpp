#include "Input.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//using namespace Microsoft::WRL;

Input* Input::GetInstance()
{
	static Input instance;

	return &instance;
}

void Input::Initialize(WinApp* winApp)
{
	// 借りてきたWinAppのインスタンスを記録
	this->winApp = winApp;

	HRESULT result;

	// DirectInputのインスタンス生成
	/*ComPtr<IDirectInput8> directInput = nullptr;*/
	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	//ComPtr<IDirectInputDevice8> keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//マウスデバイスのセット
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = mouse->SetDataFormat(&c_dfDIMouse); // 標準形式
	assert(SUCCEEDED(result));
	
	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	assert(SUCCEEDED(result));

	//デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;

	result = mouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	assert(SUCCEEDED(result));
	//マウスカーソルを隠す
	ShowCursor(FALSE);
}

void Input::Update()
{
	HRESULT result;

	// 前回のキー入力を保持
	memcpy(keyPre, key, sizeof(key));

	// キーボード情報の取得開始
	result = keyboard->Acquire();

	// 全キーの入力状態を取得する
	/*BYTE key[256] = {};*/
	result = keyboard->GetDeviceState(sizeof(key), key);

	//マウス制御開始
	memcpy(&mouseState_bak, &mouseState, sizeof(mouseState_bak));
	mouse->Acquire();

	result = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
}

//マウス左
bool Input::PushMouseLeft()
{
	if (mouseState.rgbButtons[0] != 0) {
		return true;
	}
	return false;
}

bool Input::TriggerMouseLeft()
{
	if (mouseState.rgbButtons[0] > 0 && mouseState_bak.rgbButtons[0] == 0) {
		return true;
	}
	return false;
}

bool Input::PushMouseRight()
{
	if (mouseState.rgbButtons[1] > 0) {
		return true;
	}
	return false;
}

bool Input::TriggerMouseRight()
{
	if (mouseState.rgbButtons[1] > 0 && mouseState_bak.rgbButtons[1] == 0) {
		return true;
	}
	return false;
}

Vector3 Input::GetMousePos()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(winApp->GetHwnd(), &p);

	//z成分を0で返す
	return Vector3((float)p.x,(float)p.y,0.0f);
}

Vector3 Input::GetMouseVelo()
{
	return Vector3((float)mouseState.lX - mouseState_bak.lX,(float)mouseState.lY - mouseState_bak.lY,0);
}

void Input::SetMousePos(Vector2 pos)
{
	POINT p{ (long)pos.x,(long)pos.y };
	SetCursorPos(p.x,p.y);
}

bool Input::PushKey(BYTE keyNumber) 
{
	// 指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	// そうではなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (key[keyNumber] && keyPre[keyNumber] == false) {
		return true;
	}
	
	return false;
}

bool Input::AnyKey()
{
	for (int i = 0; i < sizeof(key); i++) {
		if (key[i] && keyPre[i] == false) {
			return true;
		}
	}

	return false;
}
