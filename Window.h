#pragma once
#include <Windows.h>
#include "PNG_reader.h"

class Window
{
public:
	Window(PNG_reader image);
	bool isRunning();

	void SetPixel(PNG_reader image);
	RECT getClientWindowRect();
	RECT getSizeScreen();
	HWND getWindow();

	void onDestroy();

	~Window();
private:
	bool broadcast();
protected:
	HWND m_hwnd;
	bool m_is_run;
	bool m_is_init = false;
};