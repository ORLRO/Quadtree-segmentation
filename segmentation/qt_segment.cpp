#include "qt_segment.h"

using namespace cv;

qt_segment::qt_segment(const Mat_<unsigned char>& in_image, 
	bool(*in_homogeneous)(const Mat_<unsigned char>&), 
	bool(*in_similar)(const Mat_<unsigned char>&,const Mat_<unsigned char>&)) :
	image(in_image.clone()),
	homogeneous(in_homogeneous),
	similar(in_similar)
{
	//TODO: assume image size is power of 2 (what if not)
	root = new Quadrant(image, NULL, 0, 0, image.cols);
	//split on creation
	//cimg::tic();
	//split_merge(root);

	split(root); 
	merge(root);
	label(root);
	//cimg::toc();
}
qt_segment::~qt_segment()
{
	if (root != NULL)
	{
		delete root;
	}
}

Mat_<unsigned char> qt_segment::get_marked_split()
{
	if (root == NULL) return Mat_<unsigned char>();
	// copy image 
	Mat_<unsigned char> marked = image.clone();
	Scalar color(128, 128, 128);
	Scalar blk(0, 0, 0);
	std::queue <Quadrant*> Q;
	Q.push(root);
	//while there is at least one discovered node
	while (!Q.empty())
	{
		Quadrant* current = Q.front();
		int ix0 = current->x0;
		int iy0 = current->y0;
		int iL = current->width;


		Q.pop(); // removing the element at front
		if (!current->isLeaf())
		{
			//draw split lines
			line(marked, Point(ix0 + iL / 2, iy0), Point(ix0 + iL / 2, iy0 + iL), color); //vertical
			line(marked, Point(ix0, iy0 + iL / 2), Point(ix0 + iL, iy0 + iL / 2), color); //horizontal
			
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
				line(marked, Point(ix0 + iL / 2, iy0), Point(ix0 + iL / 2, iy0 + iL / 2), color);
			if (current->siLabel[1][0] != current->siLabel[1][1])
				line(marked, Point(ix0 + iL / 2, iy0 + iL / 2), Point(ix0 + iL / 2, iy0 + iL), color);

			if (current->siLabel[0][0] != current->siLabel[1][0])
				line(marked, Point(ix0, iy0 + iL / 2), Point(ix0 + iL / 2, iy0 + iL / 2), color);
			if (current->siLabel[0][1] != current->siLabel[1][1])
				line(marked, Point(ix0 + iL / 2, iy0 + iL / 2), Point(ix0 + iL, iy0 + iL / 2), color);

			//draw [o]uter [v]ertical similarity split lines (horizontal lines)
			if (!current->svoLabel[0][0])
				line(marked, Point(ix0, iy0), Point(ix0 + iL, iy0), color);
			if (!current->svoLabel[0][1])
				line(marked, Point(ix0 + iL, iy0), Point(ix0 + iL, iy0), color);

			if (!current->svoLabel[1][0])
				line(marked, Point(ix0, iy0 + iL), Point(ix0 + iL, iy0 + iL), color);
			if (!current->svoLabel[1][1])
				line(marked, Point(ix0 + iL, iy0 + iL), Point(ix0 + iL, iy0 + iL), color);
			//draw [o]uter [h]orizontal similarity split lines (vertical lines)
			if (!current->shoLabel[0][0])
				line(marked, Point(ix0, iy0), Point(ix0, iy0 + iL), color);
			if (!current->shoLabel[0][1])
				line(marked, Point(ix0 + iL, iy0), Point(ix0 + iL, iy0 + iL), color);

			if (!current->shoLabel[1][0])
				line(marked, Point(ix0, iy0 + iL), Point(ix0, iy0 + iL), color);
			if (!current->shoLabel[1][1])
				line(marked, Point(ix0 + iL, iy0 + iL), Point(ix0 + iL, iy0 + iL), color);

		}

	}
	return marked;
}
Mat_<unsigned char> qt_segment::get_marked_split_merged()
{
	if (root == NULL) return Mat_<unsigned char>();
	// copy image 
	Mat_<unsigned char> marked = Mat::zeros(512, 512, CV_8UC1);
	Scalar color(128, 128, 128);
	Scalar blk(0, 0, 0);
	std::queue <Quadrant*> Q;
	Q.push(root);
	//while there is at least one discovered node
	while (!Q.empty())
	{
		Quadrant* current = Q.front();
		Q.pop(); // removing the element at front

		int ix0 = current->x0;
		int iy0 = current->y0;
		int iL = current->width;

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
				line(marked, Point(ix0 + iL / 2, iy0), Point(ix0 + iL / 2, iy0 + iL / 2), color);
			if (current->siLabel[1][0] != current->siLabel[1][1])
				line(marked, Point(ix0 + iL / 2, iy0 + iL / 2), Point(ix0 + iL / 2, iy0 + iL), color);

			if (current->siLabel[0][0] != current->siLabel[1][0])
				line(marked, Point(ix0, iy0 + iL / 2), Point(ix0 + iL / 2, iy0 + iL / 2), color);
			if (current->siLabel[0][1] != current->siLabel[1][1])
				line(marked, Point(ix0 + iL / 2, iy0 + iL / 2), Point(ix0 + iL, iy0 + iL / 2), color);
			//draw [o]uter [v]ertical similarity split lines (horizontal lines)
			if (!current->svoLabel[0][0])
				line(marked, Point(ix0, iy0), Point(ix0 + iL, iy0), color);
			if (!current->svoLabel[0][1])
				line(marked, Point(ix0 + iL, iy0), Point(ix0 + iL, iy0), color);

			if (!current->svoLabel[1][0])
				line(marked, Point(ix0, iy0 + iL), Point(ix0 + iL, iy0 + iL), color);
			if (!current->svoLabel[1][1])
				line(marked, Point(ix0 + iL, iy0 + iL), Point(ix0 + iL, iy0 + iL), color);
			//draw [o]uter [h]orizontal similarity split lines (vertical lines)
			if (!current->shoLabel[0][0])
				line(marked, Point(ix0, iy0), Point(ix0, iy0 + iL), color);
			if (!current->shoLabel[0][1])
				line(marked, Point(ix0 + iL, iy0), Point(ix0 + iL, iy0 + iL), color);

			if (!current->shoLabel[1][0])
				line(marked, Point(ix0, iy0 + iL), Point(ix0, iy0 + iL), color);
			if (!current->shoLabel[1][1])
				line(marked, Point(ix0 + iL, iy0 + iL), Point(ix0 + iL, iy0 + iL), color);
		}
	}
	return marked;
}

Mat_<unsigned char> qt_segment::get_labeled()
{
	if (root == NULL) return Mat_<unsigned char>();

	Mat_<unsigned char> labeled = Mat::zeros(512, 512, CV_8U);//TODO fixed size!!
	Mat_<unsigned char> colored;
	std::queue <Quadrant*> Q;
	Q.push(root);
	//while there is at least one discovered node
	while (!Q.empty())
	{
		Quadrant* current = Q.front();
		Q.pop(); // removing the element at front

		int ix0 = current->x0;
		int iy0 = current->y0;
		int iL = current->width;

		if (!current->isLeaf())//not leaf
		{
			Q.push(current->Qs[0][0]);
			Q.push(current->Qs[0][1]);
			Q.push(current->Qs[1][0]);
			Q.push(current->Qs[1][1]);
		}
		else //leaf
		{
			Mat_<unsigned char> ROI = labeled(cv::Rect(ix0, iy0, iL, iL));
			
			ROI.setTo(Scalar((unsigned char)current->get_Label()->get_id() % 256));
			//cout << current->label->get_id() << '\t';
		}
	}
	//applyColorMap(labeled, colored, COLORMAP_HSV);
	return labeled;
}

void qt_segment::label(Quadrant * q)
{
	if (q == NULL) return;

	if (!q->isLeaf())
	{
		label(q->Qs[0][0]);
		label(q->Qs[0][1]);
		label(q->Qs[1][0]);
		label(q->Qs[1][1]);
	}
	else //leaf
	{
		vector<const Quadrant*> lefts = q->get_left_adjacencies();
		vector<const Quadrant*> tops = q->get_top_adjacencies();
		vector<const Quadrant*> all_adjacencies = lefts;
		bool anyOneSimilarToMe = false;
		all_adjacencies.insert(all_adjacencies.end(), tops.begin(), tops.end());
		for each (auto adj in all_adjacencies)
		{
			if (similar(q->get_quadrant_region(), adj->get_quadrant_region()))
			{
				if (!anyOneSimilarToMe)//yet
				{
					q->set_Label(adj->get_Label());
				}
				else
				{
					q->set_equevelant_Label_to(adj);
				}
				anyOneSimilarToMe = true;
			}
		}
		if(!anyOneSimilarToMe)
			{
				q->set_Label(Quadrant::Label::get_unusedLabel());
			}

	}
}
void qt_segment::merge(Quadrant * q)
{
	if (q == NULL) return;

	merge_quadrant_inside(q);

	if (!q->isLeaf())
	{
		merge(q->Qs[0][0]);
		merge(q->Qs[0][1]);
		merge(q->Qs[1][0]);
		merge(q->Qs[1][1]);
	}
	else //leaf
	{
		merge_quadrant_outside(q);
	}
}

void qt_segment::merge_quadrant_inside(Quadrant * q)
{
	if (!q->isLeaf())
	{
		if (similar(q->Qs[0][0]->get_quadrant_region(), q->Qs[0][1]->get_quadrant_region()))
		{
			q->siLabel[0][1] = 0;
		}
		if (similar(q->Qs[0][0]->get_quadrant_region(), q->Qs[1][0]->get_quadrant_region()))
		{
			q->siLabel[1][0] = 0;
		}
		if (similar(q->Qs[0][1]->get_quadrant_region(), q->Qs[1][1]->get_quadrant_region()))
		{
			q->siLabel[1][1] = q->siLabel[0][1];
		}
		if (similar(q->Qs[1][0]->get_quadrant_region(), q->Qs[1][1]->get_quadrant_region()))
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
void qt_segment::merge_quadrant_outside(Quadrant * q)
{
	if (!q->isRoot() && q->isLeaf()) //process leaf quadrants only 
	{
		if (q->isTopLeft())
		{
			const Quadrant* left = q->get_left();
			const Quadrant* top = q->get_top();
			if (left != NULL && left->isLeaf() && !similar(q->get_quadrant_region(), left->get_quadrant_region())) q->shoLabel[0][0] = false;
			if (top != NULL && top->isLeaf() && !similar(q->get_quadrant_region(), top->get_quadrant_region())) q->svoLabel[0][0] = false;
		}
		else if (q->isTopRight())
		{
			const Quadrant* right = q->get_right();
			const Quadrant* top = q->get_top();
			if (right != NULL && right->isLeaf() && !similar(q->get_quadrant_region(), right->get_quadrant_region())) q->shoLabel[0][1] = false;
			if (top != NULL && top->isLeaf() && !similar(q->get_quadrant_region(), top->get_quadrant_region())) q->svoLabel[0][1] = false;
		}
		else if (q->isBottomLeft())
		{
			const Quadrant* left = q->get_left();
			const Quadrant* bottom = q->get_bottom();
			if (left != NULL && left->isLeaf() && !similar(q->get_quadrant_region(), left->get_quadrant_region())) q->shoLabel[1][0] = false;
			if (bottom != NULL && bottom->isLeaf() && !similar(q->get_quadrant_region(), bottom->get_quadrant_region())) q->svoLabel[1][0] = false;
		}
		else if (q->isBottomRight())
		{
			const Quadrant* right = q->get_right();
			const Quadrant* bottom = q->get_bottom();
			if (right != NULL && right->isLeaf() && !similar(q->get_quadrant_region(), right->get_quadrant_region())) q->shoLabel[1][1] = false;
			if (bottom != NULL && bottom->isLeaf() && !similar(q->get_quadrant_region(), bottom->get_quadrant_region())) q->svoLabel[1][1] = false;
		}
	}
}

void qt_segment::split_merge(Quadrant * q)
{
	//double *mx = nullptr, *mn = nullptr;
	//minMaxLoc(q->img, mx, mn, NULL, NULL);
	//return mx - mn < 90;
	//if not homogeneous -> split
	if (!homogeneous(q->get_quadrant_region()))
	{
		int ix0 = q->x0;
		int iy0 = q->y0;
		int iL = q->width;

		//top-left
		//q->Qs[0][0] = new Quadrant(q->img(Rect(0, 0, iL / 2 - 1, iL / 2 - 1)), q, ix0, iy0);
		q->Qs[0][0] = new Quadrant(image, q, ix0, iy0, iL / 2);
		//top-right
		//q->Qs[0][1] = new Quadrant(q->img(Rect(iL / 2, 0, iL - 1, iL / 2 - 1)), q, ix0 + iL / 2, iy0);
		q->Qs[0][1] = new Quadrant(image, q, ix0 + iL / 2, iy0, iL / 2);
		//bottom-left
		//q->Qs[1][0] = new Quadrant(q->img(Rect(0, iL / 2, iL / 2 - 1, iL - 1)), q, ix0, iy0 + iL / 2);
		q->Qs[1][0] = new Quadrant(image, q, ix0, iy0 + iL / 2, iL / 2);
		//bottom-right
		//q->Qs[1][1] = new Quadrant(q->img(Rect(iL / 2, iL / 2, iL - 1, iL - 1)), q, ix0 + iL / 2, iy0 + iL / 2);
		q->Qs[1][1] = new Quadrant(image, q, ix0 + iL / 2, iy0 + iL / 2, iL / 2);
		merge_quadrant_inside(q); //merges only the current quadrant being split

		split_merge(q->Qs[0][0]);
		split_merge(q->Qs[0][1]);
		split_merge(q->Qs[1][0]);
		split_merge(q->Qs[1][1]);

	}
}
void qt_segment::split(Quadrant * q)
{
	//if not homogeneous -> split
	if (!homogeneous(q->get_quadrant_region()))
	{
		int ix0 = q->x0;
		int iy0 = q->y0;
		int iL = q->width;
		//top-left
		//q->Qs[0][0] = new Quadrant(q->img(Rect(0, 0, iL/2, iL / 2)), q, ix0, iy0);
		q->Qs[0][0] = new Quadrant(image, q, ix0, iy0, iL / 2);
		split(q->Qs[0][0]);
		//top-right
		//q->Qs[0][1] = new Quadrant(q->img(Rect(iL / 2, 0, iL / 2, iL / 2)), q, ix0 + iL / 2, iy0);
		q->Qs[0][1] = new Quadrant(image, q, ix0 + iL / 2, iy0, iL / 2);
		split(q->Qs[0][1]);
		//bottom-left
		//q->Qs[1][0] = new Quadrant(q->img(Rect(0, iL / 2, iL / 2, iL / 2)), q, ix0, iy0 + iL / 2);
		q->Qs[1][0] = new Quadrant(image, q, ix0, iy0 + iL / 2, iL / 2);
		split(q->Qs[1][0]);
		//bottom-right
		//q->Qs[1][1] = new Quadrant(q->img(Rect(iL / 2, iL / 2, iL/ 2, iL/ 2)), q, ix0 + iL / 2, iy0 + iL / 2);
		q->Qs[1][1] = new Quadrant(image, q, ix0 + iL / 2, iy0 + iL / 2, iL / 2);
		split(q->Qs[1][1]);
	}

}
