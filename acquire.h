#include "cameraClass.h"
#include <picam.h>
#include <string>  

void saveData(pibyte* buf, piint numframes, piint framelength, piint height, piint width);

void acquire(cameraClass &kuroCam, int numFrames);

void cameraFanDisable(cameraClass &kuroCam, bool onOff);

void getCamTemp(cameraClass &kuroCam);

std::string cameraSetupInformation(cameraClass &kuroCam);