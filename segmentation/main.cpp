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
	unsigned char threshold = 150;

	qt_segment* QT = new qt_segment(image, threshold);

	CImg<unsigned char>marked = QT->get_marked_split_merged();
	//CImg<unsigned char>marked2 = QT->get_marked_split();
	CImgDisplay main_disp(marked, "Display");
	//CImgDisplay main_disp2(marked2, "Display2");
	cout << (int)threshold << endl;
	while (!main_disp.is_closed()) 
	{
		main_disp.wait();
		if (main_disp.is_keyARROWDOWN())
		{
			delete QT;
			threshold -= 5;
			QT = new qt_segment(image, threshold);
			main_disp.display(QT->get_marked_split_merged());
			//main_disp2.display(QT->get_marked_split());
			cout << (int)threshold << endl;
		}
		if (main_disp.is_keyARROWUP())
		{
			delete QT;
			threshold += 5;
			QT = new qt_segment(image, threshold);
			main_disp.display(QT->get_marked_split_merged());
			//main_disp2.display(QT->get_marked_split());
			cout << (int)threshold << endl;
		}		
	}
	return 0;
}