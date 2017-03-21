#pragma once
#include "CImg.h"
#include <queue>
#include <algorithm>
#include <string>

using namespace cimg_library;
using namespace std;

class qt_segment
{
private:
	class Qadrant
	{
	public:
		Qadrant(Qadrant* parent, int in_x0, int in_y0, int in_length) :
			x0(in_x0), y0(in_y0), length(in_length)
		{
			Qs[0][0] = NULL;		Qs[0][1] = NULL;
			Qs[1][0] = NULL;		Qs[1][1] = NULL;
			// assume all are NOT similar on creation
			sLabel[0][0] = 0;		sLabel[0][1] = 1;
			sLabel[1][0] = 2;		sLabel[1][1] = 3;
		}
		~Qadrant()
		{
			if (Qs[0][0] != NULL)
			{
				delete Qs[0][0];
				delete Qs[1][0];
				delete Qs[0][1];
				delete Qs[1][1];
			}
		}

	public:
		Qadrant* parent; // parent
		Qadrant* Qs[2][2]; // quadrants in 2x2 array
		unsigned char sLabel[2][2];//label similar quadrant with same number

		int x0, y0; // top left pixel
		int length; // quadrant side length
	};
public:
	qt_segment(CImg<unsigned char>& in_image, unsigned char in_threshold) :
		image(in_image),
		threshold(in_threshold)
	{//assume image size is power of 2
		root = new Qadrant(NULL, 0, 0, min(in_image.width(), in_image.height()));
		//split on creation
		//split_merge(root);
		split(root);
		merge(root);
	}
	~qt_segment()
	{
		if (root != NULL)
		{
			delete root;
		}
	}

	CImg<unsigned char> get_marked_split()
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
			if (current->Qs[0][0] != NULL)
			{
				int ix0 = current->x0;
				int iy0 = current->y0;
				int iL = current->length;
				//draw split lines
				marked.draw_line(ix0 + iL / 2, iy0, ix0 + iL / 2, iy0 + iL, color); //vertical
				marked.draw_line(ix0, iy0 + iL / 2, ix0 + iL, iy0 + iL / 2, color); //horizontal
				//for debugging 
				//marked.draw_text(ix0 + iL / 4, iy0 + iL / 4, to_string(current->sLabel[0][0]).c_str(), color, 0, 1, 12);
				//marked.draw_text(ix0 + 3 * iL / 4, iy0 + iL / 4, to_string(current->sLabel[0][1]).c_str(), color, 0, 1, 12);
				//marked.draw_text(ix0 + iL / 4, iy0 + 3 * iL / 4, to_string(current->sLabel[1][0]).c_str(), color, 0, 1, 12);
				//marked.draw_text(ix0 + 3 * iL / 4, iy0 + 3 * iL / 4, to_string(current->sLabel[1][1]).c_str(), color, 0, 1, 12);
				//add children to Q
				Q.push(current->Qs[0][0]);
				Q.push(current->Qs[0][1]);
				Q.push(current->Qs[1][0]);
				Q.push(current->Qs[1][1]);
			}

		}
		return marked;
	}
	CImg<unsigned char> get_marked_split_merged()
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
			if (current->Qs[0][0] != NULL)
			{
				int ix0 = current->x0;
				int iy0 = current->y0;
				int iL = current->length;
				//draw split lines
				if(current->sLabel[0][0] != current->sLabel[0][1])
					marked.draw_line(ix0 + iL / 2, iy0, ix0 + iL / 2, iy0 + iL / 2, color); 
				if (current->sLabel[1][0] != current->sLabel[1][1])
					marked.draw_line(ix0 + iL / 2, iy0 + iL / 2, ix0 + iL / 2, iy0 + iL, color);
				
				if (current->sLabel[0][0] != current->sLabel[1][0])
					marked.draw_line(ix0, iy0 + iL / 2, ix0 + iL / 2, iy0 + iL / 2, color); 
				if (current->sLabel[0][1] != current->sLabel[1][1])
					marked.draw_line(ix0 + iL /2, iy0 + iL / 2, ix0 + iL, iy0 + iL / 2, color);

				Q.push(current->Qs[0][0]);
				Q.push(current->Qs[0][1]);
				Q.push(current->Qs[1][0]);
				Q.push(current->Qs[1][1]);
			}
			else
			{

			}

		}
		return marked;
	}
private:
	void merge(Qadrant* q)
	{
		if (q == NULL) return;

		if (q->Qs[0][0] != NULL) //leaf quadrant
		{
			int ix0 = q->x0;
			int iy0 = q->y0;
			int iL = q->length;

			CImg<unsigned char> TL = image.get_crop(ix0, iy0, ix0 + iL / 2 - 1, iy0 + iL / 2 - 1);
			CImg<unsigned char> TR = image.get_crop(ix0 + iL / 2, iy0, ix0 + iL - 1, iy0 + iL / 2 - 1);
			
			CImg<unsigned char> T = TL.get_append(TR, 'x');
			if (T.max() - T.min() < threshold)
			{
				q->sLabel[0][1] = 0;
			}
			T.assign();
			CImg<unsigned char> BL = image.get_crop(ix0, iy0 + iL / 2, ix0 + iL / 2 - 1, iy0 + iL - 1);
			CImg<unsigned char> L = TL.get_append(BL, 'y');
			if (L.max() - L.min() < threshold)
			{
				q->sLabel[1][0] = 0;
			}
			L.assign();
			TL.assign();
			CImg<unsigned char> BR = image.get_crop(ix0 + iL / 2, iy0 + iL / 2, ix0 + iL - 1, iy0 + iL - 1);
			CImg<unsigned char> R = TR.get_append(BR, 'y');
			if (R.max() - R.min() < threshold)
			{
				q->sLabel[1][1] = q->sLabel[0][1];
			}
			R.assign();
			TR.assign();

			CImg<unsigned char> B = BL.get_append(BR, 'x');
			if (B.max() - B.min() < threshold)
			{
				if (q->sLabel[1][0] != 0)
					q->sLabel[1][0] = q->sLabel[1][1];
				else
					q->sLabel[1][1] = 0;
			}
			B.assign();
			BR.assign();
			BL.assign();

			merge(q->Qs[0][0]);
			merge(q->Qs[0][1]);
			merge(q->Qs[1][0]);
			merge(q->Qs[1][1]);

		}
		else
		{
			
		}
	}
	void merge_level(Qadrant* q)
	{
		int ix0 = q->x0;
		int iy0 = q->y0;
		int iL = q->length;

		CImg<unsigned char> TL = image.get_crop(ix0, iy0, ix0 + iL / 2 - 1, iy0 + iL / 2 - 1);
		CImg<unsigned char> TR = image.get_crop(ix0 + iL / 2, iy0, ix0 + iL - 1, iy0 + iL / 2 - 1);

		CImg<unsigned char> T = TL.get_append(TR, 'x');
		if (T.max() - T.min() < threshold)
		{
			q->sLabel[0][1] = 0;
		}
		T.assign();
		CImg<unsigned char> BL = image.get_crop(ix0, iy0 + iL / 2, ix0 + iL / 2 - 1, iy0 + iL - 1);
		CImg<unsigned char> L = TL.get_append(BL, 'y');
		if (L.max() - L.min() < threshold)
		{
			q->sLabel[1][0] = 0;
		}
		L.assign();
		TL.assign();
		CImg<unsigned char> BR = image.get_crop(ix0 + iL / 2, iy0 + iL / 2, ix0 + iL - 1, iy0 + iL - 1);
		CImg<unsigned char> R = TR.get_append(BR, 'y');
		if (R.max() - R.min() < threshold)
		{
			q->sLabel[1][1] = q->sLabel[0][1];
		}
		R.assign();
		TR.assign();

		CImg<unsigned char> B = BL.get_append(BR, 'x');
		if (B.max() - B.min() < threshold)
		{
			if (q->sLabel[1][0] != 0)
				q->sLabel[1][0] = q->sLabel[1][1];
			else
				q->sLabel[1][1] = 0;
		}
		B.assign();
		BR.assign();
		BL.assign();
	}
	void split_merge(Qadrant* q)
	{
		const unsigned char color[] = { 255,128,64 };
		//if not homogeneious -> split
		int ix0 = q->x0;
		int iy0 = q->y0;
		int iL = q->length;
		//if (iL > 0)
		{
			CImg<unsigned char> cropped = image.get_crop(ix0, iy0, ix0 + iL, iy0 + iL);
			if (cropped.max() - cropped.min() > threshold)
			{
				//cropped.assign();
				//top-left
				q->Qs[0][0] = new Qadrant(q, ix0, iy0, iL / 2);
				split(q->Qs[0][0]);
				//top-right
				q->Qs[0][1] = new Qadrant(q, ix0 + iL / 2, iy0, iL / 2);
				split(q->Qs[0][1]);
				//bottom-left
				q->Qs[1][0] = new Qadrant(q, ix0, iy0 + iL / 2, iL / 2);
				split(q->Qs[1][0]);
				//bottom-right
				q->Qs[1][1] = new Qadrant(q, ix0 + iL / 2, iy0 + iL / 2, iL / 2);
				split(q->Qs[1][1]);
				merge_level(q); //merges only the current level in the garaph  
			}
			cropped.assign();
		}
	}
	void split(Qadrant* q)
	{
		const unsigned char color[] = { 255,128,64 };
		//if not homogeneious -> split
		int ix0 = q->x0; 
		int iy0 = q->y0;
		int iL = q->length;
		//if (iL > 0)
		{
			CImg<unsigned char> cropped = image.get_crop(ix0, iy0, ix0 + iL, iy0 + iL);
			if (cropped.max() - cropped.min() > threshold)
			{	
				//cropped.assign();
				//top-left
				q->Qs[0][0] = new Qadrant(q, ix0, iy0, iL / 2);
				split(q->Qs[0][0]);
				//top-right
				q->Qs[0][1] = new Qadrant(q, ix0 + iL / 2 , iy0, iL / 2);
				split(q->Qs[0][1]);
				//bottom-left
				q->Qs[1][0] = new Qadrant(q, ix0, iy0 + iL / 2 , iL / 2);
				split(q->Qs[1][0]);
				//bottom-right
				q->Qs[1][1] = new Qadrant(q, ix0 + iL / 2 , iy0 + iL / 2 , iL / 2);
				split(q->Qs[1][1]); 
			}
			//else
				cropped.assign();
		}
	}
private:
	Qadrant* root;
	CImg<unsigned char>& image;
	unsigned char threshold;
};


