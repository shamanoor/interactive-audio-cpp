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

const int numDatapoints = 30;
int motionMA;
int motionThreshold;

float scaledSpeed = 0;
float weightedAverage;

array<int, numDatapoints> motions{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

array<int, numDatapoints> weightedMotions{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

FMOD_RESULT result;
FMOD::System* syst = NULL;
FMOD::Channel* channel;

array<int, numDatapoints> initArray(array<int, numDatapoints> array) {
	for (int i = 0; i < numDatapoints; i++) {
		array[i] = 0;
	}
	return array;
}

void initAudio() {
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
}


array<int, numDatapoints> updateMotionArray(int currentMotion, array<int, numDatapoints> motions) {
	for (int i = numDatapoints - 1; i > 0; i--) {
		motions[i] = motions[i - 1];
	}

	motions[0] = currentMotion;
	return motions;
}

array<int, numDatapoints> computeWeightedArray(array<int, numDatapoints> motions) {
	for (int i = 0; i < numDatapoints; i++) {
		motions[i] = motions[i] * (numDatapoints - i);
	}
	return motions;
}

float computeWeightedAverage(array<int, numDatapoints> weightedMotions) {
	int sum = 0;
	int totalWeight = 0;

	for (int i = 0; i < numDatapoints; i++) {
		sum += weightedMotions[i];
		totalWeight += i;
	}

	return sum / totalWeight;
}

float computeScaledSpeed(float value, float sourceRangeMin, float sourceRangeMax, float targetRangeMin, float targetRangeMax) {
	return targetRangeMin + (targetRangeMax - targetRangeMin) * ((value - sourceRangeMin) / (sourceRangeMax - sourceRangeMin));
}

int main() {
	// Use OpenCV to do webcam loading + motion tracking
	Mat frame, fgMask, grayFrame; // fgMask is for background subtraction
	VideoCapture cap;
	Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorKNN(10); // for background subtraction
	Scalar avgMotion; // to store the mean value of the motion
	
	int deviceID = 0;
	int apiID = CAP_ANY;
	float startFrequency = 0;

	int delta = 150;
	motionThreshold = 20;

	// Stuff for audio
	initAudio();

	FMOD::Sound* song;

	syst->createSound("vald-rappel.wav", FMOD_DEFAULT, NULL, &song);
	syst->playSound(song, NULL, false, &channel);
	channel->getFrequency(&startFrequency);

	// initialize frequency to startfrequency so we play the song at normal rate initially
	float frequency = startFrequency; 
	float frequencyMA = startFrequency;

	// open camera
	cap.open(deviceID, apiID);

	if (!cap.isOpened()) {
		cerr << "Unable to open camera.\n";
		return -1;
	}

	for (;;) {
		cap.read(frame);

		if (frame.empty()) {
			cerr << "Error: blank frame grabbed.\n";
		}
		// update background model
		pBackSub->apply(frame, fgMask);

		imshow("Live", frame);
		imshow("FG Mask", fgMask);

		// to get a quantitative value, we compute the average pixel brightness in the fgMask
		// closer to 0 means no motion, the higher the number, the more motion there is
		avgMotion = mean(fgMask);
		int avgMotion_int = int(avgMotion[0]);
		cout << "Average amount of motion: " << avgMotion_int << endl;

		motions = updateMotionArray(avgMotion_int, motions);
		weightedMotions = computeWeightedArray(motions);
		weightedAverage = computeWeightedAverage(weightedMotions);
		
		float factor = computeScaledSpeed(weightedAverage, 0, 40, 0.4, 1.6);
		float playbackspeed = factor * startFrequency;
		
		// update playback speed
		channel->setFrequency(playbackspeed);

		if (waitKey(5) >= 0) {
			break;
		}
	}

	waitKey(0);
	return 0;
}