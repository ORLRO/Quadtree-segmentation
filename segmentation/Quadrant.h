#pragma once

#include <opencv2/core.hpp>
#include "Label.h"
#include <stack> 
#include <vector>

using namespace cv;
using namespace std;

class Quadrant
{
public:
	Quadrant(Mat_<unsigned char>& in_img, const Quadrant* in_parent, 
		int in_x0, int in_y0, int in_width);
	~Quadrant();
private:
	const Quadrant* get_first_right_parent(stack<bool>& vertical_moves) const;
	const Quadrant* get_first_left_parent(stack<bool>& vertical_moves) const;
	const Quadrant* get_first_top_parent(stack<bool>& horizontal_moves) const;
	const Quadrant* get_first_bottom_parent(stack<bool>& horizontal_moves) const;

	const Quadrant* get_immediate_left() const;
	const Quadrant* get_immediate_right() const;
	const Quadrant* get_immediate_top() const;
	const Quadrant* get_immediate_bottom() const;

	const Quadrant* get_right_child(stack<bool>& vertical_moves) const;
	const Quadrant* get_left_child(stack<bool>& vertical_moves) const;
	const Quadrant* get_top_child(stack<bool>& horizontal_moves) const;
	const Quadrant* get_bottom_child(stack<bool>& horizontal_moves) const;

	//finds all child nodes touching the right edge  
	void get_right_children(vector<const Quadrant*>& results) const;
	//finds all child nodes touching the bottom edge 
	void get_bottom_children(vector<const Quadrant*>& results) const;
public:
	Mat_<unsigned char> get_quadrant_region() const;
	vector<const Quadrant*> get_left_adjacencies()const;
	vector<const Quadrant*> get_top_adjacencies()const;
	const Quadrant* get_left() const;
	const Quadrant* get_right() const;
	const Quadrant* get_top() const;
	const Quadrant* get_bottom() const;

	bool isRoot() const;
	bool isLeaf() const;
	///carefull: check !isRoot() before using those
	//TODO don't use parent-> must be done after each quadranr represent itself only
	bool isTopLeft() const;
	bool isTopRight() const;
	bool isBottomLeft() const;
	bool isBottomRight() const;
	bool isTop() const;
	bool isBottom() const;
	bool isRight() const;
	bool isLeft() const;
	
	void set_Label(Label*);
	Label* get_Label() const;

	void set_equevelant_Label_to(const Quadrant*);
public:
	//TODO: each quadrant must represent itself not its children
	Quadrant* Qs[2][2]; //children quadrants in 2x2 array
	unsigned char siLabel[2][2];//label similar quadrant with same number (i: inner)
	bool svoLabel[2][2]; // is quadrant [s]imilar to its [v]ertical [o]uter neighbor 
	bool shoLabel[2][2]; // is quadrant [s]imilar to its [h]orizontal [o]uter neighbor 
	int x0, y0; // top left pixel coordinates in the original image
	int width; //width of the quadrant in pixels
	
private:
	Label* label; //for connected components labeling - CCL
	Mat_<unsigned char>& img; // the whole image to be segmented 
	const Quadrant* parent; // parent
	
};

