#include "HandleInput.h"

#include "windows.h"

HandleInput::HandleInput(int key)
{
	_mKey = key;
	_mWasPressed = false;
}
int HandleInput::GetKey()
{
	return _mKey;
}
bool HandleInput::IsPressed()
{
	return GetAsyncKeyState(_mKey);
}
bool HandleInput::WasReleased()
{
	if (GetAsyncKeyState(_mKey) != 0)
	{
		_mWasPressed = true;
	}
	else if (_mWasPressed)
	{
		_mWasPressed = false;
		return true;
	}
	return false;
}
