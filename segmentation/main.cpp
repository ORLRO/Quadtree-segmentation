#define cimg_use_png
//TODO: quadrdants should not have separate copies of the image

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "qt_segment.h"
#include <iostream>

//TODO: linear combination of multiple factors!
// define * operator
// define + operator

using namespace std;
using namespace cv;

unsigned char threshold2 = 90;//must be global (-_-) 
							//TODO: deal with it
							//OR hard coded in the function (un-controllable)
bool similar(const Mat_<unsigned char>& A, const Mat_<unsigned char>& B)
{
	double Amx, Amn, Bmx, Bmn;
	minMaxLoc(A, &Amn, &Amx, NULL, NULL);
	minMaxLoc(B, &Bmn, &Bmx, NULL, NULL);
	return (std::max(Amx, Bmx) - std::min(Amn, Bmn)) < threshold2;
}
bool homogeneous(const Mat_<unsigned char>& A)
{
	double mx, mn;
	minMaxLoc(A, &mn, &mx, NULL, NULL);
	return mx - mn < threshold2;
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
	Mat_<unsigned char>marked2 = QT->get_labeled();
	imshow("Main window", marked);
	imshow("Segment window", marked2);
	cout << (int)threshold2 << endl;
	
	char c = 0; //capture key input
	do
	{
		c = cvWaitKey(-1);
		if (c == 's' && threshold2 > 5) 
		{
			delete QT;
			threshold2 -= 5;
			QT = new qt_segment(image, homogeneous, similar);
			imshow("Main window", QT->get_marked_split());
			imshow("Segment window", QT->get_labeled());

			cout << (int)threshold2 << endl;
		}
		if (c == 'w' && threshold2 < 255) 
		{
			delete QT;
			threshold2 += 5;
			QT = new qt_segment(image, homogeneous, similar);
			imshow("Main window", QT->get_marked_split());
			imshow("Segment window", QT->get_labeled());
			cout << (int)threshold2 << endl;
		}		
	} while (c != 27);
	return 0;
}