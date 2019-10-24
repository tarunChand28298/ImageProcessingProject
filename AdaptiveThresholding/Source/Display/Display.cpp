#include "Display.h"

Display::Display()
{
}

Display::~Display()
{
}

void Display::Initialize(int _width, int _height, int _clearColour, bool _clearBeforeDraw)
{
	width = _width;
	height = _height;
	clearColour = _clearColour;
	clearBeforeDraw = _clearBeforeDraw;

	//Create window:
	windowClassInfo.style = CS_OWNDC;
	windowClassInfo.lpfnWndProc = MainWindowProcedure;
	windowClassInfo.cbClsExtra = 0;
	windowClassInfo.cbWndExtra = 0;
	windowClassInfo.hInstance = GetModuleHandle(0);
	windowClassInfo.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClassInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClassInfo.hbrBackground = nullptr;
	windowClassInfo.lpszMenuName = 0;
	windowClassInfo.lpszClassName = "MainWindowClassName";
	RegisterClass(&windowClassInfo);

	mainWindowHandle = CreateWindow("MainWindowClassName", "Image Processing Project", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width + 16, height + 39, nullptr, nullptr, GetModuleHandle(0), this);
	ShowWindow(mainWindowHandle, SW_SHOW);

	//Create pixelbuffer:
	pixelbufferInfo.bmiHeader.biSize = sizeof(pixelbufferInfo.bmiHeader);
	pixelbufferInfo.bmiHeader.biWidth = width;
	pixelbufferInfo.bmiHeader.biHeight = -height;
	pixelbufferInfo.bmiHeader.biPlanes = 1;
	pixelbufferInfo.bmiHeader.biBitCount = sizeof(int) * 8;
	pixelbufferInfo.bmiHeader.biCompression = BI_RGB;

	pixelBuffer = new int[width * height];
	ClearPixelbuffer();

	OnInitialize();
}

void Display::Update()
{
	{
		//Handle window messages:
		MSG message = {};
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	{
		//render:
		UpdateMaster();
	}
}

void Display::Shutdown()
{
	OnShutdown();
	UnregisterClass("MainWindowClassName", GetModuleHandle(0));

	delete[] pixelBuffer;
	pixelBuffer = nullptr;
}

void Display::ClearPixelbuffer()
{
	for (int i = 0; i < height * width; i++) {
		pixelBuffer[i] = clearColour;
	}
}

void Display::OnInitialize()
{
}

void Display::OnUpdate()
{
}

void Display::OnShutdown()
{
}

void Display::PresentPixelbufferToWindow()
{
	HDC currentDeviceContext = GetDC(mainWindowHandle);
	StretchDIBits(currentDeviceContext, 0, 0, width, height, 0, 0, width, height, pixelBuffer, &pixelbufferInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(mainWindowHandle, currentDeviceContext);
}

void Display::UpdateMaster()
{
	if (pixelBuffer == nullptr) { return; }

	if (clearBeforeDraw) { ClearPixelbuffer(); }
	OnUpdate();
	PresentPixelbufferToWindow();
}

LRESULT Display::MainWindowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
	static Display* displayInstance = nullptr;
	switch (message)
	{
	case WM_CREATE: {
		displayInstance = (Display*)((LPCREATESTRUCT)lparam)->lpCreateParams;
		displayInstance->running = true;
		break;
	}
	case WM_CLOSE: {
		DestroyWindow(windowHandle);
		break;
	}
	case WM_DESTROY: {
		displayInstance->running = false;
		displayInstance->Shutdown();
		break;
	}
	default: {
		break;
	}
	}

	return DefWindowProc(windowHandle, message, wparam, lparam);
}

int ColourMaths::MakeColour(char r, char g, char b)
{
	RGBColour outputColour;
	outputColour.channel[0] = b;
	outputColour.channel[1] = g;
	outputColour.channel[2] = r;
	outputColour.channel[3] = 0;

	return outputColour.colour;
}

int ColourMaths::Brightness(int colour)
{
	RGBColour inputColour;
	inputColour.colour = colour;

	return ((inputColour.channel[0] * 11) + (inputColour.channel[1] * 59) + (inputColour.channel[2] * 30)) / 100;
}

int LerpChar(char start, char end, float fraction)
{
	return (start + (end - start) * fraction);
}

int ColourMaths::LerpColour(int start, int end, float fraction)
{
	RGBColour outputColour;	
	RGBColour startColour;	
	RGBColour endColour;

	startColour.colour = start;
	endColour.colour = end;

	for (int i = 0; i < 4; i++) 
	{
		outputColour.channel[i] = LerpChar(startColour.channel[i], endColour.channel[i], fraction);
	}

	return outputColour.colour;
}
