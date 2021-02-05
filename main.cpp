/*
	February project: upgrade interactive audio prototype.
*/
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>


using namespace std;
using namespace cv;

int main() {
	// Use OpenCV to do webcam loading + motion tracking
	Mat frame, fgMask; // fgMask is for background subtraction
	VideoCapture cap;
	Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorKNN(10); // for background subtraction

	int deviceID = 0;
	int apiID = CAP_ANY;

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

		imshow("Live", frame);
		imshow("FG Mask", fgMask);

		if (waitKey(5) >= 0) {
			break;
		}
	}

	waitKey(0);
	return 0;
}