/*
	February project: upgrade interactive audio prototype.
*/
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>
#include <windows.h> // for audio
#include <codecvt>

#include <fmod.hpp>

using namespace FMOD;
using namespace std;
using namespace cv;

FMOD::Sound* song;


int main() {
	FMOD_RESULT result;
	FMOD::System* syst = NULL;

	result = FMOD::System_Create(&syst);      // Create the main system object.
	if (result != FMOD_OK)
	{
		printf("FMOD error!");
		exit(-1);
	}

	result = syst->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	if (result != FMOD_OK)
	{
		printf("FMOD error!\n");
		exit(-1);
	}

	FMOD::Sound* song;
	FMOD::Channel* channel;
	syst->createSound("vald-rappel.wav", FMOD_DEFAULT, NULL, &song);

	syst->playSound(song, NULL, false, &channel);


	// Use OpenCV to do webcam loading + motion tracking
	Mat frame, fgMask, grayFrame; // fgMask is for background subtraction
	VideoCapture cap;
	Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorKNN(10); // for background subtraction
	Scalar avgMotion; // to store the mean value of the motion
	
	int deviceID = 0;
	int apiID = CAP_ANY;
	float frequency = 0;
	channel->getFrequency(&frequency);

	cap.open(deviceID, apiID);

	if (!cap.isOpened()) {
		cerr << "Unable to open camera.\n";
		return -1;
	}
	for (;;) {
		frequency = max(2000, frequency - 100); // constrain frequency
		channel->setFrequency(frequency);

		cap.read(frame);

		// update background model
		pBackSub->apply(frame, fgMask);

		if (frame.empty()) {
			cerr << "Error: blank frame grabbed.\n";
		}

		imshow("Live", frame );
		imshow("FG Mask", fgMask);

		// to get a quantitative value, we compute the average pixel brightness in the fgMask
		// closer to 0 means no motion, the higher the number, the more motion there is
		avgMotion = mean(fgMask);
		int avgMotion_int = avgMotion[0];
		// cout << "Average amount of motion: " << avgMotion[0] << endl;


		if (waitKey(5) >= 0) {
			break;
		}
	}

	waitKey(0);
	return 0;
}