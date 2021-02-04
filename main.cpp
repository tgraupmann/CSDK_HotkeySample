// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Razer\ChromaAnimationAPI.h"
#include "HandleInput.h"
#include <array>
#include <chrono>
#include <conio.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <time.h>
#include <thread>

using namespace ChromaSDK;
using namespace std;

static bool _sInitialized = false;
static bool _sWaitForExit = true;
static bool _sPressShow = false;
static bool _sToggleShow = true;
static bool _sToggleUninit = false; //toggle uninit Chroma when LCTRL is not pressed

void Init()
{
	if (_sInitialized)
	{
		return; //already initialized
	}

	ChromaSDK::APPINFOTYPE appInfo = {};

	_tcscpy_s(appInfo.Title, 256, _T("Hotkey Chroma sample"));
	_tcscpy_s(appInfo.Description, 1024, _T("A sample application using Razer Chroma SDK"));
	_tcscpy_s(appInfo.Author.Name, 256, _T("Razer"));
	_tcscpy_s(appInfo.Author.Contact, 256, _T("https://developer.razer.com/chroma"));

	//appInfo.SupportedDevice = 
	//    0x01 | // Keyboards
	//    0x02 | // Mice
	//    0x04 | // Headset
	//    0x08 | // Mousepads
	//    0x10 | // Keypads
	//    0x20   // ChromaLink devices
	appInfo.SupportedDevice = (0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20);
	//    0x01 | // Utility. (To specifiy this is an utility application)
	//    0x02   // Game. (To specifiy this is a game);
	appInfo.Category = 1;

	RZRESULT result = ChromaAnimationAPI::InitSDK(&appInfo);
	if (result != RZRESULT_SUCCESS)
	{
		cerr << "Failed to initialize Chroma! Result=" << result  << endl;
		exit(1);
	}
	
	_sInitialized = true;
}

void Uninit()
{
	if (!_sInitialized)
	{
		return;
	}

	RZRESULT result = ChromaAnimationAPI::Uninit();
	if (result != RZRESULT_SUCCESS)
	{
		cerr << "Failed to uninitialize Chroma! Result=" << result << endl;
		exit(1);
	}

	_sInitialized = false;
}

int GetKeyColorIndex(int row, int column)
{
	return Keyboard::MAX_COLUMN * row + column;
}

void SetKeyColor(int* colors, int rzkey, int color)
{
	int row = HIBYTE(rzkey);
	int column = LOBYTE(rzkey);
	colors[GetKeyColorIndex(row, column)] = color;
}

void SetKeyColorRGB(int* colors, int rzkey, int red, int green, int blue)
{
	SetKeyColor(colors, rzkey, ChromaAnimationAPI::GetRGB(red, green, blue));
}

const int GetColorArraySize1D(EChromaSDKDevice1DEnum device)
{
	const int maxLeds = ChromaAnimationAPI::GetMaxLeds((int)device);
	return maxLeds;
}

const int GetColorArraySize2D(EChromaSDKDevice2DEnum device)
{
	const int maxRow = ChromaAnimationAPI::GetMaxRow((int)device);
	const int maxColumn = ChromaAnimationAPI::GetMaxColumn((int)device);
	return maxRow * maxColumn;
}

void SetAmbientColor(int* colors, int size, int color)
{
	for (int i = 0; i < size; ++i)
	{
		colors[i] = color;
	}
}

void GameLoop()
{
	const int sizeChromaLink = GetColorArraySize1D(EChromaSDKDevice1DEnum::DE_ChromaLink);
	const int sizeHeadset = GetColorArraySize1D(EChromaSDKDevice1DEnum::DE_Headset);
	const int sizeKeyboard = GetColorArraySize2D(EChromaSDKDevice2DEnum::DE_Keyboard);
	const int sizeKeypad = GetColorArraySize2D(EChromaSDKDevice2DEnum::DE_Keypad);
	const int sizeMouse = GetColorArraySize2D(EChromaSDKDevice2DEnum::DE_Mouse);
	const int sizeMousepad = GetColorArraySize1D(EChromaSDKDevice1DEnum::DE_Mousepad);

	int* colorsChromaLink = new int[sizeChromaLink];
	int* colorsHeadset = new int[sizeHeadset];
	int* colorsKeyboard = new int[sizeKeyboard];
	int* colorsKeypad = new int[sizeKeypad];
	int* colorsMouse = new int[sizeMouse];
	int* colorsMousepad = new int[sizeMousepad];

	while (_sWaitForExit)
	{
		if (_sToggleUninit)
		{
			if (!_sPressShow)
			{
				Uninit(); // Uninitialize Chroma to fallback to Synapse or the next Chroma app
				Sleep(33); //30 FPS
				continue;
			}
		}
		
		Init(); // initialize Chroma if not initialized

		// start with a color
		int color = ChromaAnimationAPI::GetRGB(0, 0, 48); //blue
		SetAmbientColor(colorsChromaLink, sizeChromaLink, color);
		SetAmbientColor(colorsHeadset, sizeHeadset, color);
		SetAmbientColor(colorsKeyboard, sizeKeyboard, color);
		SetAmbientColor(colorsKeypad, sizeKeypad, color);
		SetAmbientColor(colorsMouse, sizeMouse, color);
		SetAmbientColor(colorsMousepad, sizeMousepad, color);

		// Show only when key is pressed
		if (_sPressShow)
		{			
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_O, 0, 255, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_P, 0, 255, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_SPACE, 0, 255, 0);
		}

		// Show when toggled
		if (_sToggleShow)
		{
			// Show hotkeys
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_ESC, 255, 255, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_H, 255, 255, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_U, 255, 255, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_LCTRL, 255, 255, 0);

			// WASD KEYS
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_W, 255, 0, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_A, 255, 0, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_S, 255, 0, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_D, 255, 0, 0);
		}

		ChromaAnimationAPI::SetEffectCustom1D((int)EChromaSDKDevice1DEnum::DE_ChromaLink, colorsChromaLink);
		ChromaAnimationAPI::SetEffectCustom1D((int)EChromaSDKDevice1DEnum::DE_Headset, colorsHeadset);
		ChromaAnimationAPI::SetEffectCustom1D((int)EChromaSDKDevice1DEnum::DE_Mousepad, colorsMousepad);

		ChromaAnimationAPI::SetCustomColorFlag2D((int)EChromaSDKDevice2DEnum::DE_Keyboard, colorsKeyboard);
		ChromaAnimationAPI::SetEffectKeyboardCustom2D((int)EChromaSDKDevice2DEnum::DE_Keyboard, colorsKeyboard);

		ChromaAnimationAPI::SetEffectCustom2D((int)EChromaSDKDevice2DEnum::DE_Keypad, colorsKeypad);
		ChromaAnimationAPI::SetEffectCustom2D((int)EChromaSDKDevice2DEnum::DE_Mouse, colorsMouse);

		Sleep(33); //30 FPS
	}
	
	delete[] colorsChromaLink;
	delete[] colorsHeadset;
	delete[] colorsKeyboard;
	delete[] colorsKeypad;
	delete[] colorsMouse;
	delete[] colorsMousepad;
}

void InputHandler()
{
	HandleInput inputLControl = HandleInput(VK_LCONTROL);
	HandleInput inputEscape = HandleInput(VK_ESCAPE);
	HandleInput inputH = HandleInput('H');
	HandleInput inputU = HandleInput('U');

	while (_sWaitForExit)
	{
		if (inputEscape.WasReleased())
		{
			_sWaitForExit = false;
		}
		_sPressShow = inputLControl.IsPressed();
		if (inputH.WasReleased())
		{
			_sToggleShow = !_sToggleShow;
		}
		if (inputU.WasReleased())
		{
			_sToggleUninit = !_sToggleUninit;
		}

		Sleep(1);
	}
}

int main()
{
	fprintf(stdout, "HOTKEY CHROMA SAMPLE APP\r\n\r\n");

	if (!ChromaAnimationAPI::GetIsInitializedAPI() &&
		ChromaAnimationAPI::InitAPI() != 0)
	{
		cerr << "Failed to load Chroma library!" << endl;
		exit(1);
	}

	thread thread(GameLoop);
	cout << "Press `ESC` to Quit." << endl;
	cout << "Press `HOLD LEFT CONTROL` to show keys when pressed." << endl;
	cout << "Press `H` to toggle keys." << endl;
	cout << "Press `U` to toggle uninit Chroma when LEFT CONTROL isn't pressed." << endl;

	InputHandler();

	thread.join();
	Uninit();
	return 0;
}
