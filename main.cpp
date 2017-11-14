#define WINVER 0x0500
#define WM_HOTKEY                       0x0312
#define WM_GETHOTKEY                    0x0033
#define WM_SETHOTKEY                    0x0032
#define SCROLLUP 120
#define SCROLLDOWN -120
#define _CRT_SECURE_NO_WARNINGS 

#include <Windows.h>
#include <Winuser.h> 

#define WINWORD_H_

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
//#include "SerialClass.h"	// Library described above
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

using namespace cv;
using namespace std;
//initial min and max HSV filter values.
//these will be changed using trackbars

HWND window;
RECT rect;
int H_MIN = 31;
int H_MAX = 124;
int S_MIN = 79;
int S_MAX = 256;
int V_MIN = 30;
int V_MAX = 256;
int DilateV = 3;
int ErodeV = 7;
int R = 40;
int G = 255;
int B = 26;
int camI = 0;
bool Sstop = false;

//default capture width and height
const int FRAME_WIDTH = 640;   // this is the camera viode size 
const int FRAME_HEIGHT = 480;
const int Mouse_Speed_UD = 13; //mouse moving speed up and down 
const int Mouse_Speed_LR = 11; //mouse moving speed left and right 
const double SCREEN_WIDTH = 1360;   // this is the dispaly resolution 
const double SCREEN_HEIGHT = 728;
const double X_RANGE = 32; //smaller value gives a faster change
const double Y_RANGE = 1;
const double RESOLUTION_X = 1366;
const double RESOLUTION_Y = 768;
const double CTRL_X = 1000;
const double CTRL_Y = 80;
bool changeY = true; //this is to set if need to track y position

					 // the string can be changed for control instruction 
string command1 = "LEFT";
string command2 = "UP";
string command3 = "DOWN";
string command4 = "Right";
string command5 = "Click";
string command6 = "";

//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 20;                             //make sure no noise to interupt

															//minimum and maximum object area
const int MIN_OBJECT_AREA = 5 * 5;                        //no need
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5; //no need

															//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";
const string windowName4 = "Cursor Control Panel";
bool objectFound = false;          // if is tracking the object
bool cali = true;                  // true when need to calibtaion 
bool clickOne = true;
HWND hwnd1, hwnd2, hwnd3;

int x_min, x_max, y_min, y_max, x_min1, x_max1, y_min1, y_max1, x_o, y_o;
int x_1, x_2, y_1, y_2;
double Spacex_1, Spacex_2, Spacex_3, Spacey_1, Spacey_2;
int x_screen, y_screen; // real coordinate in screen
int x_screen1, y_screen1; // coordinate for control panel 
int stop = 0;  // to count the time to shut down system automatically 
int userOpt, methodOpt; // option for operation mode 
int cursor_x = 683, cursor_y = 384;



//Gesture Control Para
int upS = 20, downS = 10, leftS = 30, rightS = 25;
int mX, mY;
int oX = -1;
int oY = -1;
bool oValueSet = false;

double dx = 0, dy = 0;
POINT p;
Mat cameraFeed;

//vairables for meanshift tracking 
bool selectObject = false;
int trackObject = 0;
bool showHist = true;
Point origin;
Rect selection;

Mat frame, hsv, hue, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
Rect trackWindow;

static bool start_timeg0 = true;
static clock_t startg0;
static double durationg0;
static bool start_timeg1 = true;
static clock_t startg1;
static double durationg1;
static bool start_timeg2 = true;
static clock_t startg2;
static double durationg2;
static bool start_timeg3 = true;
static clock_t startg3;
static double durationg3;
static bool start_timeg4 = true;
static clock_t startg4;
static double durationg4;
static bool mouse1on = false, mouse2on = false, mouse3on = false, mouse4on = false;

int n = 100, ia = 0;
int rnum[100] = { 0 }, cnum[100] = { 0 }, sum = 0, raverage, caverage;
bool MouseOn1 = false;
int dsc0 = 200, dsc1 = 200, dsc2 = 200, dsc3 = 200, dsc4 = 200;

void pressKeyCtrl(char mK)
{
	//creat a generic key board structure
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//while(1)
	//for (int i=1; i<2; ++i)
	{
		//press "Ctrl"
		ip.ki.wVk = VK_CONTROL;
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
		//press "+"
		ip.ki.wVk = VK_ADD;
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
		//release "+"
		ip.ki.wVk = VK_ADD;
		ip.ki.dwFlags = KEYEVENTF_KEYUP; // '+' key release 
		SendInput(1, &ip, sizeof(INPUT));
		//release "Ctrl"
		ip.ki.wVk = VK_CONTROL;
		ip.ki.dwFlags = KEYEVENTF_KEYUP; // 'Ctrl' key release 
		SendInput(1, &ip, sizeof(INPUT));

		Sleep(1000);
	}

}
void pressKeyCtrlm(char mK)
{
	//creat a generic key board structure
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//while(1)
	//for (int i=1; i<2; ++i)
	{
		//press "Ctrl"
		ip.ki.wVk = VK_CONTROL;
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
		//press "-"
		ip.ki.wVk = VK_SUBTRACT;
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
		//release "-"
		ip.ki.wVk = VK_SUBTRACT;
		ip.ki.dwFlags = KEYEVENTF_KEYUP; // '+' key release 
		SendInput(1, &ip, sizeof(INPUT));
		//release "Ctrl"
		ip.ki.wVk = VK_CONTROL;
		ip.ki.dwFlags = KEYEVENTF_KEYUP; // 'Ctrl' key release 
		SendInput(1, &ip, sizeof(INPUT));

		//Sleep(1000);
	}

}

void pressKeyB(char mK)
{
	HKL kbl = GetKeyboardLayout(0);
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.dwFlags = KEYEVENTF_UNICODE;
	if ((int)mK<65 && (int)mK>90) //for lowercase
	{
		ip.ki.wScan = 0;
		ip.ki.wVk = VkKeyScanEx(mK, kbl);
	}
	else //for uppercase
	{
		ip.ki.wScan = mK;
		ip.ki.wVk = 0;

	}
	ip.ki.dwExtraInfo = 0;
	SendInput(1, &ip, sizeof(INPUT));
}

void pressEnter()
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.dwFlags = KEYEVENTF_UNICODE;
	ip.ki.wScan = VK_RETURN; //VK_RETURN is the code of Return key
	ip.ki.wVk = 0;

	ip.ki.dwExtraInfo = 0;
	SendInput(1, &ip, sizeof(INPUT));

}

void SetCursorSize(const int iSize = 0)
{
	HANDLE cxHandle;
	cxHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;

	cci.dwSize = iSize;
	cci.bVisible = true;
	SetConsoleCursorInfo(cxHandle, &cci);


}//End SetCursorSize  

void  endtime()
{
	SetCursorPos(mX, mY);
	start_timeg1 = true;
	durationg1 = 0;


}
void on_trackbar(int, void*)
{//This function gets called whenever a
 // trackbar position is changed
}

string intToString(int number) {

	std::stringstream ss;
	ss << number;
	return ss.str();
}

//functions: 

//to create the control HSV bar
void createTrackbars() {
	//create window for trackbars

	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf_s(TrackbarName, "H_MIN", H_MIN);
	sprintf_s(TrackbarName, "H_MAX", H_MAX);
	sprintf_s(TrackbarName, "S_MIN", S_MIN);
	sprintf_s(TrackbarName, "S_MAX", S_MAX);
	sprintf_s(TrackbarName, "V_MIN", V_MIN);
	sprintf_s(TrackbarName, "Dilate", DilateV);
	sprintf_s(TrackbarName, "Erode", ErodeV);
	sprintf_s(TrackbarName, "R", R);
	sprintf_s(TrackbarName, "G", G);
	sprintf_s(TrackbarName, "B", B);
	sprintf_s(TrackbarName, "If show cam img", camI);


	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, 255, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, 255, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, 255, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, 256, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, 256, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, 256, on_trackbar);
	createTrackbar("Dilate", trackbarWindowName, &DilateV, 10, on_trackbar);
	createTrackbar("Erode", trackbarWindowName, &ErodeV, 10, on_trackbar);
	createTrackbar("R Value", trackbarWindowName, &R, 255, on_trackbar);
	createTrackbar("G Value", trackbarWindowName, &G, 255, on_trackbar);
	createTrackbar("B Value", trackbarWindowName, &B, 255, on_trackbar);
	createTrackbar("Show img", trackbarWindowName, &camI, 1, on_trackbar);
}

//to draw ROI 
void drawObject(int x, int y, Mat &frame) {

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	//added 'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame, Point(x, y), 20, Scalar(0, 0, 255), 2);
	if (y - 25>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 0, 255), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 0, 255), 2);
	if (y + 25<FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 0, 255), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 0, 255), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 0, 255), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 0, 255), 2);
	if (x + 25<FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 0, 255), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 0, 255), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 0, 255), 2);

}

//doing morphOps to the roi
void morphOps(Mat &thresh) {
	/*
	Erode and Dialate:
	para1: shape: MORPH_RECT MORPH_CROSS  MORPH_ELLIPSE

	*/

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_ELLIPSE, Size(ErodeV, ErodeV));
	//dilate with larger element so make sure object is nicely visible                      
	Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(DilateV, DilateV));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);

}

//do the tracking things 
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed, Mat & wholePanel) {

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	//bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px th
				//en it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {                    //MAKE SURE IS TRACKING CORRECTLY
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
					if (x >= 1280 || y >= 720)                                                       //need to change according to camera size 
						objectFound = false;
				}
				else objectFound = false;

			}
			//let user know you found an object
			if (objectFound == true) {
				putText(wholePanel, "Tracking Mode", Point(80, 980), 2, 1, Scalar(160, 210, 60), 2);
				//draw object location on screen
				//	drawObject(x, y, cameraFeed);
			}
		}
		else putText(wholePanel, "PLEASE ADJUST FILTER", Point(80, 980), 2, 1, Scalar(0, 0, 255), 2);
	}
}

// this function is to create a green window for main panel 
void createPanel(Mat &panel, int height, int weight) {
	rectangle(panel, Point(0, 0), Point(height, weight), Scalar(B, G, R), CV_FILLED, 8);
}


int controlGesture1(int x, int y, Mat &camera) {
	HCURSOR WINAPI GetCursor(void);

	//putText(camera, "Controlling", Point(80, 900), 2, 1, Scalar(160, 210, 60), 2);
	char strx[20] = { 0 };

	HWND window;
	RECT rect;
	POINT pt;
	window = GetForegroundWindow();
	GetCursorPos(&p);

	mX = p.x;
	mY = p.y;
	//ScreenToClient(window, &p);

	rectangle(camera, Point(mX - 220, mY - 30), Point(mX - 20, mY + 30), Scalar(255, 400, 255), 1, 18);   //x-axies width 
	rectangle(camera, Point(mX - 90, mY - 130), Point(mX - 150, mY + 130), Scalar(255, 400, 255), 1, 18); //y-axies width +-30 for x-axis

	int cX = x;
	int cY = y;



	n = 20;
	int sum = 0;

	ia = 0;
	sum = 0;
	rnum[1] = x;

	for (int k = 100; k > ia; k--) {
		rnum[k] = rnum[k - 1];
	}

	for (ia = 0; ia < n; ++ia)
	{

		sum += rnum[ia];
	}

	raverage = sum / n;

	sum = 0;
	ia = 0;
	cnum[1] = y;

	for (int k = 100; k > ia; k--) {
		cnum[k] = cnum[k - 1];
	}

	for (ia = 0; ia < n; ++ia)
	{

		sum += cnum[ia];
	}

	caverage = sum / n;

	int ddx = mX - x - 460;
	int ddy = mY - y;
	int dd = 120;

	// start of mouse focus zone
	if (abs(ddx)<dd && abs(ddy) <dd) {
		if (start_timeg0) {
			startg0 = clock();
			start_timeg0 = false;
		}
		durationg0 = (double)((clock() - startg0) / CLOCKS_PER_SEC);
		if (durationg0>1)
		{
			static HCURSOR  hCursorWE;
			static HCURSOR  hCursorNS;
			//The value is set here (in the handle for WM_CREATE):
			hCursorWE = LoadCursor(NULL, IDC_HAND);
			hCursorNS = LoadCursor(NULL, IDC_SIZENS);
			//if (abs(ddx)>abs(ddy))
			//SetCursor(hCursorWE);
			//else if (abs(ddx)<=abs(ddy))
			//SetCursor(hCursorNS);
			SetCursor(hCursorWE);
			//pressKeyB('v');
			//	endtime;
			start_timeg0 = true;
			durationg0 = 0;

			MouseOn1 = false;
			dsc0 = 100;
		}
	} // end of mouse focus zone

	  // mouse move action
	MouseOn1 = false;
	if (MouseOn1 == true && mX >460 && mX <1680 && mY > 200 && mY <1050)

	{
		int nn = 1;
		if (ddx>dd && ddx<2.5*dd && abs(ddy)<dd) {
			rectangle(camera, Point(mX - 120, mY - 30), Point(mX, mY + 30), Scalar(255, dsc1, 255), 1, 18);
			if (start_timeg1) {
				startg1 = clock();
				start_timeg1 = false;
			}
			durationg1 = (double)((clock() - startg1) / CLOCKS_PER_SEC);
			if (durationg1 >1) {
				mouse1on = true;

				for (ia = 0; ia < nn; ++ia) {

					mX = mX - Mouse_Speed_LR;
				}


				dsc1 = 100;
				SetCursorPos(mX, mY);
			}
		}
		else  if (ddx > -2.5*dd && ddx<-dd && abs(ddy)<dd) {
			rectangle(camera, Point(mX - 240, mY - 30), Point(mX - 120, mY + 30), Scalar(255, dsc1, 255), 1, 18);
			if (start_timeg2) {
				startg2 = clock();
				start_timeg2 = false;
			}
			durationg2 = (double)((clock() - startg2) / CLOCKS_PER_SEC);
			if (durationg2 >1) {
				for (ia = 0; ia < nn; ++ia)
					mX = mX + Mouse_Speed_LR;


				dsc2 = 100;
				SetCursorPos(mX, mY);
			}
		}
		else if (ddy>dd && ddy < 2.5*dd && abs(ddx)<dd) {
			rectangle(camera, Point(mX - 180, mY - 100), Point(mX - 60, mY), Scalar(255, dsc3, 255), 1, 18); //y-axies
			if (start_timeg3) {
				startg3 = clock();
				start_timeg3 = false;
			}
			durationg3 = (double)((clock() - startg3) / CLOCKS_PER_SEC);
			if (durationg3 >1) {

				for (ia = 0; ia < nn; ++ia)
					mY = mY - Mouse_Speed_UD;


				dsc3 = 100;
				SetCursorPos(mX, mY);
			}
		}
		else if (ddy > -2.5*dd && ddy<-dd && abs(ddx)<dd) {
			if (start_timeg4) {
				rectangle(camera, Point(mX - 180, mY), Point(mX - 60, mY + 100), Scalar(255, dsc4, 255), 1, 18); //y-axies
				startg4 = clock();
				start_timeg4 = false;
			}
			durationg4 = (double)((clock() - startg4) / CLOCKS_PER_SEC);
			if (durationg4 >1) {

				for (ia = 0; ia < nn; ++ia)
					mY = mY + Mouse_Speed_UD;

				dsc4 = 100;
				SetCursorPos(mX, mY);
			}
		}

		if (mouse1on = true && 2 * dd<ddx && ddx<dd)
		{
			start_timeg1 = true;
			durationg1 = 0;

			mouse1on = false;
		}
		else if (mouse2on = true && -2 * dd>ddx && ddx>-dd)
		{
			start_timeg2 = true;
			durationg2 = 0;

			mouse2on = false;
		}
		else if (mouse3on = true && 2 * dd<ddy && ddy<dd)
		{
			start_timeg3 = true;
			durationg3 = 0;

			mouse3on = false;
		}
		else if (mouse4on = true && -2 * dd>ddy && ddy>-dd)
		{
			start_timeg4 = true;
			durationg4 = 0;

			mouse4on = false;
		}
	} //end of mouse move action




	int xxd = mX - 460 - raverage, yyd = mY - caverage;

	sprintf_s(strx, "%d", xxd);
	cout << strx;
	//putText(camera, strx , Point(x,y),cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100,255,dsc1), 5, 8);
	sprintf_s(strx, "%d", (yyd));
	cout << strx;
	//putText(camera, strx , Point(x+200, y),cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100,255,dsc1), 5, 8);







	//sprintf_s(strx, "%d",xxd);
	//cout <<strx;			
	//putText(camera, strx , Point(x, y+100),cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100,255,dsc), 5, 8);
	//sprintf_s(strx, "%d",(yyd));
	//cout <<strx;			
	//putText(camera, strx , Point(x+200, y+100),cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100,255,dsc), 5, 8);



	if (durationg1 >10000000)
	{
		sprintf_s(strx, "%d", x);
		cout << strx;
		putText(camera, strx, Point(480, 1000), cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100, 255, 100), 5, 8);
		sprintf_s(strx, "%d", (y));
		cout << strx;
		putText(camera, strx, Point(680, 1000), cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100, 255, 100), 5, 8);
		sprintf_s(strx, "%d", (mX));
		cout << strx;
		putText(camera, strx, Point(480, 800), cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100, 255, 100), 5, 8);
		sprintf_s(strx, "%d", (mY));
		cout << strx;
		putText(camera, strx, Point(680, 800), cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100, 255, 100), 5, 8);

	}
	return 0;
}
// selection for control panel gesture contrl 

// selection for control panel coordinates 
int control(int x, int y, Mat &camera) {

	putText(camera, "Controlling", Point(80, 650), 2, 1, Scalar(160, 210, 60), 2);
	GetCursorPos(&p);
	char strx[20] = { 0 };
	sprintf_s(strx, "%d", p.x);
	cout << strx;
	putText(camera, strx, Point(80, 690), cv::FONT_HERSHEY_SIMPLEX, 1.5, Scalar(100, 255, 100), 5, 8);



	//if (y < 500){
	if (x < 200)
	{
		return 1;
	}
	else if (x > 200 && x < 400)
	{
		return 2;
	}
	else if (x > 400 && x < 600)
	{
		return 3;
	}
	else if (x > 600 && x < 800)
	{
		return 4;
	}
	//else if (x > 0 && x < 1000)
	else if (x > 600)
	{
		return 5;
	}
	else
		return 0;
	//}
	//else if ( y >500)
	//  return 0; 

	//	else
	return 0;
}

char controlC(int x, int y, Mat &camera) {

	putText(camera, "Controlling", Point(80, 650), 2, 1, Scalar(160, 210, 60), 2);

	if (y < 500) {
		if (x > 0 && x < 200)  //left
		{
			return '1';
		}
		else if (x > 0 && x < 400) //forward
		{
			return '2';
		}
		else if (x > 0 && x < 600) //backward
		{
			return'3';
		}
		else if (x > 0 && x < 800) //right
		{
			return '4';
		}
		else
			return '5';  //stop 
	}
	else
		return '5';  //stop 

}


// doing eye stare  calibration 

void calibration(int x, int y, Mat &cameraFeed, double duration) {
	putText(cameraFeed, "Calibration:", Point(80, 650), 4, 1, Scalar(0, 0, 255), 2);

	if (duration < 1)
	{
		putText(cameraFeed, "Please stare at the center:", Point(80, 690), FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 255), 2);
		x_o = x;
		y_o = y;
	}

	else if (duration < 3 && objectFound == true)
	{
		//dx = SCREEN_WIDTH / 2 - x_screen;
		//dy = SCREEN_HEIGHT / 2 - y_screen;
		x_screen = (x - x_o) / X_RANGE*(RESOLUTION_X / 2) + SCREEN_WIDTH / 2 + dx;
		y_screen = (y - y_o) / Y_RANGE*(RESOLUTION_Y / 2) + SCREEN_HEIGHT / 2 + dy;


	}
}

//screensize is used to calculate the x,y coordinate in real screen display 
void screensize(int & x_screen, int & y_screen, int x, int y) {
	x_screen = (x - x_o) / X_RANGE*(RESOLUTION_X / 2) + SCREEN_WIDTH / 2 + dx;
	y_screen = (y - y_o) / X_RANGE*(RESOLUTION_Y / 2) + SCREEN_HEIGHT / 2 + dy;
}

//control box program is used to divide the control panel into num region 
void controlbox(Mat &output, int num) {
	//rectangle(output, Point(130, 0), Point(CTRL_X+130, CTRL_Y), Scalar(0, 0, 0), CV_FILLED, 8);
	//rectangle(output, Point(130, CTRL_Y/2), Point(CTRL_X+130, CTRL_Y), Scalar(0, 0, 0), CV_FILLED, 8);
	int num1 = 17;
	for (int n = 0; n < num1; n++) {
		//line(output, Point(n*CTRL_X / num1+130, 0), Point(n*CTRL_X / num1+130, CTRL_Y), Scalar(0, 0, 255));
		rectangle(output, Point(130 + n * 100, 50), Point(230 + n * 100, 90), Scalar(55, 55, 55), 1, 18);
		rectangle(output, Point(130 + n * 100, 10), Point(230 + n * 100, 50), Scalar(55, 55, 55), 1, 18);
	}


	putText(output, command1, Point(200, 40), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, command2, Point(400, 40), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, command3, Point(600, 40), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, command4, Point(800, 40), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, command5, Point(1000, 40), 2, 1, Scalar(0, 0, 255), 2);

	putText(output, "Z+", Point(160, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "Z-", Point(260, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "R+", Point(360, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "R-", Point(460, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "Ex", Point(560, 80), 2, 1, Scalar(0, 0, 255), 2);

	putText(output, "a", Point(860, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "b", Point(960, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "c", Point(1060, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "d", Point(1160, 80), 2, 1, Scalar(0, 0, 255), 2);
	putText(output, "e", Point(1260, 80), 2, 1, Scalar(0, 0, 255), 2);
}




//this function is used to control cursor 
void cursor(int input) {
	GetCursorPos(&p);
	cursor_x = p.x;
	cursor_y = p.y;
	switch (input) {
	case 1:  //key left
		cursor_x = cursor_x - Mouse_Speed_LR;
		SetCursorPos(cursor_x, cursor_y);

		break;
	case 2: // key up
		cursor_y = cursor_y - Mouse_Speed_UD;
		SetCursorPos(cursor_x, cursor_y);
		break;
	case 3:  // key down
		cursor_y = cursor_y + Mouse_Speed_UD;
		SetCursorPos(cursor_x, cursor_y);
		break;
	case 4:  // key right
		cursor_x = cursor_x + Mouse_Speed_LR;
		SetCursorPos(cursor_x, cursor_y);
		break;
	case 5:
		mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_x, cursor_y, 0, 0); // moving left click
		mouse_event(MOUSEEVENTF_LEFTUP, cursor_x, cursor_y, 0, 0); // moving cursor leftup 
		break;
	case 6:
		mouse_event(MOUSEEVENTF_RIGHTDOWN, cursor_x, cursor_y, 0, 0); // cursor right click
		mouse_event(MOUSEEVENTF_RIGHTUP, cursor_x, cursor_y, 0, 0);
		break;
	default: // not arrow
		break;
	}
	Sleep(20);
}

// time fucntion for staring counting and confirmation
void ctrlSelect(int input, Mat ctrlPanel) {
	static bool start_time2 = false;
	static clock_t startx;
	static double duration2;
	static int input1 = 0;
	static int Gselection = 0;

	//Global Cursor Actions
	GetCursorPos(&p);
	mX = p.x;
	mY = p.y;
	if (40 <= mY && mY <= 120)
	{
		if (250 <= mX && mX <= 350)
		{
			rectangle(ctrlPanel, Point(130, 50), Point(230, 90), Scalar(255, 255, 255), 1, 18);
			Gselection = 1;
		}
		if (350 <= mX && mX <= 450)
		{
			rectangle(ctrlPanel, Point(230, 50), Point(330, 90), Scalar(255, 255, 255), 1, 18);
			Gselection = 2;
		}
		if (450 <= mX && mX <= 550)
		{
			rectangle(ctrlPanel, Point(330, 50), Point(430, 90), Scalar(255, 255, 255), 1, 18);
			Gselection = 3;
		}
		if (550 <= mX && mX <= 650)
		{
			rectangle(ctrlPanel, Point(430, 50), Point(530, 90), Scalar(255, 255, 255), 1, 18);
			Gselection = 4;
		}
		if (650 <= mX && mX <= 750)
		{
			rectangle(ctrlPanel, Point(530, 50), Point(630, 90), Scalar(255, 255, 255), 1, 18);
			Gselection = 5;
		}
		if (750 <= mX && mX <= 850)
		{
			rectangle(ctrlPanel, Point(530, 50), Point(630, 90), Scalar(255, 255, 255), 1, 18);
			Gselection = 6;
		}
	}
	else
		Gselection = 100;


	rectangle(ctrlPanel, Point((input - 1) * 200 + 130, 0), Point(input * 200 + 130, 40), Scalar(156, 195, 165), CV_FILLED, 8);

	if (start_time2) {
		startx = clock();
		start_time2 = false;
	}

	duration2 = (double)(clock() - startx) / CLOCKS_PER_SEC;

	if (input == input1) {

		if (input == 5 && clickOne && duration2 >1)
		{
			cursor(input);
			rectangle(ctrlPanel, Point((input - 1) * 200 + 130, 0), Point(input * 200 + 130, 40), Scalar(232, 144, 144), CV_FILLED, 8);
			clickOne = false;
			if (Gselection == 1)
			{

				//window=GetForegroundWindow();
				//mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_x, cursor_y, 0, 0); // moving left click
				//mouse_event(MOUSEEVENTF_LEFTUP, cursor_x, cursor_y, 0, 0); // moving cursor leftup 
				//GetWindowRect(window,&rect);
				//ShowWindow(window,SW_SHOWNORMAL);
				SetForegroundWindow(window);
				SetActiveWindow(window);
				SetWindowPos(window,       // handle to window
					HWND_TOPMOST,  // placement-order handle
					0,     // horizontal position
					200,      // vertical position
					1400,  // width
					900, // height
					SWP_SHOWWINDOW | SWP_NOMOVE// window-positioning options
				);
				p.x = 500;
				p.y = 500;
				SetCursorPos(p.x, p.y);
				ShowWindow(window, SW_SHOWNORMAL);
				pressKeyCtrl('+');
				p.x = mX;
				p.y = mY;
				SetCursorPos(p.x, p.y);
			}
			else if (Gselection == 2)
			{

				//mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_x, cursor_y, 0, 0); // moving left click
				//mouse_event(MOUSEEVENTF_LEFTUP, cursor_x, cursor_y, 0, 0); // moving cursor leftup 
				//window=GetForegroundWindow();
				//GetWindowRect(window,&rect);
				//ShowWindow(window,SW_SHOWNORMAL);

				SetForegroundWindow(window);
				SetActiveWindow(window);
				p.x = 500;
				p.y = 500;
				SetCursorPos(p.x, p.y);
				ShowWindow(window, SW_SHOWNORMAL);
				pressKeyCtrlm('-');
				p.x = mX;
				p.y = mY;
				SetCursorPos(p.x, p.y);
			}

			else if (Gselection == 3)
			{
				p.x = 500;
				p.y = 500;
				SetCursorPos(p.x, p.y);
				//window=GetForegroundWindow();
				//GetWindowRect(window,&rect);	
				ShowWindow(window, SW_SHOWNORMAL);
				SetForegroundWindow(window);
				SetActiveWindow(window);

				for (int k = 0; k <2; ++k) {
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, SCROLLDOWN, 0);
				}
				p.x = mX;
				p.y = mY;
				SetCursorPos(p.x, p.y);
			}
			else if (Gselection == 4)
			{	//Start WINWORD2016
				//SHELLEXECUTEINFO ShExecInfo = {0};
				//ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				//ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				//ShExecInfo.hwnd = NULL;
				//ShExecInfo.lpVerb = NULL;
				//ShExecInfo.lpFile = "C:\\Program Files\\Microsoft Office\\Office16\\WINWORD.EXE";        
				//ShExecInfo.lpParameters = "";   
				//ShExecInfo.lpDirectory = NULL;
				//ShExecInfo.nShow = SW_SHOW;
				//ShExecInfo.hInstApp = NULL; 
				//ShellExecuteEx(&ShExecInfo);
				//WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

				p.x = 500;
				p.y = 500;
				SetCursorPos(p.x, p.y);
				//window=GetForegroundWindow();
				//GetWindowRect(window,&rect);
				ShowWindow(window, SW_SHOWNORMAL);
				SetForegroundWindow(window);
				SetActiveWindow(window);
				for (int k = 0; k <2; ++k) {
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, SCROLLUP, 0);
				}
				p.x = mX;
				p.y = mY;
				SetCursorPos(p.x, p.y);
			}
			else if (Gselection == 5) {

				ShowWindow(window, SW_RESTORE);
			}
			else if (Gselection == 6) {

				exit(EXIT_FAILURE);
				return;
			}
		}

		else if (input != 5 && duration2 > 1)
		{
			cursor(input);
			rectangle(ctrlPanel, Point((input - 1) * 200 + 130, 0), Point(input * 200 + 130, 40), Scalar(232, 144, 144), CV_FILLED, 8);
		}
		else if (input != 5)
			clickOne = true;
	}
	else
	{
		input1 = input;
		startx = clock();
	}

}


//functions for Meanshift: 
static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);

		selection &= Rect(0, 0, cameraFeed.cols, cameraFeed.rows);
	}

	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;
		break;
	case EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1;   // Set up CAMShift properties in main() loop
		break;
	}
}

bool track(int &x, int &y, Mat &threshold) {

	bool trackResult = false;
	int area = 0;
	int hsize = 16;
	float hranges[] = { 0, 180 };
	const float* phranges = hranges;

	if (trackObject)
	{

		int ch[] = { 0, 0 };
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);

		if (trackObject < 0)
		{
			// Object has been selected by user, set up CAMShift search properties once
			Mat roi(hue, selection), maskroi(threshold, selection);
			calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
			normalize(hist, hist, 0, 255, NORM_MINMAX);

			trackWindow = selection;
			trackObject = 1; // Don't set up again, unless user selects new ROI
			trackResult = true;
			histimg = Scalar::all(0);
			int binW = histimg.cols / hsize;
			Mat buf(1, hsize, CV_8UC3);
			for (int i = 0; i < hsize; i++)
				buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
			cvtColor(buf, buf, COLOR_HSV2BGR);

			for (int i = 0; i < hsize; i++)
			{
				int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);
				rectangle(histimg, Point(i*binW, histimg.rows),
					Point((i + 1)*binW, histimg.rows - val),
					Scalar(buf.at<Vec3b>(i)), -1, 8);
			}
		}

		// Perform CAMShift
		calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
		backproj &= threshold;
		RotatedRect trackBox = CamShift(backproj, trackWindow,
			TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
		x = trackBox.center.x;
		y = trackBox.center.y;
		area = trackBox.size.height * trackBox.size.width;
		if (trackWindow.area() <= 1)
		{
			int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
			trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
				trackWindow.x + r, trackWindow.y + r) &
				Rect(0, 0, cols, rows);
		}
		//here to add the conditions if trackWindw larger or smaller. 



		else
			ellipse(cameraFeed, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
	}


	if (selectObject && selection.width > 0 && selection.height > 0)
	{
		Mat roi(cameraFeed, selection);
		bitwise_not(roi, roi);
	}

	if (area > 10)
	{
		//	trackObject = 0;
		trackResult = true;
	}

	else if (area <= 10)
	{
		//	trackObject = 0;
		trackResult = false;
	}
	return trackResult;

}

//Opens Word minimized, shows the user a dialog box to allow them to
//select the printer, number of copies, etc., and then closes Word
void wordprint(char* filename) {
	char* command = new char[64 + strlen(filename)];
	strcpy(command, "start /min winword \"");
	strcat(command, filename);
	strcat(command, "\" /q /n /f /mFilePrint /mFileExit");
	system(command);
	delete command;
}

//Opens the document in Word
void wordopen(char* filename) {
	char* command = new char[64 + strlen(filename)];
	strcpy(command, "start /max winword \"");
	strcat(command, filename);
	strcat(command, "\" /q /n");
	system(command);
	delete command;
}

//Opens a copy of the document in Word so the user can save a copy
//without seeing or modifying the original
void wordduplicate(char* filename) {
	char* command = new char[64 + strlen(filename)];
	strcpy(command, "start /max winword \"");
	strcat(command, filename);
	strcat(command, "\" /q /n /f");
	system(command);
	delete command;
}
void wordstart() {
	//Start WINWORD2016
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = window;
	ShExecInfo.lpVerb = NULL;
	//ShExecInfo.lpFile = "C:\\Program Files\\Microsoft Office\\Office16\\WINWORD.EXE";
	//ShExecInfo.lpParameters = "";
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);


	//Set ForegroundWindow (Handle=window)
	p.x = 500;
	p.y = 500;
	SetCursorPos(p.x, p.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_x, cursor_y, 0, 0); // moving left click
	window = GetForegroundWindow();

	//mouse_event(MOUSEEVENTF_LEFTUP, cursor_x, cursor_y, 0, 0); // moving cursor leftup 
	GetWindowRect(window, &rect);
	SetForegroundWindow(window);
	SetActiveWindow(window);
	//pressKeyCtrl('+');	
	//		ShowWindow(window,SW_MINIMIZE);


}

int main(int argc, char* argv[])
{
	//first to initial varibales 

#pragma region variables
	//some boolean variables for different functionality within this
	bool trackObjects = true;     //if need to track objects 
	bool useMorphOps = true;      // tracking shadows 
								  //bool isTracking = false;    
	bool start_time = true;       //calibration time function, true when calibration starts 
								  //static bool start_time2 = true;      //this is for selection output 
	bool startControl = false;    // if start to control the arduino, true when calibration finish, false when track lost 
	bool ArduinoConnect = false;  //state if arduino is connected to computer 
	bool flipI = false;            // if need to flip the camera 
	bool move = true;                    //to move for once only 
										 //Matrix to store each frame of the webcam feed


										 //wordduplicate("c:\\Program Files\\MSOffice\\name.doc");

										 //system("C:\\Program Files\\Microsoft Office\\Office16\\WINWORD.EXE");

										 //ShellExecuteExW("C:\\Program Files\\Microsoft Office\\Office16\\WINWORD.EXE");

										 //Start WINWORD2016
										 //SHELLEXECUTEINFO ShExecInfo = {0};
										 //ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
										 //ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
										 //ShExecInfo.hwnd = NULL;
										 //ShExecInfo.lpVerb = NULL;
										 //ShExecInfo.lpFile = "C:\\Program Files\\Microsoft Office\\Office16\\WINWORD.EXE";        
										 //ShExecInfo.lpParameters = "";   
										 //ShExecInfo.lpDirectory = NULL;
										 //ShExecInfo.nShow = SW_SHOW;
										 //ShExecInfo.hInstApp = NULL; 
										 //ShellExecuteEx(&ShExecInfo);
										 //WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

										 //matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold, temp, inverse;
	//x and y values for the location of the object
	int x = 0, y = 0;
	//create slider bars for HSV filtering
	int x_left, x_right, y_up, y_down, x_center, y_center;
	//these are for calibration
	createTrackbars();
	//video capture object to acquire webcam feed
	double duration;
	clock_t start;
	int timex;
	//is for counitng time display
	int input = 0; // control input 
	char inputC = '0'; // for arduino character input
	char calibrationOption;
	bool Option1 = true;
	bool result = false; // this is for meanshift result tracking 
#pragma endregion
						 //wordstart;



						 //open window edge
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	SHELLEXECUTEINFOW sei = { sizeof sei };
	sei.lpVerb = L"open";
	sei.lpFile = L"microsoft-edge:http://www.bbc.com";
	ShellExecuteExW(&sei);

	//wordstart;


	//Set ForegroundWindow (Handle=window)


	p.x = 500;
	p.y = 500;
	SetCursorPos(p.x, p.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, cursor_x, cursor_y, 0, 0); // moving left click
	window = GetForegroundWindow();

	//mouse_event(MOUSEEVENTF_LEFTUP, cursor_x, cursor_y, 0, 0); // moving cursor leftup 
	GetWindowRect(window, &rect);
	SetForegroundWindow(window);
	SetActiveWindow(window);
	//pressKeyCtrl('+');	
	ShowWindow(window, SW_MINIMIZE);

#pragma region console_display
	VideoCapture capture;
	capture.open(0);
	if (!capture.isOpened())
	{
		printf("Error in opening the camera, please check!  \n");
		system("pause");
		return -1;
	}





	// to select the opertation mode: 

	userOpt = 1;
	methodOpt = 1;
	//open capture object at location zero (default location for webcam)

	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop	
	cout << "you may enter your instruction ";

#pragma endregion 

#pragma region arduinoSetUp
/*	Serial* port = new Serial("COM4"); //connect to arduino        Here to change the port number 
									   //Arduino Communication set up:
	char data[8] = "";
	char command[2] = "";
	int datalength = 8;  //length of the data,
	int readResult = 0;
	int n;
	int opt; // to choose functions 
	for (int i = 0; i < 8; ++i) { data[i] = 0; }
*/

#pragma endregion 
	namedWindow(windowName, 0);
	namedWindow(windowName2, 0);
	setMouseCallback("Original Image", onMouse, 0);

	while (1) {

#pragma region basic tracking function 

		//store image to matrix and flip the image
		capture.read(cameraFeed);
		/*
		//cameraFeed rotation 180 degree
		Point2f src_center(cameraFeed.cols / 2.0F, cameraFeed.rows / 2.0F);
		Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
		Mat dst;
		warpAffine(cameraFeed, dst, rot_mat, cameraFeed.size());
		cameraFeed = dst;

		// to flip over image 
		if (flipI)
		{
			cameraFeed.copyTo(inverse);
			flip(inverse, cameraFeed, 1);
		}
		*/
		cvtColor(cameraFeed, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);


		//MorphOps
		if (useMorphOps)
			morphOps(threshold);

		//pass in thresholded frame to our object tracking function
		//this function will return the x and y coordinates of the


#pragma endregion 

#pragma region general set up 

		Mat wholePanel;
		wholePanel = Mat::zeros(Size(SCREEN_WIDTH, SCREEN_HEIGHT), CV_8UC3);

		Mat ctrlPanel;
		ctrlPanel = Mat::zeros(Size(1000, 80), CV_8UC3);
		createPanel(ctrlPanel, 1000, 80);
		// this is a reflection on whole screen display 

		if (trackObjects)
		{
			// choose the different method for tracking 
			if (methodOpt == 2)
				trackFilteredObject(x, y, threshold, cameraFeed, wholePanel);
			else if (methodOpt == 1)
				objectFound = track(x, y, threshold);
		}

		/*
		if (result)
		{
			cout << "x value is " << x;
			cout << "y value is " << y << endl;
		}
		*/
		screensize(x_screen, y_screen, x, y);
		circle(wholePanel, Point(x_screen, y_screen), 5, Scalar(0, 0, 255), 2);
		circle(wholePanel, Point(680,364), 20, Scalar(0, 0, 255), 2);
#pragma endregion

#pragma region calibration and projection

		if (cali == true && objectFound == true)
		{
			if (start_time) {
				start = clock();
				start_time = false;
			}

			duration = (double)(clock() - start) / CLOCKS_PER_SEC;

			if (duration < 3)
				calibration(x, y, wholePanel, duration);
			if (duration > 3)
				cali = false;
		}

		else if (cali == false) {
			startControl = true;
		}

		else
			start = clock();

		// this is for control panel 
		if (objectFound == true)
		{
			x_screen1 = (x - x_o) / X_RANGE*(RESOLUTION_X / 2) + 1000 / 2 + dx;
			if (changeY)
				y_screen1 = y_screen;
			else
				y_screen1 = 100;
			circle(ctrlPanel, Point(x_screen1, y_screen1), 5, Scalar(0, 0, 255), 2);
		}

		//function on ctrlbox and corresponding indicate area divide: 
		controlbox(wholePanel, 5);

#pragma endregion 

#pragma region opt1_control: 

		if (userOpt == 1) {
			// this is for counting time for seletion 
			if (startControl == true)
			{
				input = control(x_screen1, y_screen1, wholePanel);
				ctrlSelect(input, wholePanel);
				//controlGesture1(x_screen1, y_screen1, wholePanel);
			}

			else if (startControl == false)
				input = 0;
		}

#pragma endregion 		
/*
#pragma region opt2_control

		if (userOpt == 2) {
			if (startControl == true) {
				inputC = controlC(x_screen1, y_screen1, wholePanel);

				if (objectFound == false)
					inputC = '5';  //stop

				input = inputC - '0';
				if (input != 5)
					rectangle(ctrlPanel, Point((input - 1) * 200, 0), Point(input * 200, 200), Scalar(156, 195, 165), CV_FILLED, 8);

				command[0] = inputC;
				int msglen = strlen(command);
				if (port->WriteData(command, msglen));  //write to arduino
				printf("Wrtiting Success\n");
				Sleep(15);
				n = port->ReadData(data, 8);
				if (n != -1) {
					data[n] = 0;
					cout << "arduino: " << data << endl;
				}
			}
		}
#pragma endregion 		
*/
#pragma region screen 

		circle(cameraFeed, Point(320, 240), 20, Scalar(0, 0, 255), 2);

		//Mat colorPanel;
		//colorPanel = Mat::zeros(Size(600, 1200), CV_8UC3);
		//createPanel(colorPanel, 600, 1200);

		//show frames
/*
		if (userOpt == 2) {
			if (port->IsConnected())
			{
				putText(wholePanel, "Arduino Connected", Point(1000, 60), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 2);
				ArduinoConnect = true;
			}
			else
			{
				putText(wholePanel, "Not Connected", Point(1000, 60), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 2);
				ArduinoConnect = false;
			}
		}
*/
		// show the images
		//rectangle(wholePanel, Point(1100, 100), Point(1960, 1100), Scalar(0, 255, 0), -1, 8); //right green rectangle on "wholepanel"
		namedWindow("wholePanel", WINDOW_NORMAL);
		imshow("wholePanel", wholePanel);
		moveWindow("wholePanel", 0, 0);

		//Bright green panel
		Mat new_image = Mat::zeros(wholePanel.size(), wholePanel.type());
		//cout << " Basic Linear Transforms " << endl;
		// cout << "-------------------------" << endl;
		// cout << "* Enter the alpha value [1.0-3.0]: "; cin >> alpha;
		// cout << "* Enter the beta value [0-100]: ";    cin >> beta;
		double alpha = 2.2;
		int beta = 50;
		for (int y = 1200; y < wholePanel.rows; y++) {
			for (int x = 0; x < wholePanel.cols; x++) {
				for (int c = 0; c < 3; c++) {
					new_image.at<Vec3b>(y, x)[c] =
						saturate_cast<uchar>(alpha*(wholePanel.at<Vec3b>(y, x)[c]) + beta);
				}
			}
		}
		wholePanel = new_image;



		imshow("ctrlPanel", ctrlPanel);
		hwnd1 = (HWND)cvGetWindowHandle("ctrlPanel");
		//SetWindowPos(hwnd1, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		//imshow("panel", colorPanel);
		hwnd2 = (HWND)cvGetWindowHandle("panel");
		//SetWindowPos(hwnd2, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		namedWindow(windowName, WINDOW_NORMAL);
		imshow(windowName, cameraFeed);
		resizeWindow(windowName, 320, 240);
		moveWindow(windowName, 100, 200);

		//Keep "Original Image" on top 
		hwnd3 = (HWND)cvGetWindowHandle("Original Image");
		//SetWindowPos(hwnd3, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		//HANDLE HWED1;
		//HWED1=cvGetWindowHandle(windowName); 
		ShowWindow(hwnd3, SW_MINIMIZE);
		ShowWindow(hwnd3, SW_RESTORE);
		if (camI) {
			imshow(windowName2, threshold);

		}

		if (!camI) {
			destroyWindow(windowName2);
			//		destroyWindow(windowName);
		}
		if (move)
		{
			//moveWindow("panel", 1300, 0);
			moveWindow("wholePanel", 120, 0);
			//moveWindow("ctrlPanel", 250, 0);
			moveWindow(windowName2, 0, 520);
			moveWindow(windowName, 0, 520);
			move = false;
		}
		//image will not appear without this waitKey() command
		waitKey(1);

#pragma endregion 
	}

	return 0;
}