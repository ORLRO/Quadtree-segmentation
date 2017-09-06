#include "qt_segment.h"

qt_segment::qt_segment(CImg<unsigned char>& in_image, bool(*in_homogeneous)(const CImg<unsigned char>&), bool(*in_similar)(const CImg<unsigned char>&, const CImg<unsigned char>&)) :
	image(in_image),
	homogeneous(in_homogeneous),
	similar(in_similar)
{
	//TODO: assume image size is power of 2 (what if not)
	root = new Quadrant(image, NULL, 0, 0);
	//split on creation
	cimg::tic();
	//split_merge(root);

	split(root); merge(root);
	cimg::toc();
}
qt_segment::~qt_segment()
{
	if (root != NULL)
	{
		delete root;
	}
}

CImg<unsigned char> qt_segment::get_marked_split()
{
	if (root == NULL) return CImg<unsigned char>();
	// copy image 
	CImg<unsigned char> marked = image;
	const unsigned char color[] = { 128,128,128 };
	const unsigned char blk[] = { 0,0,0 };
	std::queue <Quadrant*> Q;
	Q.push(root);
	//while there is at least one discovered node
	while (!Q.empty())
	{
		Quadrant* current = Q.front();
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
CImg<unsigned char> qt_segment::get_marked_split_merged()
{
	if (root == NULL) return CImg<unsigned char>();
	// copy image 
	CImg<unsigned char> marked = CImg<unsigned char>(512, 512, 1, 1, 0);
	const unsigned char color[] = { 128,128,128 };
	const unsigned char blk[] = { 0,0,0 };
	std::queue <Quadrant*> Q;
	Q.push(root);
	//while there is at least one discovered node
	while (!Q.empty())
	{
		Quadrant* current = Q.front();
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
		bool anyoneSimilarToMe = false;
		vector<const Quadrant*> lefts = q->get_left_adjacencies();
		vector<const Quadrant*> tops = q->get_top_adjacencies();
		vector<const Quadrant*> all_adjacencies = lefts;
		all_adjacencies.insert(all_adjacencies.end(), tops.begin(), tops.end());
		for each (auto adj in all_adjacencies)
		{
			if (similar(q->img, adj->img))
			{
				Label newlabel;
				q->label = newlabel;
			}
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
void qt_segment::merge_quadrant_outside(Quadrant * q)
{
	if (!q->isRoot() && q->isLeaf()) //process leaf quadrants only 
	{
		if (q->isTopLeft())
		{
			const Quadrant* left = q->get_left();
			const Quadrant* top = q->get_top();
			if (left != NULL && left->isLeaf() && !similar(q->img, left->img)) q->shoLabel[0][0] = false;
			if (top != NULL && top->isLeaf() && !similar(q->img, top->img)) q->svoLabel[0][0] = false;
		}
		else if (q->isTopRight())
		{
			const Quadrant* right = q->get_right();
			const Quadrant* top = q->get_top();
			if (right != NULL && right->isLeaf() && !similar(q->img, right->img)) q->shoLabel[0][1] = false;
			if (top != NULL && top->isLeaf() && !similar(q->img, top->img)) q->svoLabel[0][1] = false;
		}
		else if (q->isBottomLeft())
		{
			const Quadrant* left = q->get_left();
			const Quadrant* bottom = q->get_bottom();
			if (left != NULL && left->isLeaf() && !similar(q->img, left->img)) q->shoLabel[1][0] = false;
			if (bottom != NULL && bottom->isLeaf() && !similar(q->img, bottom->img)) q->svoLabel[1][0] = false;
		}
		else if (q->isBottomRight())
		{
			const Quadrant* right = q->get_right();
			const Quadrant* bottom = q->get_bottom();
			if (right != NULL && right->isLeaf() && !similar(q->img, right->img)) q->shoLabel[1][1] = false;
			if (bottom != NULL && bottom->isLeaf() && !similar(q->img, bottom->img)) q->svoLabel[1][1] = false;
		}
	}
}

void qt_segment::split_merge(Quadrant * q)
{
	//if not homogeneious -> split
	if (!homogeneous(q->img))
	{
		int ix0 = q->x0;
		int iy0 = q->y0;
		int iL = q->img.width();

		//top-left
		q->Qs[0][0] = new Quadrant(q->img.get_crop(0, 0, iL / 2 - 1, iL / 2 - 1), q, ix0, iy0);
		//top-right
		q->Qs[0][1] = new Quadrant(q->img.get_crop(iL / 2, 0, iL - 1, iL / 2 - 1), q, ix0 + iL / 2, iy0);
		//bottom-left
		q->Qs[1][0] = new Quadrant(q->img.get_crop(0, iL / 2, iL / 2 - 1, iL - 1), q, ix0, iy0 + iL / 2);
		//bottom-right
		q->Qs[1][1] = new Quadrant(q->img.get_crop(iL / 2, iL / 2, iL - 1, iL - 1), q, ix0 + iL / 2, iy0 + iL / 2);

		merge_quadrant_inside(q); //merges only the current quadrant beeing split

		split_merge(q->Qs[0][0]);
		split_merge(q->Qs[0][1]);
		split_merge(q->Qs[1][0]);
		split_merge(q->Qs[1][1]);

	}
}
void qt_segment::split(Quadrant * q)
{
	//if not homogeneious -> split
	if (!homogeneous(q->img))
	{
		int ix0 = q->x0;
		int iy0 = q->y0;
		int iL = q->img.width();
		//top-left
		q->Qs[0][0] = new Quadrant(q->img.get_crop(0, 0, iL / 2 - 1, iL / 2 - 1), q, ix0, iy0);
		split(q->Qs[0][0]);
		//top-right
		q->Qs[0][1] = new Quadrant(q->img.get_crop(iL / 2, 0, iL - 1, iL / 2 - 1), q, ix0 + iL / 2, iy0);
		split(q->Qs[0][1]);
		//bottom-left
		q->Qs[1][0] = new Quadrant(q->img.get_crop(0, iL / 2, iL / 2 - 1, iL - 1), q, ix0, iy0 + iL / 2);
		split(q->Qs[1][0]);
		//bottom-right
		q->Qs[1][1] = new Quadrant(q->img.get_crop(iL / 2, iL / 2, iL - 1, iL - 1), q, ix0 + iL / 2, iy0 + iL / 2);
		split(q->Qs[1][1]);
	}

}
