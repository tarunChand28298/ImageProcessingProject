#include "Display/Display.h"
#include "../escapi/escapi.h"

int THRESHOLD = 30;
#define STROKECOLOUR 0x00000000

#define TopColour 0xFF675CB3
#define BottomColour 0xFFFFF8C9

class ImageProcessingDisplay : public Display
{
	Bitmap inputImage;
	Bitmap processedImage;
	Bitmap outputImage;

	void OnInitialize() override
	{
		inputImage.Initialize(width, height);
		processedImage.Initialize(width, height);
		outputImage.Initialize(width, height);

		nCamera = setupESCAPI();

		capture.mWidth = width;
		capture.mHeight = height;
		capture.mTargetBuf = inputImage.surface;
		initCapture(0, &capture);
	}

	void OnUpdate() override
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		THRESHOLD = cursorPos.x;

		//Input:
		{
			doCapture(0);
			while (isCaptureDone(0) == 0);
		}

		//Processing
		{
			//memcpy(processedImage.surface, inputImage.surface, width * height * sizeof(int));
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					int currentBrightness = 0;
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x - 1, y - 1));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x + 0, y - 1));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x + 1, y - 1));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x - 1, y + 0));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x + 0, y + 0));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x + 1, y + 0));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x - 1, y + 1));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x + 0, y + 1));
					currentBrightness += ColourMaths::Brightness(inputImage.GetColour(x + 1, y + 1));
					currentBrightness /= 9;

					int outputColour = currentBrightness > THRESHOLD ? 0x00000000 : 0xFFFFFFFF;

					processedImage.SetColour(x, y, outputColour);
				}
			}
		}

		//PostProcessing
		{
			memcpy(outputImage.surface, processedImage.surface, width * height * sizeof(int));
			//for (int x = 0; x < width; x++) {
			//	for (int y = 0; y < height; y++) {
			//		int processedColour = processedImage.GetColour(x, y);
			//		int outputColour = processedColour == 0x00000000 ? STROKECOLOUR : ColourMaths::LerpColour(TopColour, BottomColour, (float)y / (float)height);
			//		outputImage.SetColour(x, y, outputColour);
			//	}
			//}
		}

		//Output:
		{
			memcpy(pixelBuffer, outputImage.surface, width * height * sizeof(int));
		}
	}

private:
	int nCamera;
	SimpleCapParams capture;
};

int main()
{
	ImageProcessingDisplay display;
	display.Initialize(700, 700, 0x00000000, true);

	while (display.running)
	{
		display.Update();
	}

	display.Shutdown();
	return 0;
}
