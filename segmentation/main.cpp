#define cimg_use_png
#include "CImg.h"
#include "qt_segment.h"
#include<iostream>

using namespace std;
using namespace cimg_library;
//CImgDisplay main_disp(512, 512, "Display");

int main() {
	CImg<unsigned char> image("peppers.png");

	image.RGBtoYCbCr().channel(0); //convert to greyscale
	unsigned char threshold = 50;

	qt_segment* QT = new qt_segment(image, threshold);

	CImg<unsigned char>marked = QT->get_marked();
	CImgDisplay main_disp(marked, "Display");
	while (!main_disp.is_closed()) 
	{
		main_disp.wait();
		if (main_disp.is_keyARROWDOWN())
		{
			delete QT;
			threshold -= 5;
			QT = new qt_segment(image, threshold);
			main_disp.display(QT->get_marked());
		}
		if (main_disp.is_keyARROWUP())
		{
			delete QT;
			threshold += 5;
			QT = new qt_segment(image, threshold);
			main_disp.display(QT->get_marked());
		}		
	}
	return 0;
}