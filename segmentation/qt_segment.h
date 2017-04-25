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
			// assume all inner quadrants are NOT similar on creation
			siLabel[0][0] = 0;		siLabel[0][1] = 1;
			siLabel[1][0] = 2;		siLabel[1][1] = 3;

			//assume each quadrant is similar to its outer neighbors
			svoLabel[0][0] = true;	svoLabel[0][1] = true;
			svoLabel[1][0] = true;	svoLabel[1][1] = true;

			shoLabel[0][0] = true;	shoLabel[0][1] = true;
			shoLabel[1][0] = true;	shoLabel[1][1] = true;
		}
		~Qadrant()
		{
			if (!isLeaf())
			{
				delete Qs[0][0];delete Qs[0][1];
				delete Qs[1][0];delete Qs[1][1];
			}
		}
	private:
		const Qadrant* get_first_right_parent(stack<bool>& vertical_moves) const
		{
			if (isRoot()) return NULL;
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
		const Qadrant* get_first_left_parent(stack<bool>& vertical_moves) const
		{
			if (isRoot()) return NULL;
			if (parent->Qs[0][1] == this)
			{
				vertical_moves.push(0);
				return parent->get_first_left_parent(vertical_moves);
			}
			if (parent->Qs[1][1] == this)
			{
				vertical_moves.push(1);
				return parent->get_first_left_parent(vertical_moves);
			}
			return this;
		}
		const Qadrant* get_first_top_parent(stack<bool>& horizontal_moves) const
		{
			if (isRoot()) return NULL;
			if (parent->Qs[1][0] == this)
			{
				horizontal_moves.push(0);
				return parent->get_first_top_parent(horizontal_moves);
			}
			if (parent->Qs[1][1] == this)
			{
				horizontal_moves.push(1);
				return parent->get_first_top_parent(horizontal_moves);
			}
			return this;
		}
		const Qadrant* get_first_bottom_parent(stack<bool>& horizontal_moves) const
		{
			if (isRoot()) return NULL;
			if (parent->Qs[0][0] == this)
			{
				horizontal_moves.push(0);
				return parent->get_first_bottom_parent(horizontal_moves);
			}
			if (parent->Qs[0][1] == this)
			{
				horizontal_moves.push(1);
				return parent->get_first_bottom_parent(horizontal_moves);
			}
			return this;
		}

		const Qadrant* get_immediate_left() const
		{
			if (this == parent->Qs[0][0] || this == parent->Qs[1][0] || isRoot())
				return NULL; //current node already on the left or root quadrant
			if (this == parent->Qs[0][1])
				return parent->Qs[0][0];
			if (this == parent->Qs[1][1])
				return parent->Qs[1][0];
		}
		const Qadrant* get_immediate_right() const
		{
			if (this == parent->Qs[0][1] || this == parent->Qs[1][1] || isRoot())
				return NULL; //current node already on the right or root quadrant
			if (this == parent->Qs[0][0])
				return parent->Qs[0][1];
			if (this == parent->Qs[1][0])
				return parent->Qs[1][1];
		}
		const Qadrant* get_immediate_top() const
		{
			if (this == parent->Qs[0][0] || this == parent->Qs[0][1] || isRoot())
				return NULL; //current node already on the top or root quadrant
			if (this == parent->Qs[1][0])
				return parent->Qs[0][0];
			if (this == parent->Qs[1][1])
				return parent->Qs[0][1];
		}
		const Qadrant* get_immediate_bottom() const
		{
			if (this == parent->Qs[1][0] || this == parent->Qs[1][1] || isRoot())
				return NULL; //current node already on the bottom or root quadrant
			if (this == parent->Qs[0][0])
				return parent->Qs[1][0];
			if (this == parent->Qs[0][1])
				return parent->Qs[1][1];
		}

		const Qadrant* get_right_child(stack<bool>& vertical_moves) const
		{
			if (!vertical_moves.empty())
			{
				if (isLeaf())
					return this;
				bool v = vertical_moves.top();
				vertical_moves.pop();
				return Qs[v][1]->get_right_child(vertical_moves);
			} 
			return this;
		}
		const Qadrant* get_left_child(stack<bool>& vertical_moves) const
		{
			if (!vertical_moves.empty())
			{
				if (isLeaf())
					return this;
				bool v = vertical_moves.top();
				vertical_moves.pop();
				return Qs[v][0]->get_left_child(vertical_moves);
			}
			return this;
		}
		const Qadrant* get_top_child(stack<bool>& horizontal_moves) const
		{
			if (!horizontal_moves.empty())
			{
				if (isLeaf())
					return this;
				bool h = horizontal_moves.top();
				horizontal_moves.pop();
				return Qs[0][h]->get_top_child(horizontal_moves);
			}
			return this;
		}
		const Qadrant* get_bottom_child(stack<bool>& horizontal_moves) const
		{
			if (!horizontal_moves.empty())
			{
				if (isLeaf())
					return this;
				bool h = horizontal_moves.top();
				horizontal_moves.pop();
				return Qs[1][h]->get_bottom_child(horizontal_moves);
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
		const Qadrant* get_right() const
		{
			std::stack<bool> vertical_moves;
			//get first non left parent
			const Qadrant* first_non_right_parent = get_first_left_parent(vertical_moves);
			if (first_non_right_parent == NULL)
				return NULL;
			//get the left neighbor
			const Qadrant* parent_right = first_non_right_parent->get_immediate_right();
			//get right quadrant by back tracing the same vertical movements
			return parent_right->get_left_child(vertical_moves);
		}
		const Qadrant* get_top() const
		{
			std::stack<bool> horizontal_moves;
			//get first non left parent
			const Qadrant* first_non_top_parent = get_first_bottom_parent(horizontal_moves);
			if (first_non_top_parent == NULL)
				return NULL;
			//get the left neighbor
			const Qadrant* parent_top = first_non_top_parent->get_immediate_top();
			//get right quadrant by back tracing the same vertical movements
			return parent_top->get_bottom_child(horizontal_moves);
		}
		const Qadrant* get_bottom() const
		{
			std::stack<bool> horizontal_moves;
			//get first non left parent
			const Qadrant* first_non_bottom_parent = get_first_top_parent(horizontal_moves);
			if (first_non_bottom_parent == NULL)
				return NULL;
			//get the left neighbor
			const Qadrant* parent_bottom = first_non_bottom_parent->get_immediate_bottom();
			//get right quadrant by back tracing the same vertical movements
			return parent_bottom->get_top_child(horizontal_moves);
		}
		bool isRoot() const
		{
			return parent == NULL;
		}
		bool isLeaf() const
		{
			return Qs[0][0] == NULL;
		}
	public:
		const Qadrant* parent; // parent
		Qadrant* Qs[2][2]; //children quadrants in 2x2 array
		unsigned char siLabel[2][2];//label similar quadrant with same number (i: inner)
		bool svoLabel[2][2]; // is quadrant [s]imilar to its [v]ertical [o]uter neighbor 
		bool shoLabel[2][2]; // is quadrant [s]imilar to its [h]orizontal [o]uter neighbor 

		int x0, y0; // top left pixel coordinates in the origenal image
		CImg<unsigned char> img;// portion of the image in this quadrant
	};
public:
	qt_segment(CImg<unsigned char>& in_image, 
		bool(*in_homogeneous)(const CImg<unsigned char>&),
		bool(*in_similar)(const CImg<unsigned char>&, const CImg<unsigned char>& )) :
		image(in_image),
		homogeneous(in_homogeneous),
		similar(in_similar)
	{
		//assume image size is power of 2
		//TODO: what if not
		root = new Qadrant(image, NULL, 0, 0);
		//split on creation
		cimg::tic();
		//split_merge(root);
		
		split(root); merge(root); 
		cimg::toc();
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
		const unsigned char color[] = { 128,128,128 };
		const unsigned char blk[] = { 0,0,0 };
		std::queue <Qadrant*> Q;
		Q.push(root);
		//while there is at least one discovered node
		while (!Q.empty())
		{
			Qadrant* current = Q.front();
			int ix0 = current->x0;
			int iy0 = current->y0;
			int iL = current->img.width();

			
			Q.pop(); // removing the element at front
			if (!current->isLeaf())
			{
				//draw split lines
				marked.draw_line(ix0 + iL / 2, iy0, ix0 + iL / 2, iy0 + iL, color); //vertical
				marked.draw_line(ix0, iy0 + iL / 2, ix0 + iL, iy0 + iL / 2, color); //horizontal
				
				//add children to Q
				Q.push(current->Qs[0][0]);
				Q.push(current->Qs[0][1]);
				Q.push(current->Qs[1][0]);
				Q.push(current->Qs[1][1]);
			}
			else
			{
				//draw [i]nner split lines
				if (current->siLabel[0][0] != current->siLabel[0][1])
					marked.draw_line(ix0 + iL / 2, iy0, ix0 + iL / 2, iy0 + iL / 2, color);
				if (current->siLabel[1][0] != current->siLabel[1][1])
					marked.draw_line(ix0 + iL / 2, iy0 + iL / 2, ix0 + iL / 2, iy0 + iL, color);

				if (current->siLabel[0][0] != current->siLabel[1][0])
					marked.draw_line(ix0, iy0 + iL / 2, ix0 + iL / 2, iy0 + iL / 2, color);
				if (current->siLabel[0][1] != current->siLabel[1][1])
					marked.draw_line(ix0 + iL / 2, iy0 + iL / 2, ix0 + iL, iy0 + iL / 2, color);

				//draw [o]uter [v]ertical similarity split lines (horizontal lines)
				if (!current->svoLabel[0][0])
					marked.draw_line(ix0, iy0, ix0 + iL, iy0, color);
				if (!current->svoLabel[0][1])
					marked.draw_line(ix0 + iL, iy0, ix0 + iL, iy0, color);

				if (!current->svoLabel[1][0])
					marked.draw_line(ix0, iy0 + iL, ix0 + iL, iy0 + iL, color);
				if (!current->svoLabel[1][1])
					marked.draw_line(ix0 + iL, iy0 + iL, ix0 + iL, iy0 + iL, color);
				//draw [o]uter [h]orizontal similarity split lines (vertical lines)
				if (!current->shoLabel[0][0])
					marked.draw_line(ix0, iy0, ix0, iy0 + iL, color);
				if (!current->shoLabel[0][1])
					marked.draw_line(ix0 + iL, iy0, ix0 + iL, iy0 + iL, color);

				if (!current->shoLabel[1][0])
					marked.draw_line(ix0, iy0 + iL, ix0, iy0 + iL, color);
				if (!current->shoLabel[1][1])
					marked.draw_line(ix0 + iL, iy0 + iL, ix0 + iL, iy0 + iL, color);
				
			}

		}
		return marked;
	}
	CImg<unsigned char> get_marked_split_merged()
	{
		if (root == NULL) return CImg<unsigned char>();
		// copy image 
		CImg<unsigned char> marked = CImg<unsigned char>(512, 512, 1, 1, 0);
		const unsigned char color[] = { 128,128,128 };
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

			if (!current->isLeaf())//not leaf
			{
				Q.push(current->Qs[0][0]);
				Q.push(current->Qs[0][1]);
				Q.push(current->Qs[1][0]);
				Q.push(current->Qs[1][1]);
			}
			else //leaf
			{
				//draw [i]nner split lines
				if (current->siLabel[0][0] != current->siLabel[0][1])
					marked.draw_line(ix0 + iL / 2, iy0, ix0 + iL / 2, iy0 + iL / 2, color);
				if (current->siLabel[1][0] != current->siLabel[1][1])
					marked.draw_line(ix0 + iL / 2, iy0 + iL / 2, ix0 + iL / 2, iy0 + iL, color);

				if (current->siLabel[0][0] != current->siLabel[1][0])
					marked.draw_line(ix0, iy0 + iL / 2, ix0 + iL / 2, iy0 + iL / 2, color);
				if (current->siLabel[0][1] != current->siLabel[1][1])
					marked.draw_line(ix0 + iL / 2, iy0 + iL / 2, ix0 + iL, iy0 + iL / 2, color);
				//draw [o]uter [v]ertical similarity split lines (horizontal lines)
				if (!current->svoLabel[0][0])
					marked.draw_line(ix0, iy0, ix0 + iL, iy0, color);
				if (!current->svoLabel[0][1])
					marked.draw_line(ix0 + iL, iy0, ix0 + iL, iy0, color);

				if (!current->svoLabel[1][0])
					marked.draw_line(ix0, iy0 + iL, ix0 + iL, iy0 + iL, color);
				if (!current->svoLabel[1][1])
					marked.draw_line(ix0 + iL, iy0 + iL, ix0 + iL, iy0 + iL, color);
				//draw [o]uter [h]orizontal similarity split lines (vertical lines)
				if (!current->shoLabel[0][0])
					marked.draw_line(ix0, iy0, ix0, iy0 + iL, color);
				if (!current->shoLabel[0][1])
					marked.draw_line(ix0 + iL, iy0, ix0 + iL, iy0 + iL, color);

				if (!current->shoLabel[1][0])
					marked.draw_line(ix0, iy0 + iL, ix0, iy0 + iL, color);
				if (!current->shoLabel[1][1])
					marked.draw_line(ix0 + iL, iy0 + iL, ix0 + iL, iy0 + iL, color);
			}

		}
		return marked;
	}
private:

	void merge(Qadrant* q)
	{
		if (q == NULL) return;

		if (!q->isLeaf())
		{
			merge_quadrant_inside(q);
			
			merge(q->Qs[0][0]);
			merge(q->Qs[0][1]);
			merge(q->Qs[1][0]);
			merge(q->Qs[1][1]);
		}
		else //leaf
		{
			merge_quadrant_inside(q);
			merge_quadrant_outside(q);
		}
	}
	void merge_quadrant_inside(Qadrant* q)
	{
		if (!q->isLeaf())
		{
			if (similar(q->Qs[0][0]->img, q->Qs[0][1]->img))
			{
				q->siLabel[0][1] = 0;
			}
			if (similar(q->Qs[0][0]->img, q->Qs[1][0]->img))
			{
				q->siLabel[1][0] = 0;
			}
			if (similar(q->Qs[0][1]->img, q->Qs[1][1]->img))
			{
				q->siLabel[1][1] = q->siLabel[0][1];
			}
			if (similar(q->Qs[1][0]->img, q->Qs[1][1]->img))
			{
				if (q->siLabel[1][0] != 0)
					q->siLabel[1][0] = q->siLabel[1][1];
				else
					q->siLabel[1][1] = 0;
			}
		}
		else
		{
			q->siLabel[0][0] = 0;	q->siLabel[0][1] = 0;
			q->siLabel[1][0] = 0;	q->siLabel[1][1] = 0;
		}
		
	}
	void merge_quadrant_outside(Qadrant* q)
	{
		if (!q->isRoot() && q->isLeaf()) //process leaf quadrants only 
		{
			if (q->parent->Qs[0][0] == q)//top-left
			{
				const Qadrant* left = q->get_left();
				const Qadrant* top = q->get_top();
				if (left != NULL && left->isLeaf() && !similar(q->img, left->img)) q->shoLabel[0][0] = false;
				if (top != NULL && top->isLeaf() && !similar(q->img, top->img)) q->svoLabel[0][0] = false;
			}
			else if (q->parent->Qs[0][1] == q)//top-right
			{
				const Qadrant* right = q->get_right();
				const Qadrant* top = q->get_top();
				if (right != NULL && right->isLeaf() && !similar(q->img, right->img)) q->shoLabel[0][1] = false;
				if (top != NULL && top->isLeaf() && !similar(q->img, top->img)) q->svoLabel[0][1] = false;
			}
			else if (q->parent->Qs[1][0] == q)//bottom-left
			{
				const Qadrant* left = q->get_left();
				const Qadrant* bottom = q->get_bottom();
				if (left != NULL && left->isLeaf() && !similar(q->img, left->img)) q->shoLabel[1][0] = false;
				if (bottom != NULL && bottom->isLeaf() && !similar(q->img, bottom->img)) q->svoLabel[1][0] = false;
			}
			else if (q->parent->Qs[1][1] == q)//bottom-right
			{
				const Qadrant* right = q->get_right();
				const Qadrant* bottom = q->get_bottom();
				if (right != NULL && right->isLeaf() && !similar(q->img, right->img)) q->shoLabel[1][1] = false;
				if (bottom != NULL && bottom->isLeaf() && !similar(q->img, bottom->img)) q->svoLabel[1][1] = false;
			}
		}
	}
	void split_merge(Qadrant* q)
	{
		//if not homogeneious -> split
		if (!homogeneous(q->img))
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
			
			merge_quadrant_inside(q); //merges only the current quadrant beeing split
			
			split_merge(q->Qs[0][0]);
			split_merge(q->Qs[0][1]);
			split_merge(q->Qs[1][0]);
			split_merge(q->Qs[1][1]);
			
		}
	}
	void split(Qadrant* q)
	{
		//if not homogeneious -> split
		if (!homogeneous(q->img))
		{
			int ix0 = q->x0;
			int iy0 = q->y0;
			int iL = q->img.width();
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
	bool(*homogeneous)(const CImg<unsigned char>&);
	bool(*similar)(const CImg<unsigned char>&, const CImg<unsigned char>&);
};


