#pragma once

#include "Quadrant.h"
#include "Label.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
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
	qt_segment(Mat_<unsigned char>& in_image, 
		bool(*in_homogeneous)(const Mat_<unsigned char>&),
		bool(*in_similar)(const Mat_<unsigned char>&, const Mat_<unsigned char>&));
	~qt_segment();
	Mat_<unsigned char> get_marked_split();
	Mat_<unsigned char> get_marked_split_merged();
private:	
	void label(Quadrant* q);
	void merge(Quadrant* q);
	void merge_quadrant_inside(Quadrant* q);
	void merge_quadrant_outside(Quadrant* q);
	void split_merge(Quadrant* q);
	void split(Quadrant* q);
private:
	Quadrant* root;
	Mat_<unsigned char>& image;
	unsigned char threshold;
	
	//function to calculate the homogeneity of Image
	bool(*homogeneous)(const Mat_<unsigned char>&);
	//function to calculate the similarity between 2 Images
	bool(*similar)(const Mat_<unsigned char>&, const Mat_<unsigned char>&);
};


