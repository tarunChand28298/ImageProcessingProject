#pragma once

#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>

struct Bitmap {
	int width;
	int height;
	int* surface;

	Bitmap() = default;
	~Bitmap() {
		if (surface != nullptr) {
			delete[] surface;
			surface = nullptr;
		}
	}
	void LoadFromFile(std::string filename)
	{
		std::ifstream file(filename, std::ios::binary);

		BITMAPFILEHEADER fileHeader = {};
		BITMAPINFOHEADER infoHeader = {};

		file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
		file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

		width = infoHeader.biWidth;
		height = infoHeader.biHeight;
		if (surface != nullptr) { delete[] surface; surface = nullptr; }
		surface = new int[width * height];

		file.seekg(fileHeader.bfOffBits);
		for (int i = 0; i < width * height; i++) {
			unsigned char r, g, b, p;
			file.read(reinterpret_cast<char*>(&b), sizeof(char));
			file.read(reinterpret_cast<char*>(&g), sizeof(char));
			file.read(reinterpret_cast<char*>(&r), sizeof(char));
			file.read(reinterpret_cast<char*>(&p), sizeof(char));

			int finalColour = 0;
			finalColour = p;
			finalColour <<= 8;
			finalColour |= r;
			finalColour <<= 8;
			finalColour |= g;
			finalColour <<= 8;
			finalColour |= b;

			surface[i] = finalColour;
		}
	}
	void Initialize(int _width, int _height)
	{
		width = _width;
		height = _height;

		surface = new int[width * height];
	}

	int GetColour(int x, int y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height) { return 0x00000000; }
		return surface[(width * y) + x];
	}
	void SetColour(int x, int y, int colour)
	{
		surface[(width * y) + x] = colour;
	}
};

namespace ColourMaths
{
	union RGBColour
	{
		int colour;
		char channel[4];
	};

	int MakeColour(char r, char g, char b);

	int Brightness(int colour);
	int LerpColour(int start, int end, float fraction);
}

class Display
{
public:
	Display();
	~Display();

	void Initialize(int _width, int _height, int _clearColour, bool _clearBeforeDraw);
	void Update();
	void Shutdown();
	static LRESULT CALLBACK MainWindowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam);

	int width;
	int height;
	bool running;

	HWND mainWindowHandle = nullptr;
	int* pixelBuffer = nullptr;

	BITMAPINFO pixelbufferInfo = {};
	WNDCLASS windowClassInfo = {};

	bool clearBeforeDraw;
	int clearColour;

	void ClearPixelbuffer();
	virtual void OnInitialize();
	virtual void OnUpdate();
	virtual void OnShutdown();
	void PresentPixelbufferToWindow();

	void UpdateMaster();
};