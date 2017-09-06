#define cimg_use_png

#include "CImg.h"
#include "qt_segment.h"
#include <iostream>

//TODO: linear combination os multiple factors!(WOW)
// define * operator
// define + operator

using namespace std;
using namespace cimg_library;

unsigned char threshold = 90;//must be global (-_-) TODO: deal with it
					//OR hard coded in the function (un-controlable)
bool similar(const CImg<unsigned char>& A, const CImg<unsigned char>& B)
{
	return (std::max(A.max(), B.max()) - std::min(A.min(), B.min())) < threshold;
}
bool homogeneous(const CImg<unsigned char>& A)
{
	return A.max()- A.min() < threshold;
}

int main() {
	CImg<unsigned char> image("cropped-dfmm-tomato2.bmp");

	image.RGBtoYCbCr().channel(0); //convert to greyscale

	qt_segment* QT = new qt_segment(image,  homogeneous, similar );

	CImg<unsigned char>marked = QT->get_marked_split();
	CImg<unsigned char>marked2 = QT->get_marked_split_merged();
	CImgDisplay main_disp(marked, "Display");
	CImgDisplay main_disp2(marked2, "Display2");
	cout << (int)threshold << endl;
	while (!main_disp.is_closed() && !main_disp2.is_closed())
	{
		main_disp.wait();
		if (main_disp.is_keyARROWDOWN() && threshold > 5)
		{
			delete QT;
			threshold -= 5;
			QT = new qt_segment(image, homogeneous, similar);
			main_disp.display(QT->get_marked_split());
			main_disp2.display(QT->get_marked_split_merged());
			cout << (int)threshold << endl;
		}
		if (main_disp.is_keyARROWUP() && threshold < 255)
		{
			delete QT;
			threshold += 5;
			QT = new qt_segment(image, homogeneous, similar);
			main_disp.display(QT->get_marked_split());
			main_disp2.display(QT->get_marked_split_merged());
			cout << (int)threshold << endl;
		}		
	}
	return 0;
}