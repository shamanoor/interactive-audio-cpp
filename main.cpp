/*
	February project: upgrade interactive audio prototype.
*/
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

int main() {
	// Use OpenCV to do webcam loading + motion tracking
	Mat frame;
	VideoCapture cap;

	int deviceID = 0;
	int apiID = CAP_ANY;

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

		imshow("Live", frame);

		if (waitKey(5) >= 0) {
			break;
		}
	}

	waitKey(0);
 
	return 0;
}