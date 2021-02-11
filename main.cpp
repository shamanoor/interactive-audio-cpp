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

	mciSendString(L"open \"vald-rappel.wav\" type mpegvideo alias wav", NULL, 0, NULL);
	mciSendString(L"play wav", NULL, 0, NULL);

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
		int speed = avgMotion[0];
		cout << "Average amount of motion: " << avgMotion[0] << endl;

		string param_value = "0";
		if (speed < 5) {
			param_value = "500";
		}
		else {
			param_value = "1000";
		}

		string param = "set wav speed ";
		param = param + param_value;

		// create the parameter to set the speed
		std::wstring str_turned_to_wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(param);
		LPCWSTR parameter = str_turned_to_wstr.c_str();
	;
		mciSendString(parameter, 0, 0, 0);

		if (waitKey(5) >= 0) {
			break;
		}
	}

	waitKey(0);
	return 0;
}