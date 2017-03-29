#pragma once
#include "CImg.h"
#include <queue>
#include <algorithm>
#include <string>
#include <iostream>
#include <stack> 
using namespace cimg_library;
using namespace std;

class qt_segment
{
private:
	class Qadrant
	{
	public:
		Qadrant(CImg<unsigned char> in_img, const Qadrant* in_parent, int in_x0, int in_y0) :
			img(in_img), 
			parent(in_parent),
			x0(in_x0), y0(in_y0)
		{
			Qs[0][0] = NULL;		Qs[0][1] = NULL;
			Qs[1][0] = NULL;		Qs[1][1] = NULL;
			// assume all are NOT similar on creation
			sLabel[0][0] = 0;		sLabel[0][1] = 1;
			sLabel[1][0] = 2;		sLabel[1][1] = 3;
			homogeneity = img.max() - img.min();
		}
		~Qadrant()
		{
			if (Qs[0][0] != NULL)
			{
				delete Qs[0][0];delete Qs[0][1];
				delete Qs[1][0];delete Qs[1][1];
			}
		}
	private:
		const Qadrant* get_first_right_parent(stack<bool>& vertical_moves) const
		{
			if (parent == NULL) return NULL;
			if (parent->Qs[0][0] == this)
			{
				vertical_moves.push(0);
				return parent->get_first_right_parent(vertical_moves);
			}
			if (parent->Qs[1][0] == this)
			{
				vertical_moves.push(1);
				return parent->get_first_right_parent(vertical_moves);
			}
			return this;
		}
		const Qadrant* get_immediate_left() const
		{
			if (this == parent->Qs[0][0] || this == parent->Qs[1][0] || parent == NULL)
				return NULL; //current node already on the left or root quadrant
			if (this == parent->Qs[0][1])
				return parent->Qs[0][0];
			if (this == parent->Qs[1][1])
				return parent->Qs[1][0];
		}
		const Qadrant* get_right_child(stack<bool>& vertical_moves) const
		{
			if (!vertical_moves.empty())
			{
				if (Qs[0][0] == NULL) // no children
					return this;
				bool v = vertical_moves.top();
				vertical_moves.pop();
				return Qs[v][1]->get_right_child(vertical_moves);
			} 
			return this;
		}
	public:
		const Qadrant* get_left() const
		{
			std::stack<bool> vertical_moves;
			//get first non left parent
			const Qadrant* first_non_left_parent = get_first_right_parent(vertical_moves);
			if (first_non_left_parent == NULL)
				return NULL;
			//get the left neighbor
			const Qadrant* parent_left = first_non_left_parent->get_immediate_left();
			//get right quadrant by back tracing the same vertical movements
			return parent_left->get_right_child(vertical_moves);
		}
	public:
		const Qadrant* parent; // parent
		Qadrant* Qs[2][2]; //children quadrants in 2x2 array
		unsigned char sLabel[2][2];//label similar quadrant with same number
		unsigned char homogeneity; //max - min (as example) TODO: make it lambda functionc

		int x0, y0; // top left pixel coordinates in the origenal image
		CImg<unsigned char> img;// portion of the image in this quadrant
	};
public:
	qt_segment(CImg<unsigned char>& in_image, unsigned char in_threshold) :
		image(in_image),
		threshold(in_threshold)
	{
		//assume image size is power of 2
		//TODO: what if not
		root = new Qadrant(image, NULL, 0, 0);
		//split on creation
		cimg::tic();
		//split_merge(root);
		
		split(root); merge(root); 
		cout << cimg::toc() << endl;
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
				int iL = current->img.width();
				//draw split lines
				marked.draw_line(ix0 + iL / 2, iy0, ix0 + iL / 2, iy0 + iL, color); //vertical
				marked.draw_line(ix0, iy0 + iL / 2, ix0 + iL, iy0 + iL / 2, color); //horizontal
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
		const unsigned char blk[] = { 0,0,0 };
		std::queue <Qadrant*> Q;
		Q.push(root);
		//while there is at least one discovered node
		while (!Q.empty())
		{
			Qadrant* current = Q.front();
			Q.pop(); // removing the element at front

			int ix0 = current->x0;
			int iy0 = current->y0;
			int iL = current->img.width();

			if (current->Qs[0][0] != NULL)//not leaf
			{
				
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
			else //leaf
			{
				if (current->parent->Qs[0][0] == current)
				{
					if (current->get_left() != NULL)
						if(similar(current->img, current->get_left()->img))
						{
							marked.draw_line(ix0 , iy0, ix0, iy0 + iL / 2, blk);
						}
				}
				else if (current->parent->Qs[1][0] == current)
				{
					if (current->get_left() != NULL)
						if (similar(current->img, current->get_left()->img))
						{
							marked.draw_line(ix0, iy0 + iL / 2, ix0, iy0 + iL , blk);
						}
				}

					
			}

		}
		return marked;
	}
private:
	bool similar(const CImg<unsigned char> A, const CImg<unsigned char> B)
	{
		CImg<unsigned char> C = A.get_append(B);
		return (C.max() - C.min()) < threshold;
	}
	void merge(Qadrant* q)
	{
		if (q == NULL) return;

		if (q->Qs[0][0] != NULL) //not leaf quadrant
		{
			merge_quadrant(q);

			merge(q->Qs[0][0]);
			merge(q->Qs[0][1]);
			merge(q->Qs[1][0]);
			merge(q->Qs[1][1]);
		}
	}
	void merge_quadrant(Qadrant* q)
	{
		if (similar(q->Qs[0][0]->img, q->Qs[0][1]->img))
		{
			q->sLabel[0][1] = 0;
		}
		if (similar(q->Qs[0][0]->img, q->Qs[1][0]->img))
		{
			q->sLabel[1][0] = 0;
		}
		if (similar(q->Qs[0][1]->img, q->Qs[1][1]->img)) 
		{
			q->sLabel[1][1] = q->sLabel[0][1];
		}
		if (similar(q->Qs[1][0]->img, q->Qs[1][1]->img))
		{
			if (q->sLabel[1][0] != 0)
				q->sLabel[1][0] = q->sLabel[1][1];
			else
				q->sLabel[1][1] = 0;
		}
	}
	void split_merge(Qadrant* q)
	{
		//if not homogeneious -> split
		if (q->homogeneity > threshold)
		{
			int ix0 = q->x0;
			int iy0 = q->y0;
			int iL = q->img.width();

			//top-left
			q->Qs[0][0] = new Qadrant(q->img.get_crop(0, 0, iL / 2 - 1, iL / 2 - 1), q, ix0, iy0);
			//top-right
			q->Qs[0][1] = new Qadrant(q->img.get_crop(iL / 2, 0, iL - 1, iL / 2 - 1), q, ix0 + iL / 2, iy0);
			//bottom-left
			q->Qs[1][0] = new Qadrant(q->img.get_crop(0, iL / 2, iL / 2 - 1, iL - 1), q, ix0, iy0 + iL / 2);
			//bottom-right
			q->Qs[1][1] = new Qadrant(q->img.get_crop(iL / 2, iL / 2, iL - 1, iL - 1), q, ix0 + iL / 2, iy0 + iL / 2);
			
			merge_quadrant(q); //merges only the current quadrant beeing split
			
			split_merge(q->Qs[0][0]);
			split_merge(q->Qs[0][1]);
			split_merge(q->Qs[1][0]);
			split_merge(q->Qs[1][1]);
			
		}
	}
	void split(Qadrant* q)
	{
		//if not homogeneious -> split
		if (q->homogeneity > threshold)
		{
			int ix0 = q->x0;
			int iy0 = q->y0;
			int iL = q->img.width();
			//cropped.assign();
			//top-left
			q->Qs[0][0] = new Qadrant(q->img.get_crop(0, 0, iL / 2 - 1, iL / 2 - 1), q, ix0, iy0);
			split(q->Qs[0][0]);
			//top-right
			q->Qs[0][1] = new Qadrant(q->img.get_crop(iL / 2, 0, iL - 1, iL / 2 - 1), q, ix0 + iL / 2, iy0);
			split(q->Qs[0][1]);
			//bottom-left
			q->Qs[1][0] = new Qadrant(q->img.get_crop(0, iL / 2, iL / 2 - 1, iL - 1), q, ix0, iy0 + iL / 2);
			split(q->Qs[1][0]);
			//bottom-right
			q->Qs[1][1] = new Qadrant(q->img.get_crop(iL / 2, iL / 2, iL - 1, iL - 1), q, ix0 + iL / 2, iy0 + iL / 2);
			split(q->Qs[1][1]); 
		}
		
	}
private:
	Qadrant* root;
	CImg<unsigned char>& image;
	unsigned char threshold;
};


