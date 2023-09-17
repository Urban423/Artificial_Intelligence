#include "Window.h"

#define SetRValue(c, r) ((COLORREF)((c & 0x00FFFF00) | ((BYTE)(r))))
#define SetGValue(c, g) ((COLORREF)((c & 0x00FF00FF) | ((BYTE)(g) << 8)))
#define SetBValue(c, b) ((COLORREF)((c & 0x0000FFFF) | ((BYTE)(b) << 16)))

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		break;
	}
	case WM_SIZE:
	{
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		break;
	}

	case WM_MOVE:
	{
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		break;
	}

	case WM_SETFOCUS:
	{
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		break;
	}
	case WM_KILLFOCUS:
	{
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		break;
	}
	case WM_DESTROY:
	{
		Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->onDestroy();
		::PostQuitMessage(0);
		break;
	}


	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return NULL;
}

Window::Window(PNG_reader image)
{
	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.lpszClassName = L"A.I.";
	wc.lpszMenuName = L"";
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;

	::RegisterClassEx(&wc);

	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"A.I.", L"A.I.",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 333, 333,
		NULL, NULL, NULL, NULL);

	::SetWindowPos(m_hwnd, 0, 1024, 0, image.width + 120, image.height + 120, 0);
	::ShowWindow(m_hwnd, SW_SHOW);
	::UpdateWindow(m_hwnd);

	SetPixel(image);

	m_is_run = true;
}

bool Window::isRunning()
{
	if (m_is_run) {
		broadcast();
	}
	return m_is_run;
}

void Window::SetPixel(PNG_reader image)
{
	HDC hdc = GetDC(m_hwnd);

	COLORREF* color_array = new  COLORREF[image.width * image.height];
	int j = 0;
	for (int i = 0; i < image.width * image.height; i++)
	{
		if (image.bytes_per_pixel == 3 || image.bytes_per_pixel == 4)
		{
			color_array[i] = RGB(image.image_data[j + 2], image.image_data[j + 1], image.image_data[j]);
		}
		else {
			color_array[i] = RGB(image.image_data[j], image.image_data[j], image.image_data[j]);
		}
		j += image.bytes_per_pixel;
	}

	HBITMAP map = CreateBitmap(image.width, image.height, 1, 8 * 4, color_array);


	HDC src = CreateCompatibleDC(hdc);
	SelectObject(src, map);
	BitBlt(hdc, 0, 0, image.width, image.height, src, 0, 0, SRCCOPY);
	DeleteDC(src);
}

RECT Window::getClientWindowRect()
{
	RECT rc;
	::GetClientRect(this->m_hwnd, &rc);
	::ClientToScreen(this->m_hwnd, (LPPOINT)&rc.left);
	::ClientToScreen(this->m_hwnd, (LPPOINT)&rc.right);
	return rc;
}

RECT Window::getSizeScreen()
{
	RECT rc;
	rc.right = ::GetSystemMetrics(SM_CXSCREEN);
	rc.bottom = ::GetSystemMetrics(SM_CYSCREEN);
	return rc;
}

HWND Window::getWindow()
{
	return m_hwnd;
}

void Window::onDestroy()
{
	m_is_run = false;
}

Window::~Window()
{
}

bool Window::broadcast()
{
	MSG msg;

	if (!this->m_is_init)
	{
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
		this->m_is_init = true;
	}

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//Sleep(11);

	return true;
}
