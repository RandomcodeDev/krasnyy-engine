#include "video_win32.h"
#include "base/base.h"
#include "base/basicstr.h"
#include "utility/log.h"

bool CWindowsVideoSystem::Initialize()
{
	m_hinstance = reinterpret_cast<HINSTANCE>(NtCurrentImageBase());

	Log_Info("Initializing Windows video system");

	if (!RegisterWindowClass() || !InitializeMainWindow())
	{
		return false;
	}

	ShowWindow(m_window, SW_NORMAL);

	return true;
}

bool CWindowsVideoSystem::Update()
{
	m_resized = false;

	MSG msg = {};
	while (PeekMessageA(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return !m_closed;
}

void CWindowsVideoSystem::Shutdown()
{
	DestroyWindow(m_window);
	UnregisterClassA(WINDOW_CLASS, m_hinstance);

	Log_Info("Windows video system shut down");
}

void CWindowsVideoSystem::SetTitle(cstr newTitle)
{
	m_title = Base_StrClone(newTitle);
	if (m_window)
	{
		SetWindowTextA(m_window, m_title);
	}
}

bool CWindowsVideoSystem::RegisterWindowClass()
{
	WNDCLASSEXA wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEXA);

	// Check if the class is already registered
	if (GetClassInfoExA(m_hinstance, WINDOW_CLASS, &wndClass))
	{
		Log_Debug("GetClassInfoExA(\"" WINDOW_CLASS "\") returned true, assuming window class is already registered");
		return true;
	}

	Log_Debug("Registering " WINDOW_CLASS " window class");

	Base_MemSet(&wndClass, 0, sizeof(WNDCLASSEXA));
	wndClass.cbSize = sizeof(WNDCLASSEXA);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpszClassName = WINDOW_CLASS;
	wndClass.hInstance = m_hinstance;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
	if (!RegisterClassExA(&wndClass))
	{
		Log_Error("Failed to register window class: Win32 error %d (0x%X)", LastNtError(), LastNtError());
		return false;
	}

	return true;
}

bool CWindowsVideoSystem::InitializeMainWindow()
{
	RECT clientArea = {};

	Log_Debug("Creating main window");

	// make the window 2/3 of the main monitor's size by default
	clientArea.left = 0;
	clientArea.right = (u32)(GetSystemMetrics(SM_CXSCREEN) * 2 / 3);
	clientArea.top = 0;
	clientArea.bottom = (u32)(GetSystemMetrics(SM_CYSCREEN) * 2 / 3);

	// get the size the window needs to be created at for the client area to be the desired size
	RECT adjustedArea = clientArea;
	AdjustWindowRect(&adjustedArea, WINDOW_STYLE, FALSE);
	m_width = adjustedArea.right - adjustedArea.left;
	m_height = adjustedArea.bottom - adjustedArea.top;

	m_extraWidth = m_width - clientArea.right;
	m_extraHeight = m_height - clientArea.bottom;

	m_title = Base_StrFormat("Chifir Engine");

	// center the window
	u32 x = GetSystemMetrics(SM_CXSCREEN) / 2 - m_width / 2;
	u32 y = GetSystemMetrics(SM_CYSCREEN) / 2 - m_height / 2;
	Log_Info(
		"Creating %ux%u (internally %ux%u) window at (%u, %u) titled %s", m_width + m_extraWidth, m_height + m_extraHeight, x, y,
		m_title);
	m_window =
		CreateWindowExA(0, WINDOW_CLASS, m_title, WINDOW_STYLE, x, y, m_width, m_height, nullptr, nullptr, m_hinstance, nullptr);
	if (!m_window)
	{
		Log_Error("Failed to create window: Win32 error %d (0x%X)", LastNtError(), LastNtError());
		return false;
	}

	// Set the window's user pointer to this
	SetWindowLongPtrA(m_window, GWLP_USERDATA, reinterpret_cast<uptr>(this));

	// Update DPI, because size and position get messages
	UpdateDpi();

	return true;
}

void CWindowsVideoSystem::UpdateSize()
{
	RECT clientArea = {};
	GetClientRect(m_window, &clientArea);
	m_width = clientArea.right - clientArea.left;
	m_height = clientArea.bottom - clientArea.top;
}

void CWindowsVideoSystem::UpdatePosition()
{
	RECT clientArea = {};
	GetClientRect(m_window, &clientArea);
	POINT clientPos = {clientArea.left, clientArea.top};
	ClientToScreen(m_window, &clientPos);
	m_x = clientPos.x;
	m_y = clientPos.y;
}

extern "C" BASEAPI bool GetDpiForWindow_Available();

void CWindowsVideoSystem::UpdateDpi()
{
	// Only on Windows 10 and later
	if (GetDpiForWindow_Available())
	{
		m_dpi = GetDpiForWindow(m_window) / static_cast<f32>(USER_DEFAULT_SCREEN_DPI);
	}
	else
	{
		m_dpi = 1.0;
	}
}

LRESULT __stdcall CWindowsVideoSystem::WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Get the video system pointer from the window
	CWindowsVideoSystem* videoSystem = reinterpret_cast<CWindowsVideoSystem*>(GetWindowLongPtrA(window, GWLP_USERDATA));

	switch (msg)
	{
	case WM_DPICHANGED: {
		videoSystem->UpdateDpi();
		return 0;
	}
	case WM_MOVE: {
		videoSystem->UpdatePosition();
		return 0;
	}
	case WM_SIZE: {
		videoSystem->UpdateSize();
		return 0;
	}
	case WM_CLOSE: {
		videoSystem->m_closed = true;
		return 0;
	}
	}

	return DefWindowProcA(window, msg, wparam, lparam);
}
