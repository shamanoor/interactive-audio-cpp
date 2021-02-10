/*
	February project: upgrade interactive audio prototype.
*/
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>
#include <windows.h> // for audio


using namespace std;
using namespace cv;

int main() {
	// Use OpenCV to do webcam loading + motion tracking
	Mat frame, fgMask, grayFrame; // fgMask is for background subtraction
	VideoCapture cap;
	Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorKNN(10); // for background subtraction

	Scalar avgMotion; // to store the mean value of the motion
	int deviceID = 0;
	int apiID = CAP_ANY;

	mciSendString(L"open \"vald-desaccorde.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendString(L"play mp3", NULL, 0, NULL);

	cap.open(deviceID, apiID);

	if (!cap.isOpened()) {
		cerr << "Unable to open camera.\n";
		return -1;
	}
	for (;;) {
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
		cout << "Average amount of motion: " << avgMotion[0] << endl;

		if (waitKey(5) >= 0) {
			break;
		}
	}

	waitKey(0);
	return 0;
}