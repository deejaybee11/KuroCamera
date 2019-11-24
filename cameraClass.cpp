#include "cameraClass.h"
#include <picam.h>
#include <stdio.h>

cameraClass::cameraClass()
{
	Picam_InitializeLibrary();
	const pichar* string;
	pibln committed;

	if (Picam_OpenFirstCamera(&camera) == PicamError_None)
		Picam_GetCameraID(camera, &id);
	else
	{
		printf("No Camera Detected\n");

	}

	EXPOSURE_TIME = 30;
	BIT_DEPTH = 16;

	piint readoutControlMode;

	Picam_GetEnumerationString(PicamEnumeratedType_Model, id.model, &string);
	printf("%s", string);
	printf(" (SN:%s) [%s]\n", id.serial_number, id.sensor_name);
	Picam_DestroyString(string);

	Picam_GetParameterIntegerValue(camera, PicamParameter_ReadoutStride, &readoutstride);
	Picam_GetParameterIntegerValue(camera, PicamParameter_SensorActiveWidth, &imageDataWidth);
	Picam_GetParameterIntegerValue(camera, PicamParameter_SensorActiveHeight, &imageDataHeight);
	Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerResponse, PicamTriggerResponse_ReadoutPerTrigger);
	//Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerResponse, PicamTriggerResponse_ExposeDuringTriggerPulse);
	Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerDetermination, PicamTriggerDetermination_RisingEdge);
	//Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerDetermination, PicamTriggerDetermination_PositivePolarity);
	Picam_SetParameterFloatingPointValue(camera, PicamParameter_ExposureTime, EXPOSURE_TIME);
	Picam_SetParameterIntegerValue(camera, PicamParameter_AdcAnalogGain, PicamAdcAnalogGain_Low);
	Picam_SetParameterIntegerValue(camera, PicamParameter_AdcBitDepth, BIT_DEPTH);
	Picam_SetParameterFloatingPointValue(camera, PicamParameter_ReadoutControlMode, PicamReadoutControlMode_FullFrame);
	Picam_GetParameterIntegerValue(camera, PicamParameter_ReadoutControlMode, &readoutControlMode);
	printf("Readout stride: %d, Width: %d, Height: %d, Readout control mode %d\n\n", (int)readoutstride, (int)imageDataWidth, (int)imageDataHeight, (int)readoutControlMode);
	
	Picam_AreParametersCommitted(camera, &committed);
	if (!committed)
	{
		const PicamParameter* failed_parameter_array = NULL;
		piint failed_parameter_count = 0;
		Picam_CommitParameters(camera, &failed_parameter_array, &failed_parameter_count);
		if (failed_parameter_count)
			Picam_DestroyParameters(failed_parameter_array);
	}
}

cameraClass::~cameraClass()
{
	Picam_CloseCamera(camera);
	Picam_UninitializeLibrary();
}
