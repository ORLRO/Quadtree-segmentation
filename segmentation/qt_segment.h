#pragma once

#include "Quadrant.h"
#include "Label.h"

#include <queue>
#include <algorithm>
#include <string>
#include <iostream>
#include <stack> 
#include <vector>
using namespace std;

class qt_segment
{	
public:
	qt_segment(CImg<unsigned char>& in_image, 
		bool(*in_homogeneous)(const CImg<unsigned char>&),
		bool(*in_similar)(const CImg<unsigned char>&, const CImg<unsigned char>&));
	~qt_segment();
	CImg<unsigned char> get_marked_split();
	CImg<unsigned char> get_marked_split_merged();
private:	
	void label(Quadrant* q);
	void merge(Quadrant* q);
	void merge_quadrant_inside(Quadrant* q);
	void merge_quadrant_outside(Quadrant* q);
	void split_merge(Quadrant* q);
	void split(Quadrant* q);
private:
	Quadrant* root;
	CImg<unsigned char>& image;
	unsigned char threshold;
	
	//function to calculate the homogeneity of Image
	bool(*homogeneous)(const CImg<unsigned char>&); 
	//function to calculate the similarity between 2 Images
	bool(*similar)(const CImg<unsigned char>&, const CImg<unsigned char>&);
};


