#define cimg_use_png
#include "CImg.h"
using namespace cimg_library;

class qt_segment
{
public:
	qt_segment(CImg<unsigned char> in_image, unsigned char in_threshold):
		image(in_image),
		threshold(in_threshold)
	{
		tr = NULL;		tl = NULL;
		bl = NULL;		br = NULL;
		//split on creation
		split();		
	}
	~qt_segment()
	{
		if (tl != NULL)
		{
			delete tl;
			delete tr;
			delete bl;
			delete br;
		}
			 
	}
private:
	void split()
	{
		//if not homogeneious -> split 
		if (image.max() - image.min() > threshold)
		{	//assume image size is power of 2
			//top-left
			tl = new qt_segment(image.get_crop(0, 0, image.width() / 2, image.height() / 2), threshold);
			//top-right
			tr = new qt_segment(image.get_crop(image.width() / 2 + 1, 0, image.width() - 1, image.height() / 2), threshold);
			//bottom-left
			bl = new qt_segment(image.get_crop(0, image.height() / 2 + 1, image.width() / 2, image.height() - 1), threshold);
			//bottom-right
			br = new qt_segment(image.get_crop(image.width() / 2 + 1, image.height() / 2 + 1,
				image.width() - 1, image.height() - 1), threshold);
		}
	}
private:
	CImg<unsigned char> image;
	unsigned char threshold;

	qt_segment* tr;
	qt_segment* tl;
	qt_segment* bl;
	qt_segment* br;
	
	int x0, y0; // top left pixel
	int x1, y1; // bottom right pixel
};


int main() {
	CImg<unsigned char> image("peppers.png"), visu(500, 400, 1, 3, 0);
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };

	image.RGBtoYCbCr().channel(0);
	qt_segment(image, 150);

	CImgDisplay main_disp(image, "Display");
	while (!main_disp.is_closed()) 
	{
		main_disp.wait();
	}
	return 0;
}

