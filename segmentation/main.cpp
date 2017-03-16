#define cimg_use_png
#include "CImg.h"
using namespace cimg_library;

class qt_segment
{
public:
	qt_segment(CImg<unsigned char>& in_image, int in_x0, int in_y0, int in_x1, int in_y1, unsigned char in_threshold) :
		image(in_image),
		threshold(in_threshold),
		x0(in_x0), x1(in_x1),
		y0(in_y0), y1(in_y1)
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
		if (x1 - x0 > 0 && y1 - y0 > 0)
		{
			CImg<unsigned char> cropped = image.get_crop(x0, y0, x1, y1);
			if (cropped.max() - cropped.min() > threshold )
			{	//assume image size is power of 2
				cropped.assign();
				int rx = x1 - x0;
				int ry = y1 - y0;
				//top-left
				tl = new qt_segment(image, x0, y0, x0 + rx / 2, y0 + ry / 2, threshold);
				//top-right
				tr = new qt_segment(image, x0 + rx / 2 + 1, y0, x1, y0 + ry / 2, threshold);
				//bottom-left
				bl = new qt_segment(image, x0, y0 + ry / 2 + 1, x0 + rx / 2, y1, threshold);
				//bottom-right
				br = new qt_segment(image, x0 + rx / 2 + 1, y0 + ry / 2 + 1, x1, y1, threshold);
			}
			else
				cropped.assign();
		}
	}
private:
	CImg<unsigned char>& image;
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
	qt_segment* QT = new qt_segment(image, 0, 0, image.width()-1, image.height()-1, 10);

	CImgDisplay main_disp(image, "Display");
	while (!main_disp.is_closed()) 
	{
		main_disp.wait();
	}
	return 0;
}

