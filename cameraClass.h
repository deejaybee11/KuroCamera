#pragma once
#include <picam.h>
class cameraClass
{
public:
	cameraClass();
	
	PicamHandle camera;
	piint readoutstride;
	piint imageDataWidth;              // - image data width (pixels)
	piint imageDataHeight;             // - image data height (pixels)
	PicamCameraID id;
	PicamAvailableData data;
	piflt EXPOSURE_TIME;
	piint BIT_DEPTH;

	~cameraClass();
};

