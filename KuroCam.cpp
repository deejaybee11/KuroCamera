#include <iostream>
#include <stdio.h>
#include <picam.h>
#include <fitsio.h>
#include <longnam.h>
#include <stdlib.h>
#include <iostream>
#include "cameraClass.h"
#include "acquire.h"
#include "server.h"

/*void saveData(pibyte* buf, piint numframes, piint framelength, piint height, piint width)
{
	unsigned short *tempdata = 0;
	tempdata = (unsigned short*)malloc(sizeof(unsigned short) * framelength / sizeof(pi16u));
	pibyte *frameptr = NULL;
	int totalsize = height * width;
	pi16u *tempptr = NULL;
	fitsfile *fptr;
	int status = 0;
	long fpixel = 1, naxis = 2, nelements;
	long naxes[2] = { height,width };   //Could be width,height but who knows. You'll figure out when it eventually breaks
	nelements = naxes[0] * naxes[1];

	remove("temp.fit");
	fits_create_file(&fptr, "temp.fit", &status);

	for (piint ii = 0; ii < numframes; ii++) {
		frameptr = buf + framelength * ii;
		for (piint zz = 0; zz < totalsize; zz++) {
			tempptr = (pi16u*)frameptr + zz;
			tempdata[zz] = (unsigned short)*tempptr;
		}
		fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
		fits_write_img(fptr, TUSHORT, fpixel, nelements, tempdata, &status);
	}

	fits_report_error(stderr, status);
	fits_close_file(fptr, &status);
	fits_report_error(stderr, status);

	free(tempdata);
}

void acquire(cameraClass &kuroCam, int numFrames)
{

	PicamAcquisitionErrorsMask errors;
	printf("Waiting for external trigger, requires NUM_FRAMES triggers.\n");
	
	if (!Picam_Acquire(kuroCam.camera, numFrames, -1, &kuroCam.data, &errors))
	{
		printf("Received all external trigger\n\n");
	}
	saveData((pibyte*)kuroCam.data.initial_readout, numFrames, kuroCam.readoutstride, kuroCam.imageDataHeight, kuroCam.imageDataWidth);
}*/

int main()
{
	cameraClass kuroCam = cameraClass();
	printf("Camera initialized\n");
	serverClass camSocket = serverClass();

	listen:
	camSocket.startListen(kuroCam);
	if (camSocket.keepServerRunning) {
		printf("Socket died, going back in\n");
		camSocket.setupSocket();
		printf("Socket re-setup\n");
		goto listen;
	}

	return 0;
}
