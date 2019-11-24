#include "acquire.h"
#include <stdio.h>
#include <iostream>
#include <string> 
#include <stdlib.h>
#include <picam.h>
#include "fitsio.h"
#include "cameraClass.h"


void saveData(pibyte* buf, piint numframes, piint framelength, piint height, piint width)
{
	unsigned short *tempdata = 0;
	tempdata = (unsigned short*)malloc(sizeof(unsigned short) * numframes * framelength / sizeof(pi16u));
	pibyte *frameptr = NULL;
	int totalsize = height * width;
	pi16u *tempptr = NULL;
	fitsfile *fptr;
	int status = 0;
	long fpixel = 1, naxis = 3, nelements;
	long naxes[3] = { height,width,numframes };   //Could be width,height but who knows. You'll figure out when it eventually breaks
	nelements = naxes[0] * naxes[1] * naxes[2];

	fits_create_file(&fptr, "Z:\\kuroTemp\\temp.fit", &status);

	unsigned int index = 0;
	for (piint ii = 0; ii < numframes; ii++) {
		//frameptr = buf + framelength * ii;
		for (piint zz = 0; zz < width; zz++) {
			for (piint jj = 0; jj < height; jj++) {
				index = ii * width*height + zz * height + jj;
				tempptr = (pi16u*)buf + index;
				tempdata[index] = (unsigned short)*tempptr;
			}
		}
	}
	fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
	fits_write_img(fptr, TUSHORT, fpixel, nelements, tempdata, &status);

	fits_report_error(stderr, status);
	fits_close_file(fptr, &status);
	fits_report_error(stderr, status);

	free(tempdata);
	printf("Saved temporary fits file\n");
}

void acquire(cameraClass &kuroCam, int numFrames)
{
	remove("Z:\\kuroTemp\\temp.fit");
	PicamAcquisitionErrorsMask errors;
	printf("Waiting for external trigger, requires %d triggers.\n",numFrames);
	getCamTemp(kuroCam);
	cameraFanDisable(kuroCam, true);
	if (!Picam_Acquire(kuroCam.camera, numFrames, -1, &kuroCam.data, &errors))
	{
		printf("Received all external triggers\n");
	}
	cameraFanDisable(kuroCam, false);
	getCamTemp(kuroCam);
	saveData((pibyte*)kuroCam.data.initial_readout, numFrames, kuroCam.readoutstride, kuroCam.imageDataHeight, kuroCam.imageDataWidth);
}

void cameraFanDisable(cameraClass &kuroCam, bool onOff)
{
	pibln committed;
	Picam_SetParameterIntegerValue(kuroCam.camera, PicamParameter_DisableCoolingFan, onOff);
	Picam_AreParametersCommitted(kuroCam.camera, &committed);
	if (!committed)
	{
		const PicamParameter* failed_parameter_array = NULL;
		piint failed_parameter_count = 0;
		Picam_CommitParameters(kuroCam.camera, &failed_parameter_array, &failed_parameter_count);
		if (failed_parameter_count)
			Picam_DestroyParameters(failed_parameter_array);
	}
}

void getCamTemp(cameraClass &kuroCam) {
	piflt temp = 0;
	//PicamEMCalibration_ReadSensorTemperatureReading(kuroCam.camera, temp);
	Picam_GetParameterFloatingPointValue(kuroCam.camera, PicamParameter_SensorTemperatureReading, &temp);
	printf("Temperature is = %f\n", (float)temp);
}

std::string cameraSetupInformation(cameraClass &kuroCam) {
	std::string xsize = std::to_string(kuroCam.imageDataWidth);
	std::string ysize = std::to_string(kuroCam.imageDataHeight);

	std::string theMessage;
	theMessage = "XSIZE:" + xsize + ":YSIZE:" + ysize+":\0";
	
	return theMessage;
}

