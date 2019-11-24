//Basic Acquisition Sample
//The sample will open the first camera attached
//and acquire 5 frames.  Part 2 of the sample will collect
//1 frame of data each time the function is called, looping
//through 5 times.

#define NUM_FRAMES  3
#define NO_TIMEOUT  -1

#include <stdio.h>
#include <picam.h>
#include <fitsio.h>
#include <longnam.h>
#include <stdlib.h>
#include <iostream>

void SaveData(pibyte* buf, piint numframes, piint framelength, piint height, piint width)
{
	unsigned short *tempdata = 0;
	tempdata = (unsigned short*)malloc(sizeof(unsigned short) * framelength / sizeof(pi16u));
	pibyte *frameptr = NULL;
	int totalsize = height * width;
	pi16u *tempptr = NULL;
	fitsfile *fptr;
	int status = 0;
	long fpixel = 1, naxis = 2, nelements;
	long naxes[2] = { height,width};   //Could be width,height but who knows. You'll figure out when it eventually breaks
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
}



void PrintData( pibyte* buf, piint numframes, piint framelength )
{
    pi16u  *midpt = NULL;
    pibyte *frameptr = NULL;

    for( piint loop = 0; loop < numframes; loop++ )
    {
        frameptr = buf + ( framelength * loop );
        midpt = (pi16u*)frameptr + ( ( ( framelength/sizeof(pi16u) )/ 2 ) );
        printf( "%5d,%5d,%5d\t%d\n", (int) *(midpt-1), (int) *(midpt),  (int) *(midpt+1), (int) loop+1 );
    }
}

void something_else()
{
    Picam_InitializeLibrary();

    // - open the first camera if any or create a demo camera
    PicamHandle camera;
    PicamCameraID id;
    const pichar* string;
    PicamAvailableData data;
    PicamAcquisitionErrorsMask errors;
    piint readoutstride = 0;
	piint imageDataWidth = 0;              // - image data width (pixels)
	piint imageDataHeight = 0;             // - image data height (pixels)
	piint pixelDepth = 0;
	pibln committed;

    if( Picam_OpenFirstCamera( &camera ) == PicamError_None )
        Picam_GetCameraID( camera, &id );
    else
    {
        Picam_ConnectDemoCamera(
            PicamModel_Pixis100F,
            "0008675309",
            &id );
        Picam_OpenCamera( &id, &camera );
        printf( "No Camera Detected, Creating Demo Camera\n" );
    }
    Picam_GetEnumerationString( PicamEnumeratedType_Model, id.model, &string );
    printf( "%s", string );
    printf( " (SN:%s) [%s]\n", id.serial_number, id.sensor_name );
    Picam_DestroyString( string );

    Picam_GetParameterIntegerValue( camera, PicamParameter_ReadoutStride, &readoutstride );
	Picam_GetParameterIntegerValue(camera, PicamParameter_PixelBitDepth, &pixelDepth);
	Picam_GetParameterIntegerValue(camera, PicamParameter_SensorActiveWidth, &imageDataWidth);
	Picam_GetParameterIntegerValue(camera, PicamParameter_SensorActiveHeight, &imageDataHeight);
	Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerResponse, PicamTriggerResponse_ReadoutPerTrigger);
	Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerDetermination, PicamTriggerDetermination_RisingEdge);
	printf("Readout stride: %d, Width: %d, Height: %d, Depth: %d \n\n", (int)readoutstride, (int)imageDataWidth, (int)imageDataHeight, (int)pixelDepth);
	printf("%d\n", sizeof(pi16u));
	
	
	Picam_AreParametersCommitted(camera, &committed);
	if (!committed)
	{
		const PicamParameter* failed_parameter_array = NULL;
		piint           failed_parameter_count = 0;
		Picam_CommitParameters(camera, &failed_parameter_array, &failed_parameter_count);
		if (failed_parameter_count)
			Picam_DestroyParameters(failed_parameter_array);
		else
		{
			printf("Waiting for external trigger, requires NUM_FRAMES triggers.\n");
			if (!Picam_Acquire(camera, NUM_FRAMES, NO_TIMEOUT, &data, &errors))
			{
				printf("Received all external trigger\n\n");
			}
		}
	}

	/*FILE *pFile;
	errno_t err;
	if ((err = fopen_s(&pFile, "sample.raw", "wb")) != 0)
	{
		printf("File was not opened\n");
	}
	else
	{
		if (!fwrite(data.initial_readout, 1, (NUM_FRAMES*readoutstride), pFile))
			printf("Data file not saved\n");
	}
	if (pFile)
	{
		fclose(pFile);
	}*/

	SaveData((pibyte*)data.initial_readout, NUM_FRAMES, readoutstride, imageDataHeight, imageDataWidth);
    Picam_CloseCamera( camera );
    Picam_UninitializeLibrary();
}
