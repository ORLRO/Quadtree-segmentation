#define cimg_use_png

//#include "CImg.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "qt_segment.h"
#include <iostream>

//TODO: linear combination os multiple factors!(WOW)
// define * operator
// define + operator

using namespace std;
using namespace cv;

unsigned char threshold = 90;//must be global (-_-) TODO: deal with it
					//OR hard coded in the function (un-controllable)
bool similar(const Mat_<unsigned char>& A, const Mat_<unsigned char>& B)
{
	double* Amx, *Amn, *Bmx, *Bmn;
	minMaxLoc(A, Amx, Amn, NULL, NULL);
	minMaxLoc(B, Bmx, Bmn, NULL, NULL);
	return (std::max(*Amx, *Bmx) - std::min(*Amn, *Bmn)) < threshold;
}
bool homogeneous(const Mat_<unsigned char>& A)
{
	double* mx, *mn;
	minMaxLoc(A, mx, mn, NULL, NULL);
	return mx - mn < threshold;
}

int main(int argc, char** argv)
{
	String imageName("cropped-dfmm-tomato2.bmp"); // by default
	if (argc > 1)
	{
		imageName = argv[1];
	}
	Mat_<unsigned char> image;
	image = imread(imageName, IMREAD_GRAYSCALE); // Read the file

	if (image.empty())                      // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	//define 2 display windows
	namedWindow("Main window", WINDOW_AUTOSIZE); // Create a window for display.
	namedWindow("Segment window", WINDOW_AUTOSIZE);  // Show our image inside it.

	// initial segmentation
	qt_segment* QT = new qt_segment(image,  homogeneous, similar );
	Mat_<unsigned char>marked = QT->get_marked_split();
	Mat_<unsigned char>marked2 = QT->get_marked_split_merged();
	imshow("Main window", marked);
	imshow("Segment window", marked2);
	cout << (int)threshold << endl;
	
	char c = 0; //capture key input
	do
	{
		c = cvWaitKey(100);
		if (c == 31 && threshold > 5) //30: up arrow
		{
			delete QT;
			threshold -= 5;
			QT = new qt_segment(image, homogeneous, similar);
			imshow("Main window", QT->get_marked_split());
			imshow("Segment window", QT->get_marked_split_merged());

			cout << (int)threshold << endl;
		}
		if (c == 30 && threshold < 255) //31: down arrow
		{
			delete QT;
			threshold += 5;
			QT = new qt_segment(image, homogeneous, similar);
			imshow("Main window", QT->get_marked_split());
			imshow("Segment window", QT->get_marked_split_merged());
			cout << (int)threshold << endl;
		}		
	} while (c != 27);
	return 0;
}