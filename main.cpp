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

static bool _sWaitForExit = true;
static bool _sPressShow = false;
static bool _sToggleShow = false;

void Init()
{
	if (ChromaAnimationAPI::InitAPI() != 0)
	{
		cerr << "Failed to load Chroma library!" << endl;
		exit(1);
	}

	ChromaSDK::APPINFOTYPE appInfo = {};

	_tcscpy_s(appInfo.Title, 256, _T("HOTKEY Chroma sample"));
	_tcscpy_s(appInfo.Description, 1024, _T("A sample application using Razer Chroma SDK"));
	_tcscpy_s(appInfo.Author.Name, 256, _T("Whiterose"));
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
		cerr << "Failed to initialize Chroma!" << endl;
		exit(1);
	}
	Sleep(100); //wait for init
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
		// start with a blank frame
		memset(colorsChromaLink, 0, sizeof(int) * sizeChromaLink);
		memset(colorsHeadset, 0, sizeof(int) * sizeHeadset);
		memset(colorsKeyboard, 0, sizeof(int) * sizeKeyboard);
		memset(colorsKeypad, 0, sizeof(int) * sizeKeypad);
		memset(colorsMouse, 0, sizeof(int) * sizeMouse);
		memset(colorsMousepad, 0, sizeof(int) * sizeMousepad);

		// Show hotkeys
		SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_ESC, 255, 255, 0);
		SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_W, 0, 255, 0);
		SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_A, 0, 255, 0);
		SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_S, 0, 255, 0);
		SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_D, 0, 255, 0);
		
		// Show only when key is pressed
		if (_sPressShow)
		{			
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_O, 255, 255, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_P, 255, 255, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_SPACE, 255, 255, 0);
		}

		// Show when toggled
		if (_sToggleShow)
		{
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_T, 255, 0, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_I, 255, 0, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_J, 255, 0, 0);
			SetKeyColorRGB(colorsKeyboard, (int)Keyboard::RZKEY::RZKEY_K, 255, 0, 0);
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

		Sleep(1);
	}
}

void Cleanup()
{
	ChromaAnimationAPI::StopAll();
	ChromaAnimationAPI::CloseAll();
	RZRESULT result = ChromaAnimationAPI::Uninit();
	if (result != RZRESULT_SUCCESS)
	{
		cerr << "Failed to uninitialize Chroma!" << endl;
		exit(1);
	}
}

int main()
{
	fprintf(stdout, "HOTKEY CHROMA SAMPLE APP\r\n\r\n");

	Init();

	thread thread(GameLoop);
	cout << "Press `ESC` to Quit." << endl;
	cout << "Press `HOLD LEFT CONTROL` to show keys when pressed." << endl;
	cout << "Press `H` to toggle keys." << endl;

	InputHandler();

	thread.join();
	Cleanup();
	return 0;
}
