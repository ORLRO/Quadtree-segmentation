#define cimg_use_png
#include "CImg.h"
#include<queue>
#include<iostream>

using namespace std;
using namespace cimg_library;
//CImgDisplay main_disp(512, 512, "Display");

class qt_segment
{
private:
	class Qadrant
	{
	public:
		Qadrant(int in_x0, int in_y0, int in_x1, int in_y1) :
			x0(in_x0), x1(in_x1),
			y0(in_y0), y1(in_y1)
		{
			tr = NULL;		tl = NULL;
			bl = NULL;		br = NULL;
		}
		~Qadrant()
		{
			if (tl != NULL)
			{
				delete tl;
				delete tr;
				delete bl;
				delete br;
			}
		}

	public:
		Qadrant* tr; // top right
		Qadrant* tl; // top left
		Qadrant* bl; // bottom left
		Qadrant* br; // bottom right
	
		int x0, y0; // top left pixel
		int x1, y1; // bottom right pixel
	};
public:
	qt_segment(CImg<unsigned char>& in_image, int in_x0, int in_y0, int in_x1, int in_y1, unsigned char in_threshold) :
		image(in_image),
		threshold(in_threshold)
	{
		root = new Qadrant(in_x0, in_y0, in_x1, in_y1);
		
		//split on creation
		split(root);
	}
	~qt_segment()
	{
		if (root != NULL)
		{
			delete root;
		}
	}

	CImg<unsigned char> get_marked()
	{
		if (root == NULL) return CImg<unsigned char>();
		// copy image 
		CImg<unsigned char> marked = image;
		const unsigned char color[] = { 255,128,64 };
		std::queue <Qadrant*> Q;
		Q.push(root);
		//while there is at least one discovered node
		while (!Q.empty())
		{
			Qadrant* current = Q.front();
			Q.pop(); // removing the element at front
			if (current->tl != NULL)
			{
				int ix0 = current->x0;
				int ix1 = current->x1;
				int iy0 = current->y0;
				int iy1 = current->y1;
				int rx = ix1 - ix0;
				int ry = iy1 - iy0;
				//draw split lines
				marked.draw_line(ix0 + rx / 2, iy0, ix0 + rx / 2, iy1, color); //vertical
				marked.draw_line(ix0, iy0 + ry / 2, ix1, iy0 + ry / 2, color); //horizontal

				//add children to Q
				Q.push(current->tl);
				Q.push(current->tr);
				Q.push(current->bl);
				Q.push(current->br);
			}
				
		}
		return marked;
	}
private:
	void split(Qadrant* q)
	{
		const unsigned char color[] = { 255,128,64 };
		//if not homogeneious -> split
		int ix0 = q->x0; int iy0 = q->y0;
		int ix1 = q->x1; int iy1 = q->y1;
		if (ix1 - ix0 > 0 && iy1 - iy0 > 0)
		{
			CImg<unsigned char> cropped = image.get_crop(ix0, iy0, ix1, iy1);
			if (cropped.max() - cropped.min() > threshold )
			{	//assume image size is power of 2
				cropped.assign();
				int rx = ix1 - ix0;
				int ry = iy1 - iy0;
				//top-left
				q->tl = new Qadrant(ix0, iy0, ix0 + rx / 2, iy0 + ry / 2);
				split(q->tl);
				//top-right
				q->tr = new Qadrant(ix0 + rx / 2 + 1, iy0, ix1, iy0 + ry / 2);
				split(q->tr);
				//bottom-left
				q->bl = new Qadrant(ix0, iy0 + ry / 2 + 1, ix0 + rx / 2, iy1);
				split(q->bl);
				//bottom-right
				q->br = new Qadrant(ix0 + rx / 2 + 1, iy0 + ry / 2 + 1, ix1, iy1);
				split(q->br);
			}
			else
				cropped.assign();
		}
	}
private:
	Qadrant* root;
	CImg<unsigned char>& image;
	unsigned char threshold;
};


int main() {
	CImg<unsigned char> image("peppers.png"), visu(500, 400, 1, 3, 0);
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };

	image.RGBtoYCbCr().channel(0);
	unsigned char threshold = 50;
	qt_segment* QT = new qt_segment(image, 0, 0, image.width()-1, image.height()-1, threshold);

	CImg<unsigned char>marked = QT->get_marked();
	CImgDisplay main_disp(marked, "Display");
	while (!main_disp.is_closed()) 
	{
		main_disp.wait();
		if (main_disp.is_keyARROWDOWN())
		{
			delete QT;
			threshold -= 5;
			QT = new qt_segment(image, 0, 0, image.width() - 1, image.height() - 1, threshold);
			main_disp.display(QT->get_marked());
		}
		if (main_disp.is_keyARROWUP())
		{
			delete QT;
			threshold += 5;
			QT = new qt_segment(image, 0, 0, image.width() - 1, image.height() - 1, threshold);
			main_disp.display(QT->get_marked());
		}
		
	}
	return 0;
}

